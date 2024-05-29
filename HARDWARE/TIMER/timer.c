#include "timer.h"


//定时器3中断服务程序	 

int Time2 = 0;
char TIME[40];
char UScore[40];
char LScore[40];
char TOUCHES[40];



void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR |= 1<<0;
	TIM2->ARR = arr;
	TIM2->PSC = psc;
	TIM2->DIER|= 1<<0;
	TIM2->CR1 |= 0x11;
	MY_NVIC_Init(3,2,TIM2_IRQn,0);
}

void TIM2_IRQHandler(void)
{
	static int beepTime = 0, ballTouches = 0;
//	static int lastKey = -3;// 
	if (gameStatus == 3 && (TIM2->SR & 1<<0))
	{
		if (Time2%25 == 0) // update the info thing every 25 frame -> around half a second
		{
			sprintf(TOUCHES,"Number of Bounces: %d", bounces);
			sprintf(UScore, "Up Player Score:%d", upScore);
			sprintf(LScore, "Low Player Score: %d", lowScore);
			sprintf(TIME, "Time: %d", Time2/50); // 50 帧率
			printString(0, 700-36, TOUCHES, RED, WHITE);
			printString(0, 700, UScore, RED, WHITE);
			printString(0, 700+36, LScore, RED, WHITE);
			printString(0, 700+72, TIME, RED, WHITE);
		}
		if (beepTime)//manage the hit beeper
		{
			if (!(--beepTime)) BEEP = 0; // stop the beeper when the beep time is cleared to 0
		}
		if (Time2%3 == 0)//move the ball and do the detection issues
		{

			ballMove(ballPos, whichBall, whichColor, xMove, yMove, directionX, directionY);
			//printf("the UpperMoveNumber %d\r\n", UpperMove);//(debug)
			//printf("the uppbar is %d %d 'r'n", HighBeaterPos[0], HighBeaterPos[0]+beaterShape[0]);
			//printf("The ball is at %d %d\r\n", *ballPos, *(ballPos+1));
			if (ballTouches!=0)
			{
				ballTouches--;
				EIE3810_TFTLCD_FillRectangle(HighBeaterPos[0], beaterShape[0], HighBeaterPos[1], beaterShape[1],BLACK);
				EIE3810_TFTLCD_FillRectangle(LowBeaterPos[0], beaterShape[0], LowBeaterPos[1], beaterShape[1], BLACK);
			}
			// printf("my x %d my y %d\r\n", *ballPos, *(ballPos+1));
			// printf("lower barX %d higher barX %d\r\n", LowBeaterPos[0], HighBeaterPos[0]);
			
			//detect collision with the border
			if (*ballPos >= (480-myBallSizes[whichBall]) || *ballPos <= myBallSizes[whichBall])
			{
				directionX *= (-1);
				bounces += 1;
				BEEP = 1;
				beepTime = 5;
				whichColor = (++whichColor)%9;//update the color of the ball
			}
			//detect collision with the beater
			if (*(ballPos+1) <= 0+1+beaterShape[1] + myBallSizes[whichBall] && directionY <0)
			{
				printf("my ball x %d my ball y%d my r is %d\r\n", *ballPos, *(ballPos+1), myBallSizes[whichBall]);
				if (HighBeaterPos[0] + beaterShape[0] >= *ballPos && HighBeaterPos[0] <= *ballPos)
				{
					directionY *= -1;
					bounces += 1;
					upScore +=1;
					BEEP = 1;
					beepTime = 5;
					// EIE3810_TFTLCD_FillRectangle(HighBeaterPos[0], beaterShape[0], HighBeaterPos[1], beaterShape[1], BLACK);
					ballTouches = 3;
					whichColor = (whichColor*7)%4;
				}
				else {
					beepTime = 0;
					gameStatus = 4;//lower guy win
				}
			} 
			else if (*(ballPos+1) + myBallSizes[whichBall] >= 799-beaterShape[1] && directionY > 0)
			{
				printf("my ball x %d my ball y%d my r is %d\r\n", *ballPos, *(ballPos+1), myBallSizes[whichBall]);
				if (LowBeaterPos[0] + beaterShape[0] >= *ballPos && LowBeaterPos[0] <= *ballPos)
				{
					directionY *= -1;
					bounces += 1;
					lowScore+=1;

					if (lowScore != 0 &&(lowScore%3 == 0)) 
					{
						ballMove(ballPos, whichBall, 4, 0, 0, 0, 0);// white it;
						whichBall = (++whichBall)%3;/// - 1 2
						ballMove(ballPos, whichBall, whichColor, 0, 0, 0, 0);
					}

					BEEP = 1;
					beepTime = 5;
					ballTouches = 3;
					// EIE3810_TFTLCD_FillRectangle(LowBeaterPos[0], beaterShape[0], LowBeaterPos[1], beaterShape[1], BLACK);
					EIE3810_TFTLCD_FillRectangle(HighBeaterPos[0], beaterShape[0], HighBeaterPos[1], beaterShape[1],BLACK);
					EIE3810_TFTLCD_FillRectangle(LowBeaterPos[0], beaterShape[0], LowBeaterPos[1], beaterShape[1], BLACK);
					whichColor = (++whichColor)%9;
					if (whichColor == 4) whichColor = 1;
				}
				else {
					beepTime = 0;
					gameStatus = 5;//upper guy win
				}
			}
			// make sure the beatter can see the ball at the close 
			// if the ball is moving to the up region and is 200pixles to the boarder it should be visible and vice versa to the down driection case
			if(directionY == -1 && ballPos[1] <= 200 && whichColor == 4 || (directionY == 1 && ballPos[1] >= 600 && whichColor == 4)) whichColor = 0;//make sure the beater can see the ball at least when beating
	
		}
		
		//move the beater if the beater should be moved
		if (UpperMove != 0) upBarMove(HighBeaterPos, beaterShape, &UpperMove);
		if (LowerMove != 0) lowerBarMove(LowBeaterPos, beaterShape, &LowerMove);

		// pause and resume function
		if (KeyRead == KEYDOWN)
		{
			KeyRead = 0;
			printf("Entering the KEY stop mode \r\n");
			while(KEY1 == 0);
			printf("Busy waiting for the down KEY\r\n");
			LCD_ShowString(100, 400, 300, 80, 24, "In pause");
			BEEP = 0;
			while(1)
			{
				if (KEY1 == 0) 
				{
					BACK_COLOR = WHITE;
					LCD_ShowString(100, 400, 300, 80, 24, "Exiting the wait status");
					delay_ms(500);
					LCD_ShowString(100, 400, 300, 80, 24, "                          ");
					delay_ms(300);
					break;
				}
				delay_ms(10);
			}
			if (beepTime) BEEP = 1;
			printf("the game now resume! Enjoy\r\n");
		}
		else if (KeyRead == KEYUP && speed == 1){
			//do the speed up thing
			speed = 2;
			xMove *= speed; yMove *= speed;//acceleration to the hard mode
			POINT_COLOR = LIGHTGREEN; BACK_COLOR = WHITE;
			LCD_ShowString(100, 400, 300, 80, 24, "Hold my gear! Don't be panic.");
			delay_ms(500);
			LCD_ShowString(100, 400, 300, 80, 24, "                             ");
			POINT_COLOR = BLACK;
		}

		Time2++; 
	}
	TIM2->SR &= ~(1<<0);
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(3,3,TIM3_IRQn,0);//抢占1，子优先级3，组0									 
}

//handler for joypad reading
void TIM3_IRQHandler(void)
{ 		    		  			    
	static u8 key;
	int i;
	if(TIM3->SR&0X0001)//溢出中断 时间到了
	{
		if (gameStatus >= 3) !LED1;// flash in the game!
		
		// Scan the joypad handler
		// in player mode and if in the game,  it just control the upper bar or pause the game and wait for re start
		// while in the computer mode
		if (mode == PLAYER || gameStatus == 1) // 1-> choosing page//这里的函数写的不好 太复杂了应该简化
		{
			key = JOYPAD_Read();
			if (key)//shape out the 0 case
			{
				for(i=0;i<8;i++)//use iteration to detect th key.... quite low efficientcy
				{
					if(key&(0X80>>i))//it find the match!
					{
						switch(i)
						{
							case 0:
								if (gameStatus!=3 || mode == 2) KeyRead = KEYRIGHT;
								else {
									UpperMove +=10;//how to let ti control other
								}
								printf("key0\r\n");
								break;
							case 1:
								if (gameStatus!=3 || mode == 2)KeyRead = KEYLEFT;
								else {
									UpperMove -=10;//
								}
								printf("key1\r\n");
								break;
							case 2:
								if (gameStatus == 3) KeyRead = 0;
								else KeyRead = KEYDOWN;			
								printf("key2\r\n");				
								break;
							case 3:
								KeyRead = KEYUP;
								printf("key3\r\n");
								break;
							case 4://start key pressed
								if (gameStatus == 3 && mode == PLAYER)//do the pause thing by busy waiting
								{
									BACK_COLOR = WHITE;POINT_COLOR = RED;
									LCD_ShowString(50, 300, 400, 40, 24, "You are pasued");
									BEEP = 0;//stop the beeper anyway
									while(JOYPAD_Read() == 8);//wait for the first push stops
									while(1){
										KeyRead = JOYPAD_Read();
										if (KeyRead != 0) 
										{
											printf("The receive keyread is %d\r\n",KeyRead);

										}
										if (KeyRead == 8) 
										{
											printf("Your game now returns! Enjoy! \r\n");
											LCD_ShowString(50, 300, 400, 40, 24, "Wait for release");//clear 
											KeyRead = 0;// if key read == 4 -> it will catch error in the another thing
											while(JOYPAD_Read() == 8);//wait for the second push stops
											LCD_ShowString(50, 300, 400, 40, 24, "                ");//clear 
											break;
										}
									}
								}
								break;
							case 5://select not used 
								KeyRead = 0;
								break;
							case 6://B // not used 
								KeyRead = 0;
								break;
							case 7://A // not used
								KeyRead = 0;
								break;												
						}
					}		
				}
				
			}
		}
		else if (mode == COMPUTER && gameStatus == 3)
		{
			if (directionY == -1)//this means the ball is going up or go left(ofcouser here it use the up)
			{
				//离得远 就轻松应对
				//离得近 就专门应对
				// ball is at the right part of the bar? => move right something
				if (ballPos[1] > 300) UpperMove = (ballPos[0]+xMove*directionX - (HighBeaterPos[0] + beaterShape[0]/2));  
				//if (ballPos[1] >200) UpperMove = (ballPos[0]>(HighBeaterPos[0]+beaterShape[0]/2))? (ballPos[0]-(HighBeaterPos[0]+beaterShape[0]/2))/2 : -((HighBeaterPos[0]+beaterShape[0])-ballPos[0])/2;
				else UpperMove = (ballPos[0]+xMove*directionX-beaterShape[0]/2-HighBeaterPos[0]);
				// if (ballPos[1] > 200) ;
				// else UpperMove = (ballPos[0] + xMove*directionX - beaterShape[0]/2 - HighBeaterPos[0]);
			}
			else {
				if (HighBeaterPos[0] != 240-beaterShape[0]/2) UpperMove = (240 - beaterShape[0]/2 - HighBeaterPos[0])/2;
			}
		}	
	}			   
	TIM3->SR&=~(1<<0);//清除中断标志位 如果不清除会有悲剧发生	    
}



//update the data info
void TIM4_Int_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |=1<<2;     //Enable TIM4
    TIM4->ARR = arr;         //Set auto-reload register.
    TIM4->PSC = psc;         //Set Prescaler.
    TIM4->DIER|=1<<0;         //Interrupt enable register, update interrupt enabled.
    TIM4->CR1 |=0x01;         //Control register, enable counter.
	MY_NVIC_Init(3,0, TIM4_IRQn,0);
}
//update for the lower bar
void TIM4_IRQHandler(void)
{    
	if ((TIM4->SR & 1<<0) && (gameStatus ==3 ))
	{
		if (KEY0 == 0)
		{
			LowerMove = 1;//1 means move right
		}
		else if (KEY2 == 0)
		{
			LowerMove = -1;//-1 means move left
		}
	}
	TIM4->SR &= ~(1<<0);
}



void InitTimers()
{
	TIM2_Int_Init(200 -1, 72e2 -1);//update the time variable 0 second 1 seond .... 50Hz
	TIM3_Int_Init(250-1, 7199);	//40Hz
	TIM4_Int_Init(250-1, 7199);// 20Hz 100Hz 就非常顺滑 我觉得就40Hz吧
	//72E6 = 100 * 72E4
	//1MS -> 72E6/1E3 = 72E3
	//10mS -> 72E4
	//20ms (50Hz)
}






