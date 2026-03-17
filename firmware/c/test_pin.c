#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <stdio.h>

#define DHT_PIN 11

int main() {
    stdio_init_all();
    sleep_ms(2000);

    gpio_init(DHT_PIN);
    gpio_set_dir(DHT_PIN, GPIO_IN);

    printf("Testing DHT11 pin - reading pin state for 10 seconds\n");

    for (int i = 0; i < 100; i++) {
        int state = gpio_get(DHT_PIN);
        printf("Pin state: %d\n", state);
        sleep_ms(100);
    }

    return 0;
}
