#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"
#include "beep.h"
#include "exti.h"
#include "key.h"
#include "timer.h"
#include "joypad.h"
#include "stdio.h"

#define screenEdgeX 480
#define screenEdgeY 800
#define goLeftOrUp -1
#define goRightOrDown 1

// to let the hardware stuck at one point (used for debugging)
void debug()
{
	while(10);
}
void b()//shortcut for debugging 
{
	debug();
}

void showWelcome(void);
void showSelect(void);
void LevelSelectFunction(void);
void PlayerSelectFunction(void);
void showUSARTPage(void); // show the static fram USART waiting
void USARTWorking(void);

void gameCountDown(void);

void gameLoop(void);

void showResult(void);//after a game
// void controlUpperMove(int* pos, int* size, int* direction);

int mode = 1;// 1 palery 2 cpu
int Level = 1; // 1: easy; 2: hard;

int KeyRead = 0;//KEY UP KEYLOW KEY LEFT KEYRIGHT 

int UpperMove = 0;// this is upper move pixels
int LowerMove = 0;//however this is lower move able or not and the direction

int randomNumber;
u8 serialReceive = 0;//the thing recieve from the  serial given by the eie3810.exe program
int tempSign = 0;// used in usart out sign to indicate that you have finished the rx process

int gameStatus = 0;//game statuse ->0  ... 1 ... 2 .... 3.... ,etc
const u8*YourGameStatus[7] = { "Initing","SelectLevel" , "USARTWaiting", "InGame", "GameOutLowWin", "GameOutHighWin", "Exit"};

int bounces = 0;//number of bounces 
int upScore = 0;// socre of the upper guy (one bounces by up guy one score add)
int lowScore = 0;// socre of the lower beater
int Time = 0;// time variable
int speed; // 1 or 2 
int UnitY = 5;//the smallest number of pixels moved in y axis
int UnitX = 3;//the smallest number of pixels moved in x axis
int directionX, directionY;//global variable for the ball direction
int ballPos[2];//global variable to store the position of the ball x and y

int beaterShape[2] = {80, 5};
int HighBeaterPos[2] = {240-80/2, 0};// error update
int LowBeaterPos[2] = {240-80/2, 800-5};//update 

int xMove;
int yMove;
int x,y;//used in the last frame
int myScreenColor = WHITE;
int myBallColors[] = {BLACK, GREEN, BLUE, YELLOW, WHITE, GBLUE, DARKBLUE, RED, CYAN};
int myBallSizes[3] = {9, 19, 39};
int whichBall, whichColor;


void gameLoop()
{
	//do the init thing
	speed = Level;
	xMove = (UnitX * (randomNumber%4+1)) * speed;// init the xmove unit for the ball
	yMove = UnitY * speed; 
	directionX = (randomNumber/4)? goRightOrDown:goLeftOrUp;//go left if smaller than 4 otherwise go right
	directionY = goLeftOrUp;
	whichBall = 0; whichColor = 2;
	ballPos[0] = LowBeaterPos[0]+beaterShape[0]/2; ballPos[1] = LowBeaterPos[1] - myBallSizes[whichBall]-1;
	EIE3810_TFTLCD_FillRectangle(0,480, 0, 800, WHITE);
	EIE3810_TFTLCD_FillRectangle(HighBeaterPos[0], beaterShape[0], HighBeaterPos[1], beaterShape[1],BLACK);
	EIE3810_TFTLCD_FillRectangle(LowBeaterPos[0], beaterShape[0], LowBeaterPos[1], beaterShape[1], BLACK);
	
	//let the timer control the whole thing	
	KeyRead = 0;//clear previous effect 
	gameStatus = 3;//this means we are entering the moving game
	while(gameStatus == 3);
	BEEP = 0;//the beeper must be off after the game
}




int main(void)
{			 
	Stm32_Clock_Init(9);	//系统时钟设置->72MHz
	uart_init(72, 9600);//seems good
	delay_init(72);	   	 	//延时初始化 // enable systick timer
	LED_Init();		  		//初始化与LED连接的硬件接口

	InitTimers();// 50Hz 40 Hz 40Hz tim2 tim3 tim4
	
	JOYPAD_Init();
	EXTIX_Init();//init key and the interrupt correesponded with the key
	BEEP_Init();//comment this line for a freindly testing enviroment, this enable beeper t o work
 	LCD_Init();// init the TFTLCD screen
	
/**** The front page 0***/
	showWelcome();

/**** The game Level selection page 1***/
	gameStatus = 1;//used to update info (not meangingful)
	showSelect();	//hard level (actually the speed issue)
	LevelSelectFunction();//let the user select by using joypad or just the up/down/right button
	PlayerSelectFunction();// let the user to select the player mode ->1. Play with another player or play with chip controlled up beater

	/**** The USART page 2***/
	gameStatus = 2;//used to update info (not meangingful)
	showUSARTPage();//print out the usart page
	USARTWorking();// the working subroutine in using the usart to receive the random bumber
	
	gameCountDown();// 7 segment code to let the thing count down from 3 to 0 with each takes 1 second
/**** counting down then the game starts***/
	gameLoop();//a while loop in the game

/*** the last page ***/
	showResult();// show the result on the screen based on the result (take it longer there will be 彩蛋)
	
	while(1);//stuck in the loop no where should be going
}

//void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
void printStage(char* info)
{
	printString2412(0, 0, info, WHITE, BLUE);//print the current stage on the left up corner of the screen
}

// the first page of this program show on the screen
void showWelcome()
{
	int i;
	int startPosX = 100;
	int startPosY = 200;
	int width = 400;
	int height = 24;
	int size = 24;

	printStage((char*) YourGameStatus[0]);
	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				LCD_ShowString(startPosX,startPosY,width,height,size,"welcome to mini project^_^");	
				printString2412(startPosX, startPosY, "welcome to mini project^_^", WHITE, BLUE);
				startPosX += 24;
				startPosY += 50;
				height = 16;
				size = 16;
				break;
			case 1:
				printString(startPosX, startPosY, "This is Final Lab", WHITE, RED);			
				startPosY += height * 2;
				break;
			case 2:
				printString(startPosX, startPosY, "Are you Ready?", WHITE, RED);	
				startPosY += height * 2;
				break;
			case 3:
				printString(startPosX, startPosY, "OK! Let's Start.", WHITE, BLACK);				
				break;			
		} 
		delay_ms(800);
	}
	EIE3810_TFTLCD_DrawCircle(120, 500, 39, 1, BLACK);
	EIE3810_TFTLCD_DrawCircle(480-120, 500, 19, 1, BLACK);
	EIE3810_TFTLCD_FillRectangle(230, 20, 600, 20, GREEN); 
	EIE3810_TFTLCD_FillRectangle(210, 60, 700, 20*4, BLUE);
	printString2412(260, 700-20, "Enjoy the game!", RED, YELLOW);
	printString2412(260, 700+20, "Wait 2.5 seconds.", RED, YELLOW);
	for (i=0; i<5; i++)	delay_ms(500);
}


//print out the select page (static)
void showSelect()
{
	int startPosX = 50;
	int startPosY = 200;
	int height = 24;
	int size = 24;
	int width = 400;
	int signPos = 8 * 8;//The position to indicate that one is chosen
	int i;
	EIE3810_TFTLCD_Clear(WHITE);
	//LCD_ShowString(startPosX, startPosY-height, width, height, 16, (u8*)YourGameStatus[1]);// debugggingggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg
	printStage((char*)YourGameStatus[1]);
	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Please Select difficulty level:");	
				printString2412(startPosX, startPosY, "Please Select difficulty level:", WHITE, RED);
				startPosY += height * 2;
				height = 16;
				size = 16;
				break;
			case 1:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Easy");
				printString(startPosX, startPosY, "Easy", WHITE, BLUE);
				startPosY += height * 2;
				break;
			case 2:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Hard");	
				printString(startPosX, startPosY, "Hard", BLUE, WHITE);
				startPosY += height * 2;
				break;
			case 3:
				LCD_ShowString(startPosX,startPosY,width,height,size,"Press Key0 to enter");	
				printString(startPosX, startPosY, "Make sure you know what you are going to face.", WHITE, GRAY);
				printString(startPosX, startPosY+16, "Haha...", WHITE, GRAY);
				startPosX += signPos;
				break;			
		} 
	}
}


//let the user to choose the hard level of this game (button or using joypad)
void LevelSelectFunction()
{
	while(1)
	{
		int height = 16;
		int startPosX = 50;
		int startPosY = 200 + 24 * 2;

		
		if (KeyRead == KEYRIGHT)
		{
			KeyRead = 0;
			LCD_Clear(YELLOW);
			break;
		}
		if (KeyRead == KEYDOWN && Level == 1)//down
		{	
			printString(startPosX, startPosY, "Easy", BLUE, WHITE);
			printString(startPosX, startPosY+ height*2, "Hard", WHITE, BLUE);
			Level = 2;
			KeyRead = 0;
		}
		if (KeyRead == KEYUP && Level == 2)//up
		{
			printString(startPosX, startPosY, "Easy", WHITE, BLUE);
			printString(startPosX, startPosY+ height*2, "Hard", BLUE, WHITE);
			Level = 1;
			KeyRead = 0;
		}
	}
}



//let the user to choose the palyer mode of this game (single(with CPU) or double with another player using joypad)
void PlayerSelectFunction()
{
	int startPosX = 50;
	int startPosY = 200;
	int height = 24;
	int size = 24;
	int width = 400;
	int signPos = 8 * 8;//The position to indicate that one is chosen
	int i;
	delay_ms(1000);
	EIE3810_TFTLCD_Clear(WHITE);
	printStage((char*)YourGameStatus[1]);
	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Please Select difficulty level:");	
				printString2412(startPosX, startPosY, "Please Select Your Player:", WHITE, RED);
				startPosY += height * 2;
				height = 16;
				size = 16;
				break;
			case 1:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Easy");
				printString(startPosX, startPosY, "P2P Mode", WHITE, BLUE);
				startPosY += height * 2;
				break;
			case 2:
				//LCD_ShowString(startPosX,startPosY,width,height,size,"Hard");	
				printString(startPosX, startPosY, "Computer Mode", BLUE, WHITE);
				startPosY += height * 2;
				break;
			case 3:
				LCD_ShowString(startPosX,startPosY,width,height,size,"Press Key0 to enter");	
				printString(startPosX, startPosY, "Make sure you know what you are going to face.", WHITE, GRAY);
				printString(startPosX, startPosY+16, "Haha...", WHITE, GRAY);
				startPosX += signPos;
				break;			
		} 
	}
	mode = PLAYER;
	
	while(1)//continue in the choosing page until the mode of the game is chosen
	{
		int height = 16;
		int startPosX = 50;
		int startPosY = 200 + 24 * 2;
		
		if (KeyRead == KEYRIGHT)
		{
			KeyRead = 0;
			LCD_Clear(YELLOW);
			break;// selection finish out of the looop
		}
		if (KeyRead == KEYDOWN && mode == PLAYER)//down
		{	
			printString(startPosX, startPosY, "P2P Mode", BLUE, WHITE);
			printString(startPosX, startPosY+ height*2, "Computer Mode", WHITE, BLUE);
			mode = COMPUTER;
			KeyRead = 0;
		}
		if (KeyRead == KEYUP && mode == COMPUTER)//up
		{
			printString(startPosX, startPosY, "P2P Mode", WHITE, BLUE);
			printString(startPosX, startPosY+ height*2, "Computer Mode", BLUE, WHITE);
			mode = PLAYER;
			KeyRead = 0;
		}
	}
}

void showUSARTPage(void)
{
	int startPosX = 50;
	int startPosY = 200;
	LCD_Clear(BLUE);
	printStage((char*) YourGameStatus[2]);
	printString2412(startPosX, startPosY, "Use USART for a random direction.", WHITE, RED);
}

void USARTWorking()// receive the data and yeah.. you know  let's just pretend it work
{
	int size = 24;
	int sudoDelayTime;
	while(tempSign == 0) 
	{
		LCD_ShowNum(80, 700, size, 2, 24);//the time so that you know that the boad is not in dead state
		size++;
		printf("Hello, waiting For your number.:)\r\n");
		delay_ms(1000);
	}
	EIE3810_TFTLCD_ShowChar2412(300, 480, randomNumber+'0', WHITE, RED);//print the random number on the screen
	for (sudoDelayTime = 2; sudoDelayTime >= 0; sudoDelayTime --) delay_ms(1000);
}
void USART1_IRQHandler(void)
{
	u32 CR1; u32 buffer;
	if (!EN_USART1_RX) return;
	if ((USART1->SR & (1<<5)) && (gameStatus == 2) && (!tempSign)) //check if received data is ready to be read
	{
		buffer=USART1->DR + 0x30; //read the data value to buffer
		CR1=USART1->CR1;
		if (buffer== '0')
		{
			printString(120, 450, "The random number received is: 0", WHITE, RED);
		}
		if (buffer=='1')
		{
			printString(120, 450, "The random number received is: 1", WHITE, RED);
		}
		if (buffer=='2')
		{
			printString(120, 450, "The random number received is: 2", WHITE, RED);
		}
		if (buffer=='3')
		{
			printString(120, 450, "The random number received is: 3", WHITE, RED);

		}
		if (buffer=='4')
		{
			printString(120, 450, "The random number received is: 4", WHITE, RED);
		}
		if (buffer=='5')
		{
			printString(120, 450, "The random number received is: 5", WHITE, RED);
		}
		if (buffer=='6')
		{
			printString(120, 450, "The random number received is: 6", WHITE, RED);
		}
		if (buffer=='7')
		{
			printString(120, 450, "The random number received is: 7", WHITE, RED);
		}
		randomNumber = buffer; 
		if ((randomNumber >= '0') && (randomNumber <= '7')) randomNumber = buffer - '0';
		else randomNumber = '4' - '0';//if random number is between 0 and 7, the random number is manually chosen as '4'
		printf(" my game status is :%d\r\n",gameStatus); 
		printf(" Rx randomNumber:%d\r\n",randomNumber); 
		NVIC->ISER[5] &= 0x011111;// use this to stop the interrupt handler function but seems not working
		USART_RX_STA = 0;//
		tempSign = 1;//used in the main program's usartworking function
	}
	else {
		NVIC->ISER[5] &= 0x011111;
		buffer=USART1->DR + 0x30; //read the data value to buffer
		CR1=USART1->CR1;
		USART_RX_STA = 0;
		//printf(" Rx randomNumber:%d\r\n",randomNumber); //used in debugging (you can check it out in the xcom.exe with the serial open)
		printf("Current page index is %d \r\n", gameStatus);
		printf("This means you are in trouble! haha kid!");
	}
} 

void upBarMove(int* pos, int* size, int* direction)
{
	int posX = *pos; int posY = *(pos+1);int moveUnit = 10;//
	//*direction the forced moving direction -> larger larger move
	if ((*direction)*goLeftOrUp >= moveUnit)
	{
		if (ballPos[1] <= 200 && mode == COMPUTER  && (*direction * goLeftOrUp >= xMove)) moveUnit = xMove*1.3;//紧急时刻的移动速度必须要快！
		
		if (posX>=moveUnit)
		{
			*direction -= goLeftOrUp*moveUnit;

			EIE3810_TFTLCD_FillRectangle(posX+beaterShape[0]-moveUnit, moveUnit, posY, beaterShape[1], WHITE);
			posX+=goLeftOrUp*moveUnit; *pos = posX;
			EIE3810_TFTLCD_FillRectangle(posX, moveUnit, posY, beaterShape[1], BLACK);
		}
		else //如果不能再往左移动了 就请空direction 因为没有意义了呀
		{
			if (posX != 0)
			{
				EIE3810_TFTLCD_FillRectangle(posX+beaterShape[0]-posX, moveUnit, posY, beaterShape[1], WHITE);
				posX+=goLeftOrUp*posX; *pos = posX;
				EIE3810_TFTLCD_FillRectangle(posX, moveUnit, posY, beaterShape[1], BLACK);
			}
			*direction = 0;
		}
	}
	else if (*direction * goRightOrDown >= moveUnit)
	{
		if (posX<=480-beaterShape[0]-moveUnit)
		{
			*direction -= goRightOrDown*moveUnit;
			EIE3810_TFTLCD_FillRectangle(posX, moveUnit, posY, beaterShape[1], WHITE);
			posX+=goRightOrDown*moveUnit; *pos = posX;
			EIE3810_TFTLCD_FillRectangle(posX+beaterShape[0]-moveUnit, moveUnit, posY, beaterShape[1], BLACK);
		}
		else {
			if (posX != 480-beaterShape[0])
			{
				EIE3810_TFTLCD_FillRectangle(posX, 480-posX-beaterShape[0], posY, beaterShape[1], WHITE);//this could be better shaped
				posX = 480-beaterShape[0]; *pos = posX;
				EIE3810_TFTLCD_FillRectangle(posX, beaterShape[0], posY, beaterShape[1], BLACK);
			}
			*direction = 0;
		}
	}
}

void lowerBarMove(int* pos, int* size, int* direction)
{
	int posX = *pos, posY = *(pos+1);int moveUnit = 5;
	
	if (*direction == goLeftOrUp)
	{
		if (posX >= moveUnit)
		{
			EIE3810_TFTLCD_FillRectangle(posX+beaterShape[0]-moveUnit, moveUnit, posY, beaterShape[1], WHITE);
			posX -= moveUnit; *pos = posX;
			EIE3810_TFTLCD_FillRectangle(posX, moveUnit, posY, 5, BLACK);
		}
		*direction = 0;
	}
	else if (*direction == goRightOrDown)
	{
		if (posX <= 480-beaterShape[0]-moveUnit)
		{
			EIE3810_TFTLCD_FillRectangle(posX, moveUnit, posY, beaterShape[1], WHITE);
			posX += moveUnit; *pos = posX;
			EIE3810_TFTLCD_FillRectangle(posX+beaterShape[0]-moveUnit, moveUnit, posY, beaterShape[1], BLACK);
		}
		*direction = 0;
	}
}

void gameCountDown()
{
	int startPosX = 200, startPosY = 400;
	u32 i = 4;
	LCD_Clear(MAGENTA);
	while(i--)
	{
		BACK_COLOR = MAGENTA;
		EIE3810_TFTLCD_SevenSegment(startPosX, startPosY, i, YELLOW);
		delay_ms(1000); 
	}
	BACK_COLOR = WHITE;
	LCD_Clear(WHITE);

}

void showResult(void){
	int startPosX =  50;
	int startPosY = 300;
	int width = 400;
	int size = 24;
	int height = 24*2;
	LCD_Clear(MAGENTA);
	POINT_COLOR = GREEN; BACK_COLOR = MAGENTA;
	switch(gameStatus)
	{
		case 6://this is not used in this program
			LCD_ShowString(startPosX, startPosY, width, height, size, "This is impossible, that you exit the game");// this is not used
			break;
		case 4:
			LCD_ShowString(startPosX, startPosY, width, height, size, "OMG, the Lower player wins!");
			break;
		case 5:
			LCD_ShowString(startPosX, startPosY, width, height, size, "Oh nice, the Upper dude wins!");
			break;
	}
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);

	LCD_Clear(LGRAY);
	//show some werid ending  frames Haha
	EIE3810_TFTLCD_DrawCircle(240, 400, 199, 1, GREEN);
	EIE3810_TFTLCD_DrawCircle(240, 400, 99, 1, YELLOW);
	EIE3810_TFTLCD_DrawCircle(240, 400, 49, 1, GRAY);
	EIE3810_TFTLCD_FillRectangle(80, 480-80-80, 650, 75, RED);
	EIE3810_TFTLCD_FillRectangle(120, 20, 680, 50, WHITE);
	EIE3810_TFTLCD_FillRectangle(400-60, 20, 680, 50, WHITE);
	//speed here is just used as a variable of nothign specific
	x = 240-10; y = 10;
	for (speed = 5; speed>0;speed-- )
	{
		EIE3810_TFTLCD_DrawCircle(x, y, 9, 1, BLACK);
		y += 2;
	}// 点
	y = 45; x = 240-50;
	for (speed = 50; speed>0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x, y, 9, 1, BLACK);
		x += 2;
	}
	x = 240 - 50;
	for (speed = 10; speed>0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x,y,9,1,BLACK);
		EIE3810_TFTLCD_DrawCircle(x+100, y, 9 ,1, BLACK);
		y+=2;
	}
	x = 240 - 20; y -= 2;
	for (speed = 20; speed>0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x, y, 7, 1, BLACK);
		EIE3810_TFTLCD_DrawCircle(x, y+24, 9, 1, BLACK);
		x+=2;
	}
	x = 240 - 10; y += 30;
	for (speed = 25; speed >0 ; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x, y, 5, 1, BLACK);
		EIE3810_TFTLCD_DrawCircle(x+20, y, 7, 1, BLACK);
		y+=2;
	}
	for (speed = 12; speed >0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x,y,5,1,BLACK);
		x-=2;
	}
	x = 240+10;
	for (speed = 18; speed >0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x, y, 7, 1, BLACK);
		x+=2;
	}
	for (speed = 4; speed >0; speed--)
	{
		EIE3810_TFTLCD_DrawCircle(x, y-10, (speed+1), 1, BLACK);
		y-=2;
	}
	POINT_COLOR = DARKBLUE;
	LCD_DrawLine(0,0, 479, 799);
	POINT_COLOR = GRED;
	LCD_DrawLine(0,799, 479, 0);
	POINT_COLOR = BROWN;
	LCD_DrawLine(0, 400, 479, 400);
	POINT_COLOR = CYAN;
	LCD_DrawLine(240, 0, 240, 799);
	for (speed = 10; speed >0; speed--)
	{
		delay_ms(1000);
	}//delay for 10 ms
	//refresh the screen from up to low
	for (speed = 0; speed < 800; speed+=10)
	{
		EIE3810_TFTLCD_FillRectangle(0, 480, speed, 10, myBallColors[(whichColor++)%9]);
		delay_ms(700);//这是彩蛋
	}
}
/*
level: 1 or 2
mode: 1 or 2 -> cpu or player
direction: 0-7
*/
// this function will change the game status at the end

// void EIE3810_TFTLCD_ClearCircle(int* pos, u16 radius, u16 color)
// {
// 	u16 xPos = *(pos);
// 	u16 yPos = *(pos+1);
// 	u16 plotX, plotY, xlen, ylen;
// 	plotX = xPos-radius; plotY = yPos-radius; xlen= 2*radius; ylen=2*radius;
// 	if (xPos<=479-radius && xPos >= 0+radius && yPos>=0+radius && yPos<=799-radius) 
// 	{ EIE3810_TFTLCD_FillRectangle(plotX, xlen, plotY, ylen,color); return;}
// 	else EIE3810_TFTLCD_DrawCircle(xPos,yPos, radius, 1, color);
// }

void ballMove(int* ballPos, int whichBall, int whichColor, int xUnit, int yUnit, int dirX, int dirY)
{
	EIE3810_TFTLCD_DrawCircle(*ballPos, *(ballPos+1), myBallSizes[whichBall], 1, WHITE);//clear previous ball
	*ballPos += xUnit * (dirX);//update the position info
	*(ballPos+1) += yUnit * (dirY);
	EIE3810_TFTLCD_DrawCircle(*ballPos, *(ballPos+1), myBallSizes[whichBall], 1, myBallColors[whichColor]);//generatet the ball at the new position
}















