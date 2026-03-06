#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

typedef struct {
    float temperature;
    float humidity;
} dht11_reading;

int read_from_dht11(dht11_reading* result);

// PICO W GPIO11 -> 15 on board
#define DHT_PIN 11
#define MAX_TIMINGS 82

#endif  // !DHT11_H
