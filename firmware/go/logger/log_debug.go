//go:build debug

// logger only logs msgs to usb out
// when in debug mode
package logger

func Log(args ...any) {
	for i, a := range args {
		if i > 0 {
			print(" ")
		}
		print(a)
	}
	print("\n")
}
