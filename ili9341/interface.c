#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include "gameshared.h"
#include "pico/time.h"
#include "include/mode0/mode0.h"
#include "include/ili9341/ili9341.h"

// define pins numbers of input buttons

#define KEY_PIN_DOWN 18
#define KEY_PIN_MENU 16
#define KEY_PIN_UP 21
#define KEY_PIN_LEFT 17
#define KEY_PIN_RIGHT 20
#define KEY_PIN_CHRG 19

static uint32_t startTime = 0;
void IF_Setup()
{
	startTime = IF_GetCurrentTime();

	// Initialize gpio pins, set directions
	// and nature for all buttons
    gpio_init(KEY_PIN_DOWN);
	gpio_init(KEY_PIN_MENU);
	gpio_init(KEY_PIN_UP);
	gpio_init(KEY_PIN_LEFT);
	gpio_init(KEY_PIN_RIGHT);
	gpio_init(KEY_PIN_CHRG);
    gpio_set_dir(KEY_PIN_DOWN, GPIO_IN);
	gpio_set_dir(KEY_PIN_MENU, GPIO_IN);
	gpio_set_dir(KEY_PIN_UP, GPIO_IN);
	gpio_set_dir(KEY_PIN_LEFT, GPIO_IN);
	gpio_set_dir(KEY_PIN_RIGHT, GPIO_IN);
	gpio_set_dir(KEY_PIN_CHRG, GPIO_IN);
    gpio_pull_up(KEY_PIN_DOWN);
	gpio_pull_up(KEY_PIN_MENU);
	gpio_pull_up(KEY_PIN_UP);
	gpio_pull_up(KEY_PIN_LEFT);
	gpio_pull_up(KEY_PIN_RIGHT);
	gpio_pull_up(KEY_PIN_CHRG);
}

uint32_t IF_GetCurrentTime()
{
	return to_us_since_boot(get_absolute_time());
}


// call draw screen function from mainloop
void IF_DrawScreen(uint8_t * _framebuffer, size_t _framebufferSize)
{
    ili9341_write_data(_framebuffer, _framebufferSize);
}

static uint32_t last_seed = 0xdeadb0d1;

// to generate random no.
uint32_t IF_Random()
{
    uint32_t now = IF_GetCurrentTime();
    uint32_t result = ((uint32_t)(142*now * now + 13*now + 203) >> 4)+last_seed;
    last_seed = result;
    return result;
}

//
uint IF_Keyboard(void)
{
	// this should have been covered with interrupts
	// try searching for "Switch bouncing" to learn more
	if (gpio_get(KEY_PIN_MENU) == 1)
		Key_Down(KEY_MENU);
	else 
		Key_Up(KEY_MENU);

	if (gpio_get(KEY_PIN_LEFT) == 1)
		Key_Down(KEY_LEFT);
	else 
		Key_Up(KEY_LEFT);

	if (gpio_get(KEY_PIN_RIGHT) == 1)
		Key_Down(KEY_RIGHT);
	else 
		Key_Up(KEY_RIGHT);

	if (gpio_get(KEY_PIN_UP) == 0)
		Key_Down(KEY_UP);
	else 
		Key_Up(KEY_UP);

	if (gpio_get(KEY_PIN_DOWN) == 1)
		Key_Down(KEY_DOWN);
	else 
		Key_Up(KEY_DOWN);

	if (gpio_get(KEY_PIN_CHRG) == 1) 
		IsChrg(1);
	else
		IsChrg(0);
	return gpio_get(KEY_PIN_CHRG);

}

uint8_t IF_Touchscreen()
{
	uint8_t result = ILI9341_T_TouchGetCoordinates(&mainMemory.touch_X, &mainMemory.touch_Y);
	mainMemory.touchPressed = result;
	return result;
}
