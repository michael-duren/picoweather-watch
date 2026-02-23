#include "dht11.h"

#include <hardware/gpio.h>
#include <stdio.h>

#include "pico/stdlib.h"

/**
 * @brief Sets the output of the pico to 3.3v
 */
void set_high() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 1);
}
/**
 * @brief sets the output to ground
 */
void set_low() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
}

/**
 * @brief sets high impedence
 */
void set_input() { gpio_set_dir(DHT_PIN, GPIO_IN); }

/**
 * @brief sends start signal (5.2) section of
 * dht11 data sheet
 */
// TODO:
void send_start_signal() {}

/**
 * @brief Reads temp/humidity from dht11
 *
 * @param result Pointer to result struct
 */
int read_from_dht(dht11_reading* result) {
    printf("simulate read from dht11\n");

    result->humidity = 35.0;
    result->temperature = 77;
    //

    return 0;
}
