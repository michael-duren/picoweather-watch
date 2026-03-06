#include <pico/time.h>
#include <stdint.h>
void timer() {
    // Timer example code - This example fires off the callback after 2000ms
    // add_alarm_in_ms(2000, alarm_callback, NULL, false);
    // For more examples of timer use see
    // https://github.com/raspberrypi/pico-examples/tree/master/timer

    //
}

int64_t alarm_callback(alarm_id_t id, void* user_data) {
    // Put your timeout handler code in here
    return 0;
}
