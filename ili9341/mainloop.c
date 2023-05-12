#include "mainloop.h"
#include "include/mode0/mode0.h"
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "gameshared.h"

Callbacks _callbacks = {0, 0, 0, 0};

int breakGame = 0;


// this function calls the init function of new callback 
// and deinit function of current callback

void SetupCallbacks(const Callbacks *callbacksNew)
{
    // Clear the pressed keys buffer
    Keyboard_GetPressedKeys();
    // Maybe a previous game needs to clear the heap
    if (_callbacks.deinitCallback)
        (*_callbacks.deinitCallback)();
    // Initialize/reset a new game
    if (callbacksNew->initCallback)
        (*callbacksNew->initCallback)();
    _callbacks = *callbacksNew;
}

void ResetGame()
{
    SetupCallbacks(&_callbacks);
}

static uint32_t _lastDrawTime;
static uint32_t _lastDrawMin;
static uint32_t _lastDrawHour;
static uint8_t _min = 0;
static uint8_t _sec = 0;
static uint8_t _minD = 0;
static uint8_t isChrgg = 0;


// this function is called countinously and
// updates the screen accordingly 
void DoMainLoop()
{
    KeyPressedEnum keys_held;
    _lastDrawTime = IF_GetCurrentTime();

    // intializig the clock display
    GPU_DrawFilledSquare(MODE0_GRAY, 124, 0, 100, 38);
    GPU_DrawLetter_L(MODE0_BLACK, 124, 4, '0');
    GPU_DrawLetter_L(MODE0_BLACK, 137, 4, '0');
    GPU_DrawLetter_L(MODE0_BLACK, 150, 4, ':');
    GPU_DrawLetter_L(MODE0_BLACK, 163, 4, '0');
    GPU_DrawLetter_L(MODE0_BLACK, 176, 4, '0');

    while (1)
    {
        // Register keyboard and touch events
        isChrgg = IF_Keyboard();
        // IF_Touchscreen();
        //  Reset to home page
        if (Keyboard_GetHeldKeys() & KEY_MENU)
        {
            SetupCallbacks(&HM_Callbacks);
        }

        // get State of charge and show chrg icon
        if (isChrgg == 1 )
        {
            	GPU_draw_icon_565(290,4,32,32, MODE0_BATT);
        }

        // Do main loop logic
        if (_callbacks.updateCallback)
            (*_callbacks.updateCallback)();

        // Render screen at desired FPS
        if (_callbacks.drawCallback)
        {
            //claculate time difference before hearing keys and render screen
            uint32_t new_draw_time = IF_GetCurrentTime();
            if (new_draw_time - _lastDrawTime > (ONE_SECOND / 1000))
            {
                _lastDrawTime = new_draw_time;
                (*_callbacks.drawCallback)();
            }

            // this draw top bar containing clock every one second
            if (new_draw_time - _lastDrawMin > (10 * ONE_SECOND))
            {
                GPU_DrawFilledSquare(MODE0_GRAY, 124, 4, 100, 30);
                GPU_DrawLetter_L(MODE0_BLACK, 150, 4, ':');
                GPU_DrawLetter_L(MODE0_BLACK, 176, 4, '0');
                _lastDrawMin = new_draw_time;
                _sec += 1;
                if (_sec == 6)
                    _min += 1;
                if (_min == 10)
                    _minD += 1;
                _sec %= 6;
                _min %= 10;
                _minD %= 10;

                char c = 48 + _minD;
                GPU_DrawLetter_L(MODE0_BLACK, 124, 4, c);
                c = 48 + _min;
                GPU_DrawLetter_L(MODE0_BLACK, 137, 4, c);
                c = 48 + _sec;
                GPU_DrawLetter_L(MODE0_BLACK, 163, 4, c);
            }

            // and render the display
            GPU_render();
        };
        if (breakGame)
            break;
    }
}