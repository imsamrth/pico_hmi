// TIC TAC TOE 1P

#include "../mainloop.h"
#include "../include/mode0/mode0.h"
#include "../interface.h"
#include "../gameshared.h"
// #include "../include/icon/icon.h"
#include <stdint.h>
#include <string.h>

typedef enum {
    TTT_EMPTY     = 0,
    TTT_PLAYER_1  = 1,
    TTT_PLAYER_2  = 2
} TicTacToeBoardState;

typedef enum {
    TTTP_PLAYER_1 = 1,
    TTTP_PLAYER_2 = 2
} TicTacToePlayer;

typedef enum {
    TTTGS_PLAYING     = 0,
    TTTGS_PLAYER1_WIN = 1,
    TTTGS_PLAYER2_WIN = 2,
    TTTGS_DRAW        = 3
} TicTacToeGameState;

typedef struct {
    TicTacToeBoardState _boardState[3][3];
    uint8_t _numOfFinishedMoves;
    TicTacToePlayer _nextPlayer;
    TicTacToePlayer _aiPlayer;
    uint8_t _victoryLine;
    TicTacToeGameState _finalState;
} Game1Memory;
typedef struct {
    uint8_t _choicebright;
    uint8_t _choicescreen;
	uint8_t _choice_current;
} SettingsMemory;

static SettingsMemory * const smemory = (SettingsMemory*)(&mainMemory.sharedMemory.memblocks[1024]);


typedef struct Position {
	uint8_t x;
	uint8_t y;
	TicTacToeGameState state_result;
} Position;

static int brightness = 50;
static int screen_time = 5;
static int  onBrightness = 1 ;


#define NUM_TICKS 1

static Game1Memory * const memory = (Game1Memory*)(&mainMemory.sharedMemory.memblocks[0]);

static inline uint32_t CalculateTick(long long timeNow)
{
	return timeNow/(ONE_SECOND/NUM_TICKS);
}

static TicTacToeGameState CheckIsThereAWinner()
{
	if (memory->_numOfFinishedMoves < 4)
		return TTTGS_PLAYING;
	for (uint8_t i=0; i<3; i++)
	{
		if (memory->_boardState[i][0] != TTT_EMPTY && memory->_boardState[i][0] == memory->_boardState[i][1] && memory->_boardState[i][0] == memory->_boardState[i][2])
		{
			if (memory->_boardState[i][0] == TTT_PLAYER_1 )
				return TTTGS_PLAYER1_WIN;
			return TTTGS_PLAYER2_WIN;
		}
	}
	for (uint8_t i=0; i<3; i++)
	{
		if (memory->_boardState[0][i] != TTT_EMPTY && memory->_boardState[0][i] == memory->_boardState[1][i] && memory->_boardState[0][i] == memory->_boardState[2][i])
		{
			if (memory->_boardState[0][i] == TTT_PLAYER_1 )
				return TTTGS_PLAYER1_WIN;
			return TTTGS_PLAYER2_WIN;
		}
	}
	if (memory->_boardState[0][0] != TTT_EMPTY && memory->_boardState[0][0] == memory->_boardState[1][1] && memory->_boardState[0][0] == memory->_boardState[2][2])
	{
		if (memory->_boardState[0][0] == TTT_PLAYER_1 )
			return TTTGS_PLAYER1_WIN;
		return TTTGS_PLAYER2_WIN;
	}
	if (memory->_boardState[2][0] != TTT_EMPTY && memory->_boardState[2][0] == memory->_boardState[1][1] && memory->_boardState[2][0] == memory->_boardState[0][2])
	{
		if (memory->_boardState[2][0] == TTT_PLAYER_1 )
			return TTTGS_PLAYER1_WIN;
		return TTTGS_PLAYER2_WIN;
	}
	if (memory->_numOfFinishedMoves == 9)
		return TTTGS_DRAW;
	return TTTGS_PLAYING;
}

static void CheckVictoryLine()
{
	
	for (uint8_t i=0; i<3; i++)
	{
		if (memory->_boardState[i][0] != TTT_EMPTY && memory->_boardState[i][0] == memory->_boardState[i][1] && memory->_boardState[i][0] == memory->_boardState[i][2])
		{
			memory->_victoryLine |= 1<<i;
		}
	}
	for (uint8_t i=0; i<3; i++)
	{
		if (memory->_boardState[0][i] != TTT_EMPTY && memory->_boardState[0][i] == memory->_boardState[1][i] && memory->_boardState[0][i] == memory->_boardState[2][i])
		{
			memory->_victoryLine |= 1<<(i+3);
		}
	}
	if (memory->_boardState[0][0] != TTT_EMPTY && memory->_boardState[0][0] == memory->_boardState[1][1] && memory->_boardState[0][0] == memory->_boardState[2][2])
	{
		memory->_victoryLine |= 1<<6;
	}
	if (memory->_boardState[2][0] != TTT_EMPTY && memory->_boardState[2][0] == memory->_boardState[1][1] && memory->_boardState[2][0] == memory->_boardState[0][2])
	{
		memory->_victoryLine |= 1<<7;
	}
}


static Position ReturnBestPositionForNextMove(TicTacToePlayer nextPlayer)
{
	Position result = {0, 0, TTTGS_PLAYING};
    for (uint8_t i = 0; i < 3; i++)
    {
    	for (uint8_t j = 0; j < 3; j++)
    	{
    		if ( memory->_boardState[i][j] != TTT_EMPTY)
    		{
    			continue;
    		}
    		memory->_boardState[i][j] = nextPlayer;
    		memory->_numOfFinishedMoves++;
    		result.x = i;
    		result.y = j;
    		result.state_result = CheckIsThereAWinner();
    		memory->_boardState[i][j] = TTT_EMPTY;
    		memory->_numOfFinishedMoves--;
			if ( result.state_result == TTTGS_PLAYER1_WIN && nextPlayer == TTTP_PLAYER_1)
				break;
			if ( result.state_result == TTTGS_PLAYER2_WIN && nextPlayer == TTTP_PLAYER_2)
				break;
    	}
    	if ( result.state_result == TTTGS_PLAYER1_WIN && nextPlayer == TTTP_PLAYER_1)
			break;
		if ( result.state_result == TTTGS_PLAYER2_WIN && nextPlayer == TTTP_PLAYER_2)
			break;
    }
    return result;
}

static void PlayerPlaysPosition(uint8_t x, uint8_t y, TicTacToePlayer player)
{
	if (memory->_boardState[x][y] != TTT_EMPTY)
	{
		return;
	}
    memory->_boardState[x][y] = (player == TTTP_PLAYER_1) ? TTT_PLAYER_1 : TTT_PLAYER_2;
	memory->_numOfFinishedMoves++;
	memory->_nextPlayer = (memory->_nextPlayer == TTTP_PLAYER_1) ? TTTP_PLAYER_2 : TTTP_PLAYER_1;
}

static void AIChooseBestMove()
{
	TicTacToePlayer ai = memory->_aiPlayer;
	TicTacToePlayer p1 = memory->_aiPlayer == TTTP_PLAYER_1 ? TTTP_PLAYER_2 : TTTP_PLAYER_1;
	TicTacToeGameState desiredState = (ai == TTTP_PLAYER_1) ? TTTGS_PLAYER1_WIN : TTTGS_PLAYER2_WIN;
	TicTacToeGameState losingState = (ai == TTTP_PLAYER_1) ? TTTGS_PLAYER2_WIN : TTTGS_PLAYER1_WIN;
	Position res = ReturnBestPositionForNextMove(ai);
	if (res.state_result == desiredState)
	{
		PlayerPlaysPosition(res.x, res.y, ai);
		return;
	}
	res = ReturnBestPositionForNextMove(p1);
	if (res.state_result == losingState)
	{
		PlayerPlaysPosition(res.x, res.y, ai);
		return;
	}
	// choosing a random leftover state
	uint32_t rand = IF_Random();
	uint8_t num_of_free_spaces = 0;
	for (uint8_t i=0; i<3; i++)
		for (uint8_t j=0; j<3; j++)
		{
			if (memory->_boardState[i][j] == TTT_EMPTY)
				num_of_free_spaces++;
		}
	uint8_t choose_rand = rand % num_of_free_spaces;
	num_of_free_spaces = 0;
	for (uint8_t i=0; i<3; i++)
		for (uint8_t j=0; j<3; j++)
		{
			if (memory->_boardState[i][j] == TTT_EMPTY)
			{
				if (num_of_free_spaces == choose_rand)
				{
					PlayerPlaysPosition(i, j, ai);
				}
				num_of_free_spaces++;
			}
		}
}
static const FontDescription * f_big = &fontDescription[2];
static const FontDescription * f_huge = &fontDescription[3];
static const char screen_title[] = "SETTINGS" ;
static const char display[] =      "Brightness" ;
static const char screen_off[] =   "Screen Saver";

static void G_Init(void)
{
	mainMemory._gameState = GS_RUNNING;
 mainMemory._gameState = GS_NOT_STARTED;
	mainMemory._lastRenderedTime = 0;
	mainMemory.touch_X = 0;
	mainMemory.touch_Y = 0;
	smemory->_choicebright =brightness;
	smemory->_choice_current = onBrightness;
	smemory->_choicescreen= screen_time;
    //Texture_CopyFrom(texture_rpi);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 240);
    // GPU_DrawFilledSquare(MODE0_DESERT, 0, 0, 320, 40);
    // GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 40);
    GPU_draw_icon_565(10, 40, 32, 32, MODE0_BACK);
    GPU_draw_icon_565(60, 40, 32, 32, MODE0_WHEEL);
    FGPU_DrawText(MODE0_BLACK, f_huge, 100, 43, screen_title, 9);
	FGPU_DrawText(MODE0_BLACK, f_big, 80, 160, screen_off ,12);
	FGPU_DrawText(MODE0_BLACK, f_big,80, 90, display,10);
	GPU_DrawFilledSquare(MODE0_GRAY, 60,115, 200,30 );
	GPU_DrawFilledSquare(MODE0_GRAY, 60,185, 200,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,185, smemory->_choicescreen,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,115, smemory->_choicebright,30 );

	GPU_DrawEmptySquare(MODE0_BLACK, 75,85, 170,30 );
    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_NOTI ,10, 8);
    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_BT , 250, 8);
    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_BATT ,290, 8);
    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_MORE ,120, 48);
    // GPU_DrawText_L(MODE0_BLACK, 150, 47, titleadd ,4);
    // GPU_DrawFilledSquare(MODE0_GRAY, 308, 80, 10, 160);  
    // GPU_draw_icon_565(100,100, 32,32, MODE0_NOTI);
    // GPU_DrawFilledSquare(MODE0_BROWN, 0,0, 320,240);
	// char time[] = "04:00";
    // char * timeadd = &time ;
    // GPU_DrawText_L(MODE0_BLACK, 120, 4, timeadd ,5);
	//     GPU_draw_icon_565(8,4,32,32, MODE0_BT);
	// GPU_draw_icon_565(250,4,32,32, MODE0_NOTI);
	// GPU_draw_icon_565(290,4,32,32, MODE0_BATT);
    GPU_render();
}

#define GPU_REST ( GPU_X/2-2*GPU_BORDER_X )
#define START_X GPU_BORDER_X
#define END_X (GPU_X/2-GPU_BORDER_X)
#define START_Y GPU_BORDER_Y
#define END_Y (GPU_Y-GPU_BORDER_Y)
#define BOX_DIMENSION ((END_X-START_X)/3)
#define S1_CENTER_X ( GPU_X/4 )
#define S1_CENTER_Y ( GPU_Y/2 )
#define S2_CENTER_X ( 3*GPU_X/4 )
#define S2_CENTER_Y ( GPU_Y/2 )

#define RESET_BUTTON_W (5*16)
#define RESET_BUTTON_H (30)
#define RESET_BUTTON_X (S2_CENTER_X - RESET_BUTTON_W/2)
#define RESET_BUTTON_Y (S2_CENTER_Y + 30)

static void G_Update(void)
{
	KeyPressedEnum keyboard_held = Keyboard_GetHeldKeys();
	KeyPressedEnum keyboard = Keyboard_GetPressedKeys();
    if (keyboard == KEY_NONE)
        return;
    if (keyboard & KEY_UP )
    {

						smemory->_choice_current = 1 ;
				GPU_DrawEmptySquare(MODE0_BLACK, 75,85, 170,30 );
				GPU_DrawEmptySquare(MODE0_WHITE, 75,155, 170,30 );


    }
    if (keyboard & KEY_DOWN){

			smemory->_choice_current= 0;
				GPU_DrawEmptySquare(MODE0_WHITE, 75,85, 170,30 );
				GPU_DrawEmptySquare(MODE0_BLACK, 75,155, 170,30 );

   }
    if (keyboard & KEY_RIGHT)
    {
		if(smemory->_choice_current == 1){	
			if(smemory->_choicebright <=95)
			smemory->_choicebright = smemory->_choicebright+ 5;}
		else{	if(smemory->_choicescreen<=9)
		smemory->_choicescreen = smemory->_choicescreen+ 1;}
	// smemory->_choicebright = smemory->_choicebright+ 5;
	// smemory->_choicescreen= smemory->_choicescreen + 1;
	GPU_DrawFilledSquare(MODE0_GRAY, 60,115, 200,30 );
	GPU_DrawFilledSquare(MODE0_GRAY, 60,185, 200,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,185, 20* smemory->_choicescreen,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,115, 2* smemory->_choicebright,30 );


    }
	if (keyboard & KEY_LEFT)
    {
		if(smemory->_choice_current){	
			if(smemory->_choicebright >= 5)
			smemory->_choicebright = smemory->_choicebright- 5;}
		else{	if(smemory->_choicescreen>= 1)
		smemory->_choicescreen = smemory->_choicescreen - 1;}
	// smemory->_choicebright = smemory->_choicebright+ 5;
	// smemory->_choicescreen= smemory->_choicescreen + 1;
	GPU_DrawFilledSquare(MODE0_GRAY, 60,115, 200,30 );
	GPU_DrawFilledSquare(MODE0_GRAY, 60,185, 200,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,185, 20* smemory->_choicescreen,30 );
	GPU_DrawFilledSquare(MODE0_BLACK, 60,115, 2* smemory->_choicebright,30 );
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
	GPU_render();
	
}

static void G_Deinit(void)
{

}

const Callbacks ST_Callbacks = { &G_Init, &G_Update, &G_Draw, &G_Deinit};
