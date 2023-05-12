#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "../mainloop.h"
#include "../include/mode0/mode0.h"
#include "../interface.h"
#include "../gameshared.h"
// #include "../include/icon/icon.h"
#include <stdint.h>
#include <string.h>

typedef struct
{
    uint8_t _choiceX;
    uint8_t _choiceY;
    uint8_t _choiceEnd;
    uint8_t screenLines;
} BTScreenMemory;

typedef struct
{
    const char name[8];
    const mode0_color_t color;
} TextItem;

#define NO_OF_TEXT 4
static const TextItem listOfGames[NO_OF_TEXT] = {
    {"START \n", MODE0_GREEN},
    {"RESUME\n", MODE0_YELLOW},
    {"PAUSE \n", MODE0_RED},
    {"REPEAT\n", MODE0_BLUE},
};

static BTScreenMemory *const smemory = (BTScreenMemory *)(&mainMemory.sharedMemory.memblocks[1024 * 2]);

#define NUM_TICKS 1

static inline uint32_t CalculateTick(long long timeNow)
{
    return timeNow / (ONE_SECOND / NUM_TICKS);
}

static const FontDescription *f_big = &fontDescription[2];
static const FontDescription *f_huge = &fontDescription[3];
static const char screen_title[] = "BLUETOOTH";
static const char sent[] = "Command Sent";
static const char screen_off[] = "Screen Saver";

static void G_Init(void)
{
    mainMemory._gameState = GS_RUNNING;
    mainMemory._gameState = GS_NOT_STARTED;
    mainMemory._lastRenderedTime = 0;
    mainMemory.touch_X = 0;
    mainMemory.touch_Y = 0;
    smemory->_choiceX = 0;
    smemory->_choiceY = 0;
    smemory->screenLines = 0;
    smemory->_choiceEnd = 0;

    // Texture_CopyFrom(texture_rpi);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 200);

    uart_init(uart0, 9600);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    GPU_draw_icon_565(10, 40, 32, 32, MODE0_BACK);
    GPU_draw_icon_565(60, 40, 32, 32, MODE0_BT);
    FGPU_DrawText(MODE0_BLACK, f_huge, 100, 43, screen_title, 9);

    // char time[] = "04:00";
    // char *timeadd = &time;
    // GPU_DrawText_L(MODE0_BLACK, 120, 4, timeadd, 5);
    // GPU_draw_icon_565(8, 4, 32, 32, MODE0_BT);
    // GPU_draw_icon_565(250, 4, 32, 32, MODE0_NOTI);
    // GPU_draw_icon_565(290, 4, 32, 32, MODE0_BATT);
    GPU_render();
}

#define START_X GPU_BORDER_X
#define END_X (GPU_X / 2 - GPU_BORDER_X)
#define START_Y GPU_BORDER_Y
#define END_Y (GPU_Y - GPU_BORDER_Y)
#define BOX_DIMENSION ((END_X - START_X) / 3)


#define RESET_BUTTON_W (5 * 16)
#define RESET_BUTTON_H (30)
#define RESET_BUTTON_X (S2_CENTER_X - RESET_BUTTON_W / 2)
#define RESET_BUTTON_Y (S2_CENTER_Y + 30)

static void G_Update(void)
{
    KeyPressedEnum keyboard_held = Keyboard_GetHeldKeys();
    KeyPressedEnum keyboard = Keyboard_GetPressedKeys();
    if (keyboard == KEY_NONE)
        return;
    if (keyboard & KEY_UP)
    {

        // 		smemory->_choice_current = 1 ;
        // GPU_DrawEmptySquare(MODE0_BLACK, 75,85, 170,30 );
        // GPU_DrawEmptySquare(MODE0_WHITE, 75,155, 170,30 );
    }
    if (keyboard & KEY_DOWN)
    {
        smemory->_choiceX = (smemory->_choiceX + 1) % 4;
        // smemory->_choice_current= 0;
        // 	GPU_DrawEmptySquare(MODE0_WHITE, 75,85, 170,30 );
        // 	GPU_DrawEmptySquare(MODE0_BLACK, 75,155, 170,30 );
    }
    if (keyboard & KEY_RIGHT)
    {
        uint8_t i = smemory->_choiceX;
        // const TextItem *selectedText = &listOfGames[i];
        // // char* command_NEW[6] ;
        // // command_NEW =
        // //const char comm[6] = selectedText->name;
        // char names[6] = "      ";
        // names = &selectedText->name;
        // uart_puts(uart0, &listOfGames[i].name);

        char*  command_NEW  = listOfGames[i].name;
        uart_puts(uart0, command_NEW );

        GPU_DrawFilledSquare(MODE0_GRAY, 70, 200, 210,30);
        FGPU_DrawText(MODE0_BLACK, f_big, 80, 205, sent, 13);
        GPU_render();
        sleep_ms(1500);

        GPU_DrawFilledSquare(MODE0_WHITE, 0, 200, 320, 40);
        GPU_render();
    }
    if (keyboard & KEY_LEFT)
    {
        SetupCallbacks(&MM_Callbacks);
    }
}

#define C_BORDERS MODE0_BLACK
#define C_X MODE0_GREEN
#define C_O MODE0_RED
#define C_TEXT MODE0_WHITE
#define C_VLINE MODE0_YELLOW
#define C_VICTORY MODE0_BLUE

static void G_Draw(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        int j = smemory->_choiceX;
        int x = i % 2;
        int y = i / 2;
        const TextItem *selectedText = &listOfGames[i];
        if (j == i)
        {

            GPU_DrawFilledSquare(selectedText->color, 40 + x * 130, 90 + y * 60, 110, 40);
            GPU_DrawText_L(MODE0_BLACK, 50 + x * 130, 94 + y * 60, selectedText->name, 6);
            GPU_DrawEmptySquare(MODE0_BLACK, 38 + x * 130, 88 + y * 60, 114, 44);
        }
        else
        {
            GPU_DrawFilledSquare(selectedText->color, 40 + x * 130, 90 + y * 60, 110, 40);
            GPU_DrawText_L(MODE0_BLACK, 50 + x * 130, 94 + y * 60, listOfGames[i].name, 6);
            GPU_DrawEmptySquare(MODE0_WHITE, 38 + x * 130, 88 + y * 60, 114, 44);
        }
    }
    GPU_render();
}

static void G_Deinit(void)
{
}

const Callbacks BT_Callbacks = {&G_Init, &G_Update, &G_Draw, &G_Deinit};
