#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "led.h"
#include "joypad.h"
#include "lcd.h"
#include "key.h"
#include "stdio.h"
#include "beep.h"

#define PLAYER 1
#define COMPUTER 2

extern const u8* JOYPAD_SYMBOL_TBL[8];
extern int KeyRead;
extern int gameStatus;
extern int mode;
extern int UpperMove, LowerMove;
extern int xMove;
extern int bounces;
extern int upScore;
extern int lowScore;
extern int Time;

extern int ballPos[2], myBallSizes[3], HighBeaterPos[2], LowBeaterPos[2], beaterShape[2];
extern int whichBall, whichColor, speed, xMove, yMove, directionX, directionY;

extern void delay_ms(u16 time);

extern void lowerBarMove(int* pos, int* size, int* direction);
extern void upBarMove(int* pos, int* size, int* direction);
extern void ballMove(int* ballPos, int whichBall, int whichColor, int xUnit, int yUnit, int dirX, int dirY);


#define LED0_PWM_VAL TIM3->CCR2    
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void InitTimers(void);
#endif























