//go:build wifi

package main

import (
	"net/netip"
	"strconv"
	"time"

	"log/slog"
	"machine"

	"github.com/michael-duren/picoweather-watch/firmware/go/logger"
	"github.com/soypat/cyw43439"
	"github.com/soypat/cyw43439/examples/cywnet"
	"github.com/soypat/lneto/tcp"
)

var (
	cystack  *cywnet.Stack
	targetAP netip.AddrPort
)

func initSender() error {
	target, err := netip.ParseAddrPort(ServerAddr)
	if err != nil {
		return err
	}
	targetAP = target

	devcfg := cyw43439.DefaultWifiConfig()
	devcfg.Logger = slog.New(slog.NewTextHandler(machine.Serial, &slog.HandlerOptions{
		Level: slog.LevelInfo,
	}))

	logger.Log("initializing WiFi, SSID:", WifiSSID)
	cystack, err = cywnet.NewConfiguredPicoWithStack(WifiSSID, WifiPassword, devcfg, cywnet.StackConfig{
		Hostname:    Hostname,
		MaxTCPPorts: 1,
	})
	if err != nil {
		return err
	}

	// Start the network stack loop in a goroutine.
	go loopForeverStack(cystack)

	dhcpResults, err := cystack.SetupWithDHCP(cywnet.DHCPConfig{})
	if err != nil {
		return err
	}
	logger.Log("WiFi connected, IP:", dhcpResults.AssignedAddr.String())

	// Blink LED to signal successful WiFi connection.
	dev := cystack.Device()
	dev.GPIOSet(0, true)
	time.Sleep(time.Second)
	dev.GPIOSet(0, false)

	return nil
}

// send transmits temperature and humidity over TCP to ServerAddr.
// Data is sent as "T:<degrees>:H:<percent>\n" text format.
func send(temperature, humidity int32) error {
	lstack := cystack.LnetoStack()
	const pollTime = 5 * time.Millisecond
	rstack := lstack.StackRetrying(pollTime)

	var conn tcp.Conn
	err := conn.Configure(tcp.ConnConfig{
		RxBuf:             make([]byte, 256),
		TxBuf:             make([]byte, 256),
		TxPacketQueueSize: 2,
	})
	if err != nil {
		return err
	}

	lport := uint16(lstack.Prand32()>>17) + 1
	err = rstack.DoDialTCP(&conn, lport, targetAP, 6*time.Second, 2)
	if err != nil {
		conn.Abort()
		return err
	}

	tempC := temperature / 1000
	humPct := humidity / 1000
	msg := "T:" + strconv.Itoa(int(tempC)) + ":H:" + strconv.Itoa(int(humPct)) + "\n"
	_, err = conn.Write([]byte(msg))

	closeErr := conn.Close()
	// Wait for TCP close handshake.
	for i := 0; i < 20 && !conn.State().IsClosed(); i++ {
		time.Sleep(5 * time.Millisecond)
	}
	conn.Abort()

	if err != nil {
		return err
	}
	return closeErr
}

func loopForeverStack(stack *cywnet.Stack) {
	for {
		send, recv, _ := stack.RecvAndSend()
		if send == 0 && recv == 0 {
			time.Sleep(5 * time.Millisecond)
		}
	}
}
