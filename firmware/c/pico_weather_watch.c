#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht11.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define SUCCESS 0

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

void init() {
    LOG("initializing architecture\n");
    int res = stdio_init_all();
    if (!res) {
        LOG("unable to initialize usb io, error code: %d\n", res);
        exit(1);
    }
    sleep_ms(2000);

    gpio_init(DHT_PIN);

    int rc = cyw43_arch_init();
    if (rc != SUCCESS) {
        LOG("unable to initialize lwIP stack, error code: %d\n", rc);
        exit(1);
    }

    LOG("turning on LED to signal successful initialization\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(1000);
    LOG("turning off to signify end of initialization\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

int main() {
    LOG("starting firmware\n");
    init();

    while (1) {
        LOG("read from dht\n");
        dht11_reading reading;
        int rc = read_from_dht11(&reading);

        if (rc != SUCCESS) {
            LOG("unable to read from dht11, error code: %d\n", rc);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        } else {
            uint8_t packet[4] = {0xAA, (uint8_t)reading.temperature,
                                 (uint8_t)reading.humidity, 0x55};
            stdio_put_string((char *)packet, 4, 0, 0);
            LOG("temperature: %f\n", reading.temperature);
            LOG("humidity: %f\n", reading.humidity);
        }

        sleep_ms(2000);
    }
}
