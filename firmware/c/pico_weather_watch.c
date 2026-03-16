#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dht11.h"
#include "logger.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define SUCCESS 0

void init() {
    stdio_init_all();
    sleep_ms(2000);

    gpio_init(DHT_PIN);

    int rc = cyw43_arch_init();
    sleep_ms(2000);
    if (rc != SUCCESS) {
        logger_err("unable to initialize lwIP stack, error code: %d\n", rc);
        exit(1);
    }

    logger_info("turning on LED to signal successful initialization\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(1000);
    logger_info("turning off to signify end of initialization\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

int main() {
    logger_info("starting firmware\n");
    init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    while (1) {
        logger_info("read from dht\n");
        dht11_reading reading;
        int rc = read_from_dht11(&reading);

        if (rc != SUCCESS) {
            logger_warn("unable to read from dht11, error code: %d\n", rc);
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        } else {
            uint8_t packet[4] = {0xAA, (uint8_t)reading.temperature,
                                 (uint8_t)reading.humidity, 0x55};
            stdio_put_string((char*)packet, 4, 0, 0);
            logger_info("temperature: %f\n", reading.temperature);
            logger_info("humidity: %f\n", reading.humidity);
        }

        sleep_ms(3000);
    }
}
