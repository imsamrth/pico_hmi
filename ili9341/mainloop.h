#ifndef MAIN_LOOP_H_
#define MAIN_LOOP_H_

typedef struct Callbacks {
    void (* initCallback)(void);
    void (* updateCallback) (void);
    void (* drawCallback) (void);
    void (* deinitCallback) (void);
} Callbacks;

void SetupCallbacks(const Callbacks * callbacks);
void ResetGame();
void DoMainLoop();

extern const Callbacks MM_Callbacks;    // Main Menu
extern const Callbacks G2_Callbacks;    // TicTacToe 2P
extern const Callbacks ST_Callbacks;    // Lights Out
extern const Callbacks HM_Callbacks;    // Math
extern const Callbacks G5_Callbacks;    // Tetramino
extern const Callbacks BT_Callbacks;    // Labirinith
extern const Callbacks FS_Callbacks;    // Connect4 1P

// this are extra callbacks for apps not included in HMI
extern const Callbacks G1_Callbacks;    // TicTacToe 1P
extern const Callbacks G8_Callbacks;    // Connect4 2P
extern const Callbacks G9_Callbacks;    // 2048
extern const Callbacks G10_Callbacks;   // Paint.NOT
extern const Callbacks G11_Callbacks;   // Snake
extern const Callbacks G12_Callbacks;   // Calculator
extern const Callbacks G13_Callbacks;   // Pong
extern const Callbacks G14_Callbacks;   // Breakout

extern int breakGame;

#endif//MAIN_LOOP_H_
