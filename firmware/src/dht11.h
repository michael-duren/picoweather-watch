#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

typedef struct {
    float temperature;
    float humidity;
} dht11_reading;

int read_from_dht(dht11_reading* result);

#endif  // !DHT11_H
