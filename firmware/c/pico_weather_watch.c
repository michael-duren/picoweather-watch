#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dht11.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define SUCCESS 0

void init() {
    puts("inializing architecture");

    int res = stdio_init_all();
    if (!res) {
        printf("unable to initialize usb io, error code: %d\n", res);
        exit(1);
    }
    sleep_ms(2000);

    // init for dht
    gpio_init(DHT_PIN);

    int rc = cyw43_arch_init();
    if (rc != SUCCESS) {
        printf("unable to initialize lwIP stack, error code: %d\n", rc);
        exit(1);
    }

    printf("turning on LED to signal successful initialization");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(1000);
    printf("turning off to signifiy end of initialization");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

int main() {
    printf("starting firmware");
    init();

    while (1) {
        printf("read from dht\n");
        dht11_reading reading;
        int rc = read_from_dht11(&reading);

        if (rc != SUCCESS) {
            printf(
                "unable to read from dht11, error code: %d. turning led off to "
                "signal failure\n",
                rc);

            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        } else {
            uint8_t packet[4] = {0xAA, (uint8_t)reading.temperature,
                                 (uint8_t)reading.humidity, 0x55};
            stdio_put_string((char*)packet, 4, 0, 0);
            // printf("reading from dht11: \n");
            // printf("temperature: %f\n", reading.temperature);
            // printf("humidity: %f\n", reading.humidity);
        }

        sleep_ms(2000);
    }
}
