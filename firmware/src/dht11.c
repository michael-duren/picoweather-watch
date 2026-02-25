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
void send_start_signal() {
    // set high to low
    set_high();
    set_low();
    // wait at least 18ms (per doc)
    sleep_ms(30);

    // pull voltage
    set_input();
    // waiting 20-40us for dht res
    sleep_us(40);
}

/**
 * @brief Reads temp/humidity from dht11
 *
 * @param result Pointer to result struct
 */
int read_from_dht(dht11_reading* result) {
    int data[5] = {0, 0, 0, 0, 0};
    uint last = 1;
    uint j = 0;

    send_start_signal();

    // read dht11 response (5.3) + 40 bits
    for (uint i = 0; i < MAX_TIMINGS; i++) {
        uint count = 0;
        while (gpio_get(DHT_PIN) == last) {
            count++;
            sleep_us(1);
            // if nothing changed something went wrong
            if (count == 255) {
                printf(
                    "reached 255 iterations while reading from dht, something "
                    "is wrong\n");
                break;
            }
        }
        last = gpio_get(DHT_PIN);

        if (count == 255) {
            printf(
                "reached 255 iterations while reading from dht, something "
                "is wrong\n");
            break;
        }

        if ((i >= 4) && (i % 2 == 0)) {
            // which of the 5 bytes we're filling
            data[j / 8] <<= 1;
            if (count > 16) data[j / 8] |= 1;
            j++;
        }
    }

    // checksum
    if (data[0] + data[1] + data[2] + data[3] != data[4]) {
        return -1;
    }

    result->humidity = (float)data[0];
    result->temperature = (float)data[2];
    return 0;
}
