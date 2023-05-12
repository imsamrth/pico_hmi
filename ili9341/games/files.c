#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "../mainloop.h"
#include "../include/mode0/mode0.h"
#include "../interface.h"
#include "../gameshared.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../include/FatFs_SPI/sd_driver/hw_config.h"
#include "../include/FatFs_SPI/ff15/source/diskio.h"
#include "../include/FatFs_SPI/ff15/source/ff.h"
#include <stdint.h>
#include <string.h>
#include <math.h>


// FILES APP
typedef struct
{
    const char* name;
    const Callbacks *callbacks;
    const uint  icon;
} FilesItem;

typedef struct
{
    uint8_t _choiceStart;
    uint8_t _choiceCurrent;
    uint8_t _choiceEnd;
    uint8_t screenLines;
} FilesScreenMemory;

static FilesScreenMemory *const memory = (FilesScreenMemory *)(&mainMemory.sharedMemory.memblocks[0]);

#define NUM_TICKS 10

static const FontDescription *f_big = &fontDescription[2];
static const FontDescription *f_huge = &fontDescription[3];

static uint32_t CalculateTick()
{
    return IF_GetCurrentTime() / (ONE_SECOND / NUM_TICKS);
}

spi_t *spi;
sd_card_t *sd_card;

#define NO_OF_GAMES 4
FilesItem listOfGames[NO_OF_GAMES] = {};
// const FilesItem listOfGames[NO_OF_GAMES] = {
//     {"            \0", &BT_Callbacks, MODE0_BT},
//     {"            \0", &HM_Callbacks, MODE0_NOTI},
//     {"            \0", &ST_Callbacks, MODE0_FILES_NEW},
//     {"            \0", &ST_Callbacks, MODE0_WHEEL},
//     {"            \0", &HM_Callbacks, MODE0_WALL},
//     {"            \0", &HM_Callbacks, MODE0_INFO},
//     {"            \0", &HM_Callbacks, MODE0_MENU},
// };

void list_files(const char *path, FIL *output_file)
{
    FRESULT fr;
    DIR dir;
    FILINFO fno;

    fr = f_opendir(&dir, path);
    if (fr != FR_OK)
    {
        f_printf(output_file, "Failed to open directory: %s\n", path);
        return;
    }

    f_printf(output_file, "Listing files and directories in %s:\n", path);
    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
        {
            break;
        }

        if (fno.fattrib & AM_DIR)
        {
            f_printf(output_file, "  [DIR] %s\n", fno.fname);
        }
        else
        {
            f_printf(output_file, "  [FILE] %s (size: %lu)\n", fno.fname, fno.fsize);
        }
    }

    f_closedir(&dir);
}

void list_filesPage(const char *path, FIL *output_file, uint index)
{
    FRESULT fr;
    DIR dir;
    FILINFO fno;

    fr = f_opendir(&dir, path);
    if (fr != FR_OK)
    {
        f_printf(output_file, "Failed to open directory: %s\n", path);
        return;
    }

    f_printf(output_file, "Listing files and directories in %s:\n", path);
    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
        {
            break;
        }

        if (fno.fattrib & AM_DIR)
        {
            FilesItem* curennt = &listOfGames[index];
            f_printf(output_file, "  [DIR] %s\n", fno.fname);
            curennt->name = &fno.fname;
            curennt->callbacks = &HM_Callbacks;
            // curennt->icon = 0;
            index+=1; 
        }
        else
        {
            FilesItem* curennt = &listOfGames[index];
            f_printf(output_file, "  [FILE] %s (size: %lu)\n", fno.fname, fno.fsize);
            curennt->name = &fno.fname;
            curennt->callbacks = &HM_Callbacks;
            // curennt->icon = 1
            index+=1; 
        }
    }

    f_closedir(&dir);
}

void my_files()
{
    stdio_init_all();

    // Initialize the pointers
    spi = spi_get_by_num(0);
    sd_card = sd_get_by_num(0);

    FRESULT fr;
    FATFS fs;

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, sd_card->pcName, 1);
    if (fr != FR_OK)
    {
        printf("Failed to mount SD card: %d\n", fr);
        return 1;
    }
    printf("SD card mounted\n");

    FIL output_file;
    const char *output_file_path = "file_list.txt";
    if (f_open(&output_file, output_file_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        printf("Failed to open output file\n");
        return 1;
    }

    list_files("/", &output_file); // List files and directories at the root of the SD card

    f_close(&output_file);
    printf("File list written to %s\n", output_file_path);

    printf("Unmounting SD card...\n");
    f_mount(NULL, sd_card->pcName, 0);
    printf("SD card unmounted\n");
}

static const char screen_title[] = "FILES";

static void MM_Init(void)
{
    mainMemory._gameState = GS_NOT_STARTED;
    mainMemory._lastRenderedTime = 0;
    mainMemory.touch_X = 0;
    mainMemory.touch_Y = 0;
    memory->_choiceCurrent = 0;
    memory->_choiceStart = 0;
    memory->screenLines = 4; // GPU_Y/f_big->height;
    memory->_choiceEnd = (memory->screenLines - 1) < (NO_OF_GAMES - 1) ? (memory->screenLines - 1) : (NO_OF_GAMES - 1);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 240);
    // GPU_draw_icon_565(10, 40, 32, 32, MODE0_BACK);
    GPU_draw_icon_565(60, 40, 32, 32, MODE0_FILE);
    FGPU_DrawText(MODE0_BLACK, f_huge, 100, 43, screen_title, 5);
    // GPU_draw_icon_565(8,4,32,32, MODE0_BT);
    // GPU_draw_icon_565(250,4,32,32, MODE0_NOTI);
    // GPU_draw_icon_565(290,4,32,32, MODE0_BATT);
    stdio_init_all();

    // Initialize the pointers
    spi = spi_get_by_num(0);
    sd_card = sd_get_by_num(0);

    FRESULT fr;
    FATFS fs;

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, sd_card->pcName, 1);
    if (fr != FR_OK)
    {
        printf("Failed to mount SD card: %d\n", fr);
        return 1;
    }
    printf("SD card mounted\n");

    FIL output_file;
    const char *output_file_path = "file_list.txt";
    if (f_open(&output_file, output_file_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        printf("Failed to open output file\n");
        return 1;
    }

    //list_files("/", &output_file); // List files and directories at the root of the SD card
    list_filesPage("/", &output_file,0);
    f_close(&output_file);
    printf("File list written to %s\n", output_file_path);

    printf("Unmounting SD card...\n");
    f_mount(NULL, sd_card->pcName, 0);
    printf("SD card unmounted\n");

    GPU_render();
}

static void MM_Update(void)
{
    KeyPressedEnum keyboard_held = Keyboard_GetHeldKeys();
    KeyPressedEnum keyboard = Keyboard_GetPressedKeys();
    if (keyboard == KEY_NONE)
        return;
    if (keyboard & KEY_UP)
    {
        memory->_choiceCurrent = memory->_choiceCurrent - 1;
        if (memory->_choiceCurrent >= NO_OF_GAMES)
        {
            memory->_choiceCurrent = NO_OF_GAMES - 1;
            memory->_choiceEnd = NO_OF_GAMES - 1;
            memory->_choiceStart = NO_OF_GAMES - memory->screenLines;
            if (memory->_choiceStart > memory->_choiceEnd)
                memory->_choiceStart = 0;
        }
        else
        {
            if (memory->_choiceCurrent < memory->_choiceStart)
            {
                memory->_choiceStart--;
                memory->_choiceEnd--;
            }
        }
    }

    if (keyboard & KEY_DOWN)
    {
        memory->_choiceCurrent = memory->_choiceCurrent + 1;
        if (memory->_choiceCurrent >= NO_OF_GAMES)
        {
            memory->_choiceCurrent = 0;
            memory->_choiceStart = 0;
            memory->_choiceEnd = (memory->screenLines - 1) < (NO_OF_GAMES - 1) ? (memory->screenLines - 1) : (NO_OF_GAMES - 1);
        }
        else
        {
            if (memory->_choiceCurrent > memory->_choiceEnd)
            {
                memory->_choiceStart++;
                memory->_choiceEnd++;
            }
        }
    }
    if (keyboard & KEY_RIGHT)
    {
        // SetupCallbacks(&G1_Callbacks);
        const FilesItem *selectedGame = &listOfGames[memory->_choiceCurrent];
        if (selectedGame->callbacks)
            SetupCallbacks(selectedGame->callbacks);
    }
    if (keyboard & KEY_LEFT)
    {
        SetupCallbacks(&HM_Callbacks);
    }
}

static void MM_Draw(void)
{

    for (uint8_t i = memory->_choiceStart; i <= memory->_choiceEnd; i++)
    {
        int j = i - memory->_choiceStart;
        if (memory->_choiceCurrent == i)
        {
            GPU_DrawFilledSquare(MODE0_GRAY, 8, 84 + j * 40, 272, 32);
            GPU_DrawText_L(MODE0_BLACK, 48, 80 + j * 40 + 7, listOfGames[i].name, 5);
            GPU_DrawEmptySquare(MODE0_BLACK, 6, 82 + j * 40, 274, 36);
            GPU_draw_icon_565(8, 84 + j * 40, 32, 32, MODE0_FILES_NEW);
        }
        else
        {
            GPU_DrawFilledSquare(MODE0_WHITE, 8, 84 + j * 40, 272, 32);
            GPU_DrawText_L(MODE0_BLACK, 48, 80 + j * 40 + 7, listOfGames[i].name, 5);
            GPU_DrawEmptySquare(MODE0_BLACK, 6, 82 + j * 40, 274, 36);
            GPU_draw_icon_565(8, 84 + j * 40, 32, 32, MODE0_FILE);
        }
    }
}

static void MM_Deinit(void)
{
}

const Callbacks FS_Callbacks = {&MM_Init, &MM_Update, &MM_Draw, &MM_Deinit};
