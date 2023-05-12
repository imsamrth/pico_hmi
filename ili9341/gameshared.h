#ifndef GAMESHARED_H_
#define GAMESHARED_H_

#include "interface.h"
#include "include/mode0/mode0.h"
#include <stdint.h>

// different state of apps
typedef enum {
    GS_NOT_STARTED = 0,
    GS_RUNNING     = 1,
    GS_DONE        = 2,
    GS_ANIMATING   = 3
} GameState;

// allocating memory for different apps 
typedef struct {
    uint32_t memblocks[16*1024];
} SharedMemory;


// Keep values of touch pressed points
typedef struct {
    SharedMemory sharedMemory;
    uint8_t touchPressed;
	uint16_t touch_X;
	uint16_t touch_Y;
    GameState _gameState;
	uint32_t _lastRenderedTime;
    uint32_t _lastRenderedTick;
} MainMemory;

extern MainMemory mainMemory;

void Key_Down(KeyPressedEnum key);
void Key_Up(KeyPressedEnum key);
KeyPressedEnum Keyboard_GetPressedKeys();
KeyPressedEnum Keyboard_GetHeldKeys();
void IsChrg(uint8_t yes);

uint8_t SoC();

#endif//GAMESHARED_H_
