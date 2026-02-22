#include <stdio.h>
#include <stdlib.h>

#include "dht11.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int64_t alarm_callback(alarm_id_t id, void* user_data) {
    // Put your timeout handler code in here
    return 0;
}

#define DHT_PIN 15
#define MAX_TIMINGS 85
#define SUCCESS 0

void init() {
    puts("inializing architecture");

    int res = stdio_init_all();
    if (!res) {
        printf("unable to initialize usb io, error code: %d\n", res);
        exit(1);
    }
    sleep_ms(2000);

    int rc = cyw43_arch_init();
    if (rc != SUCCESS) {
        printf("unable to initialize lwIP stack, error code: %d\n", rc);
        exit(1);
    }

    printf("turning on LED to signal successful initialization");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
}

int main() {
    printf("starting firmware");
    init();

    while (true) {
        printf("read from dht\n");
        dht11_reading result = {.temperature = 0.0, .humidity = 0.0};
        int rc = read_from_dht(&result);

        if (rc != SUCCESS) {
            printf(
                "unable to read from dht11, error code: %d. turning led off to "
                "signal failure",
                rc);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        sleep_ms(1000);
    }
}
