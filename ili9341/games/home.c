// HOME PAGE

#include "../mainloop.h"
#include "../include/mode0/mode0.h"
#include "../interface.h"
#include "../gameshared.h"
#include <stdint.h>
#include <string.h>

// Strings for Date and time

char date[] = "12 Apr";
char *dateadd = &date;

// TO calculate time after screen on 

#define NUM_TICKS 1

static inline uint32_t CalculateTick(long long timeNow)
{
    return timeNow / (ONE_SECOND / NUM_TICKS);
}

// describing font option and intializing
static const FontDescription *f_big = &fontDescription[2];
static const FontDescription *f_huge = &fontDescription[3];

// Designing Display on Init(first time when this page is called)

static void G_Init(void)
{
    // state global update
    mainMemory._gameState = GS_RUNNING;

    // background color blocks
    GPU_DrawFilledSquare(MODE0_GRAY, 0, 0, 124, 40);
    GPU_DrawFilledSquare(MODE0_GRAY, 224, 0, 96, 40);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 200);
    GPU_DrawFilledSquare(MODE0_BLACK, 0, 38, 320, 2);

    // adding icons with location 
    GPU_draw_icon_565(8, 4, 32, 32, MODE0_BT);
    GPU_draw_icon_565(250, 4, 32, 32, MODE0_NOTI);
    GPU_draw_icon_565(0, 40, 320, 176, MODE0_WEL);

    // bottom bar with menu icon 
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 200, 320, 40);
    GPU_draw_icon_565(144, 200, 32, 36, MODE0_MENU);
    FGPU_DrawText(MODE0_RED, f_big, 128, 40, dateadd, 6);

    // build screen (render)
    GPU_render();
}

// define material design constant 
#define GPU_REST (GPU_X / 2 - 2 * GPU_BORDER_X)
#define START_X GPU_BORDER_X
#define END_X (GPU_X / 2 - GPU_BORDER_X)
#define START_Y GPU_BORDER_Y
#define END_Y (GPU_Y - GPU_BORDER_Y)
#define BOX_DIMENSION ((END_X - START_X) / 3)
#define S1_CENTER_X (GPU_X / 4)
#define S1_CENTER_Y (GPU_Y / 2)
#define S2_CENTER_X (3 * GPU_X / 4)
#define S2_CENTER_Y (GPU_Y / 2)
#define RESET_BUTTON_W (5 * 16)
#define RESET_BUTTON_H (30)
#define RESET_BUTTON_X (S2_CENTER_X - RESET_BUTTON_W / 2)
#define RESET_BUTTON_Y (S2_CENTER_Y + 30)


// function to specify update when keypress is hear
static void G_Update(void)
{
    // hearing key pressed
    KeyPressedEnum keyboard_held = Keyboard_GetHeldKeys();
    KeyPressedEnum keyboard = Keyboard_GetPressedKeys();
    if (keyboard == KEY_NONE)
        return;

    // WHEN right is key pressed, call Menu page
    if (keyboard & KEY_RIGHT)
    {
        SetupCallbacks(&MM_Callbacks);
    }
}


// define colors
#define C_BORDERS MODE0_BLACK
#define C_X MODE0_GREEN
#define C_O MODE0_RED
#define C_TEXT MODE0_WHITE
#define C_VLINE MODE0_YELLOW
#define C_VICTORY MODE0_BLUE

// define draw function according to state of variables
static void G_Draw(void)
{
}

static void G_Deinit(void)
{
}


// initializing Callback object
const Callbacks HM_Callbacks = {&G_Init, &G_Update, &G_Draw, &G_Deinit};
