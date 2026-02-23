#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

typedef struct {
    float temperature;
    float humidity;
} dht11_reading;

int read_from_dht11(dht11_reading* result);

#define DHT_PIN 15
#define MAX_TIMINGS 85

#endif  // !DHT11_H
