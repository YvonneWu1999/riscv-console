#include <stdbool.h>
#include <stdint.h>

#include "api.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile int count = 0;
volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
// coloring
volatile uint32_t blue = 0xFF0000FF;
volatile uint32_t red = 0xFFFF0000;
volatile uint32_t green = 0xFF00FF00;
volatile uint32_t pink = 0xFFFF00FF;

int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    // EnableTextMode();
    enableGraphicsMode();
    // create background
    createBackground(RECTANGLE);
    setBackgroundPalette(0, pink);
    setBackgroundControl(0, 0, 0, 0, 0);
    // create some sprites
    createLargeSprite(CIRCLE);
    setLargePalette(0, green);
    setLargeControl(0, 0, 0, 0, 0);
    createLargeSprite(SQUARE);
    setLargePalette(1, red);
    setLargeControl(1, 1, 448, 0, 0);

    while (1) {
        int c = a + b + global;
        if (a == 11) {
            b++;
        }
        global = getTicks();
        if (global != last_global) {
            controller_status = getController();
        }
        last_global = global;
    }
}
