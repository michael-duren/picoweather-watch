#include "dht11.h"

#include <hardware/gpio.h>
#include <stdio.h>

#include "logger.h"
#define ERR_DHT_READ -1
#define ERR_DHT_CHECKSUM -2

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
 * @brief sets high impedence with pull-up
 */
void set_input() {
    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
}

/**
 * @brief sends start signal (5.2) section of
 * dht11 data sheet
 */
void send_start_signal() {
    // set high to low
    set_high();
    sleep_us(100);  // stabilize
    set_low();
    // wait at least 18ms (per doc)
    sleep_ms(20);

    set_input();

    // waiting 20-40us for dht response
    sleep_us(40);

    // Check if DHT11 is responding
    int pin_state = gpio_get(DHT_PIN);
    logger_info(
        "after start signal, pin state: %d (should be 0 if DHT11 responding)\n",
        pin_state);
}

/**
 * @brief Reads temp/humidity from dht11
 *
 * @param result Pointer to result struct
 */
int read_from_dht11(dht11_reading* result) {
    int data[5] = {0, 0, 0, 0, 0};
    uint last = 1;
    uint j = 0;

    send_start_signal();
    int errorCode = 0;

    // read dht11 response (5.3) from data sheet + 40 bits
    for (uint i = 0; i < MAX_TIMINGS; i++) {
        uint count = 0;
        while (gpio_get(DHT_PIN) == last) {
            count++;
            sleep_us(1);
            // if nothing changed something went wrong
            if (count == 255) {
                logger_err(
                    "reached 255 iterations while reading from dht, something "
                    "is wrong\n");
                errorCode = ERR_DHT_READ;
                break;
            }
        }
        last = gpio_get(DHT_PIN);

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (count > 16) data[j / 8] |= 1;

            if (j < 16) {
                logger_info("bit %d: count=%d val=%d\n", j, count,
                            (count > 16) ? 1 : 0);
            }
            j++;
        }
    }

    logger_info("raw data: [0]=%d [1]=%d [2]=%d [3]=%d [4]=%d\n", data[0],
                data[1], data[2], data[3], data[4]);

    uint8_t computed = data[0] + data[1] + data[2] + data[3];
    uint8_t received = data[4];
    logger_info("checksum: computed=%d received=%d\n", computed, received);

    if (computed != received) {
        logger_err("checksum failed!\n");
        return ERR_DHT_CHECKSUM;
    }

    result->humidity = (float)data[0];
    result->temperature = (float)data[2];

    return errorCode;
}
