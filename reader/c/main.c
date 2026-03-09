#include <stdio.h>
#include "logger.h"

typedef struct {
    float temperature;
    float humidity;
} dht11_reading;

int* read(dht11_reading* val, int fd) {
    // read from input
    return 0;
}

int main() {
    logger_info("hey buddy %s, this is a test", "michael");
}
