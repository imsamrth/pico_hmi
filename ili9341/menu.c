#include "mainloop.h"
#include "interface.h"
#include "gameshared.h"
#include "include/mode0/mode0.h"
// #include "include/icon/icon.h"

#include <string.h>
#include <math.h>

// appitems obeject
typedef struct {
    const char name[13];
    const Callbacks * callbacks;
    const mode0_icon_t icon ;
} GameItem;

typedef struct {
    uint8_t _choiceStart;
    uint8_t _choiceCurrent;
    uint8_t _choiceEnd;
    uint8_t screenLines;
} MainScreenMemory;

//to store variable in global memory for particular app
static MainScreenMemory * const memory = (MainScreenMemory*)(&mainMemory.sharedMemory.memblocks[0]);

#define NUM_TICKS 10

static const FontDescription * f_big = &fontDescription[2];
static const FontDescription * f_huge = &fontDescription[3];

static uint32_t CalculateTick()
{
    return IF_GetCurrentTime()/(ONE_SECOND/NUM_TICKS);
}

// screen title constant
static const char screen_title[] = "APPS" ;

#define NO_OF_GAMES 7

// load variable eith app name and callbacks
static const GameItem listOfGames[NO_OF_GAMES] = {
    {"Bluetooth   \0", &BT_Callbacks, MODE0_BT},
    {"Notification\0", &HM_Callbacks,MODE0_NOTI },
    {"Files       \0", &FS_Callbacks,MODE0_FILES_NEW},
    {"Settings    \0", &ST_Callbacks,MODE0_WHEEL},
    {"Wallpapeer  \0", &HM_Callbacks,MODE0_WALL},
    {"Info        \0", &HM_Callbacks,MODE0_INFO},
    {"Home        \0", &HM_Callbacks,MODE0_MENU},
};

// function to initialize when menu is called
static void MM_Init(void)
{
    mainMemory._gameState = GS_NOT_STARTED;
	mainMemory._lastRenderedTime = 0;
	mainMemory.touch_X = 0;
	mainMemory.touch_Y = 0;
    memory->_choiceCurrent = 0;
    memory->_choiceStart = 0;
    memory->screenLines = 4;//GPU_Y/f_big->height;
    memory->_choiceEnd = (memory->screenLines - 1) < (NO_OF_GAMES-1) ? (memory->screenLines - 1) : (NO_OF_GAMES -1);
    
    // Background for menu screen
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 240);
    GPU_draw_icon_565(10, 40, 32, 32, MODE0_BACK);
    GPU_draw_icon_565(60, 40, 32, 32, MODE0_MENU);
    FGPU_DrawText(MODE0_BLACK, f_huge, 100, 43, screen_title, 4);
    GPU_draw_icon_565(8,4,32,32, MODE0_BT);
	GPU_draw_icon_565(250,4,32,32, MODE0_NOTI);
	GPU_draw_icon_565(290,4,32,32, MODE0_BATT);
    GPU_render();
}

// update on selected apps and hearing keys
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
            memory->_choiceEnd = NO_OF_GAMES -1;
            memory->_choiceStart = NO_OF_GAMES - memory->screenLines;
            if ( memory->_choiceStart > memory->_choiceEnd)
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

    if (keyboard & KEY_DOWN){
        memory->_choiceCurrent = memory->_choiceCurrent + 1;
        if (memory->_choiceCurrent >= NO_OF_GAMES)
        {
            memory->_choiceCurrent = 0;
            memory->_choiceStart = 0;
            memory->_choiceEnd = (memory->screenLines - 1) < (NO_OF_GAMES-1) ? (memory->screenLines - 1) : (NO_OF_GAMES -1);
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

   // when right key is heared, selected app is called
    if (keyboard & KEY_RIGHT)
    {
        //SetupCallbacks(&G1_Callbacks);
        const GameItem * selectedGame = &listOfGames[memory->_choiceCurrent];
        if (selectedGame->callbacks)
            SetupCallbacks(selectedGame->callbacks);
    }
    if (keyboard & KEY_LEFT)
    {
        SetupCallbacks(&HM_Callbacks);
    }

}

// this function update the screen, change 
// background of apps for selected and non-selected apps
static void MM_Draw(void)
{
   
    for ( uint8_t i=memory->_choiceStart; i<=memory->_choiceEnd; i++ )
    {
        int j = i - memory->_choiceStart;
        if ( memory->_choiceCurrent == i )
        {
            GPU_DrawFilledSquare(MODE0_GRAY, 8,84 + j*40, 272,32);
            GPU_DrawText_L(MODE0_BLACK,  48, 80+ j*40+7, listOfGames[i].name, 12);
            GPU_DrawEmptySquare(MODE0_BLACK,6,82 + j*40, 274,36);
            GPU_draw_icon_565(8,84 + j*40,32,32,listOfGames[i].icon);
               
           }
        else
        {
            GPU_DrawFilledSquare(MODE0_WHITE, 8,84 + j*40, 272,32);
            GPU_DrawText_L(MODE0_BLACK,  48, 80+ j*40+7, listOfGames[i].name, 12                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 );
            GPU_DrawEmptySquare(MODE0_BLACK,6,82 + j*40, 274,36);
            GPU_draw_icon_565(8,84 + j*40,32,32,listOfGames[i].icon);
        }

     }
    GPU_render();

}

static void MM_Deinit(void)
{

}

const Callbacks MM_Callbacks = { &MM_Init, &MM_Update, &MM_Draw, &MM_Deinit };

