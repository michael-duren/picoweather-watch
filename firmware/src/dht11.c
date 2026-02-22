#include "dht11.h"

#include <stdio.h>

#include "pico/stdlib.h"

/**
 * @brief Reads temp/humidity from dht11
 *
 * @param result Pointer to result struct
 */
int read_from_dht(dht11_reading* result) {
    printf("simulate read from dht11\n");

    result->humidity = 35.0;
    result->temperature = 77;

    return 0;
}
