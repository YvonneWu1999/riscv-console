#include <stdint.h>
#ifndef SYSCALL_H
#define SYSCALL_H
// graphic system calls
uint32_t enableTextMode(void);
uint32_t enableGraphicsMode(void);

uint32_t createSmallSprite(uint32_t shapeType);
void setSmallData(uint32_t x, uint32_t y);
uint32_t setSmallPalette(uint32_t index, uint32_t color);
uint32_t setSmallControl(uint32_t index, uint32_t palette, uint32_t x,
                         uint32_t y, uint32_t z);
uint32_t getSmallSpriteCount();
uint32_t createMediumSprite(uint32_t shapeType);
void setMediumData(uint32_t x, uint32_t y);
uint32_t setMediumPalette(uint32_t index, uint32_t color);
uint32_t setMediumControl(uint32_t index, uint32_t palette, uint32_t x,
                          uint32_t y, uint32_t z);
uint32_t getMediumSpriteCount();
uint32_t createLargeSprite(uint32_t shapeType);
void setLargeData(uint32_t x, uint32_t y);
uint32_t setLargePalette(uint32_t index, uint32_t color);
uint32_t setLargeControl(uint32_t index, uint32_t palette, uint32_t x,
                         uint32_t y, uint32_t z);
uint32_t getLargeSpriteCount();
uint32_t createBackground();
void setBackgroundData(uint32_t x, uint32_t y);
uint32_t setBackgroundPalette(uint32_t index, uint32_t color);
uint32_t setBackgroundControl(uint32_t index, uint32_t palette, uint32_t x,
                              uint32_t y, uint32_t z);

// Enum for shape types
enum ShapeType { SQUARE, CIRCLE, TRIANGLE, PARALLELOGRAM, NOUGHT, CROSS };
// Enum for backgroud shape types
enum BackgroundShapeType { POUND, RECTANGLE };
uint32_t setSpriteDataByShapeAndSize(enum ShapeType shapeType, uint32_t size,
                                     void (*f)(uint32_t, uint32_t));

#endif