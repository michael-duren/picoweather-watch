package main

import (
	"context"
	"errors"
	"fmt"
	"io"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"sync"
	"syscall"
	"time"

	"go.bug.st/serial"
)

// WeatherData represents a reading from the DHT11 sensor
type WeatherData struct {
	Temperature float64
	Humidity    float64
	Timestamp   time.Time
}

// WeatherReader handles reading from USB serial port
type WeatherReader struct {
	io.ReadCloser
	currentData WeatherData
	mu          sync.RWMutex
	logger      *slog.Logger
}

const (
	FrameStart = 0xAA
	FrameEnd   = 0x55
	FrameSize  = 4
)

// NewWeatherReader creates a new USB serial reader
func NewWeatherReader(logger *slog.Logger) (*WeatherReader, error) {
	path, err := getReaderPath()
	if err != nil {
		logger.Warn("multiple usb devices were found, continuing with first found: ", "path", path)
	}
	port, err := serial.Open(path, &serial.Mode{
		BaudRate: 115200,
		DataBits: 8,
		Parity:   serial.NoParity,
		StopBits: serial.OneStopBit,
	})
	if err != nil {
		panic("unable to open file to read from, make sure the pico is connected")
	}

	return &WeatherReader{
		ReadCloser: port,
		logger:     logger,
		currentData: WeatherData{
			Timestamp: time.Now(),
		},
	}, nil
}

func getReaderPath() (string, error) {
	patterns := []string{
		"/dev/ttyACM*",       // Linux - USB CDC (Pico)
		"/dev/ttyUSB*",       // Linux - USB-to-serial adapters
		"/dev/tty.usbmodem*", // macOS
	}

	var matches []string
	for _, p := range patterns {
		m, _ := filepath.Glob(p)
		matches = append(matches, m...)
	}

	if len(matches) == 0 {
		return "", errors.New("no USB serial device found, is the Pico connected?")
	}
	if len(matches) > 1 {
		return matches[0], fmt.Errorf("multiple devices found %v, using %s", matches, matches[0])
	}
	return matches[0], nil
}

// Start begins reading from the USB port
func (wr *WeatherReader) Start(ctx context.Context) error {
	buffer := make([]byte, 256)
	frameBuffer := make([]byte, 0, FrameSize)

	wr.logger.Info("starting USB reader")

	for {
		select {
		case <-ctx.Done():
			wr.logger.Info("stopping USB reader")
			return ctx.Err()
		default:
			n, err := wr.Read(buffer)
			if err != nil {
				if errors.Is(err, io.EOF) {
					continue
				}
				wr.logger.Error("error reading from serial port", "error", err)
				time.Sleep(time.Second)
				continue
			}

			for i := range n {
				b := buffer[i]

				// Start of frame
				if b == FrameStart && len(frameBuffer) == 0 {
					frameBuffer = append(frameBuffer, b)
					continue
				}

				// Building frame
				if len(frameBuffer) > 0 && len(frameBuffer) < FrameSize {
					frameBuffer = append(frameBuffer, b)

					// Complete frame received
					if len(frameBuffer) == FrameSize {
						if frameBuffer[3] == FrameEnd {
							// update data being
							wr.processFrame(frameBuffer)
						} else {
							wr.logger.Warn("invalid frame end marker", "frame", frameBuffer)
						}
						frameBuffer = frameBuffer[:0]
					}
				}
			}
		}
	}
}

// processFrame parses a complete frame and updates current data
func (wr *WeatherReader) processFrame(frame []byte) {
	if len(frame) != FrameSize {
		return
	}

	// Frame format: {0xAA, temp, humidity, 0x55}
	temperature := float64(frame[1])
	humidity := float64(frame[2])

	wr.mu.Lock()
	wr.currentData = WeatherData{
		Temperature: temperature,
		Humidity:    humidity,
		Timestamp:   time.Now(),
	}
	wr.mu.Unlock()

	wr.logger.Info("received weather data",
		"temperature_c", temperature,
		"humidity_pct", humidity,
	)
}

// GetCurrentData returns the most recent reading
func (wr *WeatherReader) GetCurrentData() WeatherData {
	wr.mu.RLock()
	defer wr.mu.RUnlock()
	return wr.currentData
}

// Close closes the serial port
func (wr *WeatherReader) Close() error {
	if wr.ReadCloser != nil {
		return wr.ReadCloser.Close()
	}
	return nil
}

// MetricsHandler serves Prometheus metrics
func MetricsHandler(reader *WeatherReader) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		data := reader.GetCurrentData()

		// Prometheus text format
		fmt.Fprintf(w, "# HELP weather_temperature_celsius Current temperature in Celsius\n")
		fmt.Fprintf(w, "# TYPE weather_temperature_celsius gauge\n")
		fmt.Fprintf(w, "weather_temperature_celsius %.2f\n", data.Temperature)

		fmt.Fprintf(w, "# HELP weather_humidity_percent Current relative humidity percentage\n")
		fmt.Fprintf(w, "# TYPE weather_humidity_percent gauge\n")
		fmt.Fprintf(w, "weather_humidity_percent %.2f\n", data.Humidity)

		fmt.Fprintf(w, "# HELP weather_last_update_timestamp_seconds Timestamp of last sensor reading\n")
		fmt.Fprintf(w, "# TYPE weather_last_update_timestamp_seconds gauge\n")
		fmt.Fprintf(w, "weather_last_update_timestamp_seconds %d\n", data.Timestamp.Unix())
	}
}

// HealthHandler serves health check endpoint
func HealthHandler(reader *WeatherReader) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		data := reader.GetCurrentData()

		// Consider stale if no update in 30 seconds
		if time.Since(data.Timestamp) > 30*time.Second {
			w.WriteHeader(http.StatusServiceUnavailable)
			fmt.Fprintf(w, "stale data: last update %s ago\n", time.Since(data.Timestamp))
			return
		}

		w.WriteHeader(http.StatusOK)
		fmt.Fprintf(w, "ok\n")
	}
}

func main() {
	// Initialize structured logger
	logger := slog.New(slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
		Level: slog.LevelInfo,
	}))

	// Get HTTP port from environment or use default
	httpPort := os.Getenv("HTTP_PORT")
	if httpPort == "" {
		httpPort = "8080"
	}

	logger.Info("starting weather reader service",
		"http_port", httpPort,
	)

	// Create reader
	reader, err := NewWeatherReader(logger)
	if err != nil {
		logger.Error("failed to create reader", "error", err)
		os.Exit(1)
	}

	defer reader.Close()

	// Create context for graceful shutdown
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	// Start USB reader in goroutine
	go func() {
		if err := reader.Start(ctx); err != nil && !errors.Is(err, context.Canceled) {
			logger.Error("reader error", "error", err)
		}
	}()

	// Setup HTTP server
	mux := http.NewServeMux()
	mux.HandleFunc("/metrics", MetricsHandler(reader))
	mux.HandleFunc("/health", HealthHandler(reader))

	server := &http.Server{
		Addr:         ":" + httpPort,
		Handler:      mux,
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout:  120 * time.Second,
	}

	// Start HTTP server in goroutine
	go func() {
		logger.Info("starting HTTP server", "addr", server.Addr)
		if err := server.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
			logger.Error("HTTP server error", "error", err)
		}
	}()

	// Wait for interrupt signal
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt, syscall.SIGTERM)
	<-sigChan

	logger.Info("shutting down gracefully...")

	// Shutdown HTTP server
	shutdownCtx, shutdownCancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer shutdownCancel()

	if err := server.Shutdown(shutdownCtx); err != nil {
		logger.Error("error shutting down HTTP server", "error", err)
	}

	// shutdown reader
	cancel()

	logger.Info("shutdown complete")
}
