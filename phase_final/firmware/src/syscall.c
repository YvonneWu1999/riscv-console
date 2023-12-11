#include "syscall.h"

#include <stdint.h>

#include "chip.h"

int small_sprite_count = 0;
int medium_sprite_count = 0;
int large_sprite_count = 0;
int background_count = 0;

uint32_t enableTextMode(void) {
    MODE_CONTROL_REGISTOR = 0;
    return 1;
}

uint32_t enableGraphicsMode(void) {
    MODE_CONTROL_REGISTOR = 1;
    return 1;
}

void setSmallData(uint32_t x, uint32_t y) {
    uint32_t index = y * 16 + x + (0x100) * small_sprite_count;
    SMALL_DATA_ADDRESS[index] = 1;
}

uint32_t setSmallPalette(uint32_t index, uint32_t palette) {
    if (index < 4) {
        SMALL_PALETTE_ADDRESS[index * 0x100 + 1] = palette;
        return 1;
    } else {
        return 0; // index out of bounds
    }
}

uint32_t setSmallControl(uint32_t index, uint32_t palette, uint32_t x,
                         uint32_t y, uint32_t z) {
    SMALL_CONTROL_ADDRESS[(0x4) * index] =
        (((uint32_t)index) << 24) | (((uint32_t)z) << 21) |
        (((uint32_t)y + 16) << 12) | (((uint32_t)x + 16) << 2) |
        (palette & 0x3);
    return 1;
}

void setMediumData(uint32_t x, uint32_t y) {
    uint32_t index = y * 32 + x + (0x400) * medium_sprite_count;
    MEDIUM_DATA_ADDRESS[index] = 1;
}

uint32_t setMediumPalette(uint32_t index, uint32_t palette) {
    if (index < 4) {
        MEDIUM_PALETTE_ADDRESS[index * 0x100 + 1] = palette;
        return 1;
    } else {
        return 0; // index out of bounds
    }
}

uint32_t setMediumControl(uint32_t index, uint32_t palette, uint32_t x,
                          uint32_t y, uint32_t z) {
    MEDIUM_CONTROL_ADDRESS[(0x4) * index] =
        (((uint32_t)index) << 24) | (((uint32_t)z) << 21) |
        (((uint32_t)y + 32) << 12) | (((uint32_t)x + 32) << 2) |
        (palette & 0x3);
    return 1;
}

void setLargeData(uint32_t x, uint32_t y) {
    uint32_t index = y * 64 + x + (0x1000) * large_sprite_count;
    LARGE_DATA_ADDRESS[index] = 1;
}

uint32_t setLargePalette(uint32_t index, uint32_t palette) {
    if (index < 4) {
        LARGE_PALETTE_ADDRESS[index * 0x100 + 1] = palette;
        return 1;
    } else {
        return 0; // index out of bounds
    }
}

uint32_t setLargeControl(uint32_t index, uint32_t palette, uint32_t x,
                         uint32_t y, uint32_t z) {
    LARGE_CONTROL_ADDRESS[(0x4) * index] =
        (((uint32_t)index) << 24) | (((uint32_t)z) << 21) |
        (((uint32_t)y + 64) << 12) | (((uint32_t)x + 64) << 2) |
        (palette & 0x3);
    return 1;
}

uint32_t createSmallSprite(uint32_t shapeType) {
    void (*setSmallDataFunc)(uint32_t, uint32_t) = setSmallData;
    setSpriteDataByShapeAndSize(shapeType, 16, setSmallDataFunc);
    small_sprite_count++;
    return small_sprite_count;
}

uint32_t createMediumSprite(uint32_t shapeType) {
    void (*setMediumDataFunc)(uint32_t, uint32_t) = setMediumData;
    setSpriteDataByShapeAndSize(shapeType, 32, setMediumDataFunc);
    medium_sprite_count++;
    return medium_sprite_count;
}

uint32_t createLargeSprite(uint32_t shapeType) {
    void (*setLargeDataFunc)(uint32_t, uint32_t) = setLargeData;
    setSpriteDataByShapeAndSize(shapeType, 64, setLargeDataFunc);
    large_sprite_count++;
    return large_sprite_count;
}

void setBackgroundData(uint32_t x, uint32_t y) {
    uint32_t index = y * 512 + x + (0x24000) * background_count;
    BACKGROUND_DATA_ADDRESS[index] = 1;
}

uint32_t setBackgroundPalette(uint32_t index, uint32_t palette) {
    if (index < 4) {
        BACKGROUND_PALETTE_ADDRESS[index * 0x100 + 1] = palette;
        return 1;
    } else {
        return 0; // index out of bounds
    }
    return 1;
}

uint32_t setBackgroundControl(uint32_t index, uint32_t palette, uint32_t x,
                              uint32_t y, uint32_t z) {
    BACKGROUND_CONTROL_ADDRESS[(0x4) * index] =
        (((uint32_t)index) << 29) | (((uint32_t)z) << 22) |
        (((uint32_t)y + 288) << 12) | (((uint32_t)x + 512) << 2) |
        (palette & 0x3);
};

uint32_t createBackground(uint32_t backgroundShapeType) {
    switch (backgroundShapeType) {
        case POUND:
            for (int y = 0; y < 288; y++) {
                for (int x = 0; x < 512; x++) {
                    if ((x > 112 && x < 400) &&
                        (x == 112 + 96 || x == 112 + 96 * 2 || y == 96 ||
                         y == 192)) {
                        setBackgroundData(x, y);
                    }
                }
            }
            break;
        case RECTANGLE:
            for (int y = 0; y < 288; y++) {
                for (int x = 0; x < 512; x++) {
                    setBackgroundData(x, y);
                }
            }
        default:
            break;
    }
    background_count++;
    return background_count;
}

uint32_t getSmallSpriteCount() { return small_sprite_count; }
uint32_t getMediumSpriteCount() { return medium_sprite_count; }
uint32_t getLargeSpriteCount() { return large_sprite_count; }

uint32_t setSpriteDataByShapeAndSize(enum ShapeType shapeType, uint32_t size,
                                     void (*f)(uint32_t, uint32_t)) {
    switch (shapeType) {
        case SQUARE: // square
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    (*f)(x, y);
                }
            }
            break;
        case CIRCLE: // circle
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (((x - size / 2) * (x - size / 2) +
                         (y - size / 2) * (y - size / 2)) <=
                        ((size / 2) * (size / 2))) {
                        (*f)(x, y);
                    };
                }
            }
            break;
        case TRIANGLE: //  triangle
            for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                    if (x <= 8 + y / 2 && x >= 8 - y / 2) {
                        (*f)(x, y);
                    }
                }
            }
            break;
        case PARALLELOGRAM: //  parallelogram
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (x >= size / 2 - y / 2 && x < size - y / 2) {
                        (*f)(x, y);
                    }
                }
            }
            break;
        case NOUGHT: // nought
            int edge = size / 2 - 1;
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (((x - edge) * (x - edge) + (y - edge) * (y - edge)) <=
                            size / 2 * size / 2 &&
                        ((x - edge) * (x - edge) + (y - edge) * (y - edge)) >=
                            edge * edge) {
                        (*f)(x, y);
                    }
                }
            }
            break;
        case CROSS: // cross
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (x == y || x + y == size - 1) {
                        (*f)(x, y);
                    }
                }
            }
            break;
        default:
            break;
    }
};
