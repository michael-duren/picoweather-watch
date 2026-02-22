#include <boards/pico_w.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

typedef struct {
    float humidity;
    float temp_c;
} dht_reading;

/**
 * @brief Reads temp/humidity from dht11
 *
 * @param result Pointer to result struct
 */
void read_from_dht(dht_reading* result);

void init() {
    puts("inializing architecture");

    int rc = stdio_init_all();
    if (rc != SUCCESS) {
        printf("unable to initialize usb io, error code: %d\n", rc);
        exit(1);
    }

    rc = cyw43_arch_init();
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
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
