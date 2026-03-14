//go:build debug

// logger only logs msgs to usb out 
// when in debug mode
package logger


func Log(args ...any) {
	println(args...)
}
