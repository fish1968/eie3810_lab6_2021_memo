#include "exti.h"


//外部中断0服务程序

extern int KeyRead;
extern int gameStatus;
extern int LowerMove;
void EXTI0_IRQHandler(void)
{
	delay_ms(2);	//消抖
	if(WK_UP==1)	//WK_UP按键
	{				 
		KeyRead = KEYUP;
		
	}		 
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
}
//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	delay_ms(2);	//消抖
	if(KEY2==0)	 	//按键KEY2
	{
		KeyRead = KEYLEFT;
		if (gameStatus == 3)
		{
			LowerMove = -1;
		}
	}		 
	EXTI->PR=1<<2;  //清除LINE2上的中断标志位  
}
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	delay_ms(2);	//消抖
	if(KEY1==0)	 	//按键KEY1
	{				 
		KeyRead = KEYDOWN;
	}		 
	EXTI->PR=1<<3;  //清除LINE3上的中断标志位  
}
//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	delay_ms(2);	//消抖
	if(KEY0==0)	 	//按键KEY0
	{
		KeyRead = KEYRIGHT;
		if (gameStatus == 3) LowerMove = 1; 
	}		 
	EXTI->PR=1<<4;  //清除LINE4上的中断标志位  
}		   

void EXTIX_Init(void)
{
	KEY_Init();// interrupt about key are usable only afte rkey are usable
	Ex_NVIC_Config(GPIO_A,0,RTIR); 	//上升沿触发
	Ex_NVIC_Config(GPIO_E,2,FTIR); 	//下降沿触发
	Ex_NVIC_Config(GPIO_E,3,FTIR); 	//下降沿触发
 	Ex_NVIC_Config(GPIO_E,4,FTIR); 	//下降沿触发  
	MY_NVIC_Init(2,3,EXTI0_IRQn,0);	//没有抢占优先级
	MY_NVIC_Init(2,2,EXTI2_IRQn,0);	//2,2 since no preempt priority doesn't mean too much actaully
	MY_NVIC_Init(2,1,EXTI3_IRQn,0);	//
	MY_NVIC_Init(2,0,EXTI4_IRQn,0);	//	   
   
}












