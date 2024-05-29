#include "exti.h"


//�ⲿ�ж�0�������

extern int KeyRead;
extern int gameStatus;
extern int LowerMove;
void EXTI0_IRQHandler(void)
{
	delay_ms(2);	//����
	if(WK_UP==1)	//WK_UP����
	{				 
		KeyRead = KEYUP;
		
	}		 
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ  
}
//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	delay_ms(2);	//����
	if(KEY2==0)	 	//����KEY2
	{
		KeyRead = KEYLEFT;
		if (gameStatus == 3)
		{
			LowerMove = -1;
		}
	}		 
	EXTI->PR=1<<2;  //���LINE2�ϵ��жϱ�־λ  
}
//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	delay_ms(2);	//����
	if(KEY1==0)	 	//����KEY1
	{				 
		KeyRead = KEYDOWN;
	}		 
	EXTI->PR=1<<3;  //���LINE3�ϵ��жϱ�־λ  
}
//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{
	delay_ms(2);	//����
	if(KEY0==0)	 	//����KEY0
	{
		KeyRead = KEYRIGHT;
		if (gameStatus == 3) LowerMove = 1; 
	}		 
	EXTI->PR=1<<4;  //���LINE4�ϵ��жϱ�־λ  
}		   

void EXTIX_Init(void)
{
	KEY_Init();// interrupt about key are usable only afte rkey are usable
	Ex_NVIC_Config(GPIO_A,0,RTIR); 	//�����ش���
	Ex_NVIC_Config(GPIO_E,2,FTIR); 	//�½��ش���
	Ex_NVIC_Config(GPIO_E,3,FTIR); 	//�½��ش���
 	Ex_NVIC_Config(GPIO_E,4,FTIR); 	//�½��ش���  
	MY_NVIC_Init(2,3,EXTI0_IRQn,0);	//û����ռ���ȼ�
	MY_NVIC_Init(2,2,EXTI2_IRQn,0);	//2,2 since no preempt priority doesn't mean too much actaully
	MY_NVIC_Init(2,1,EXTI3_IRQn,0);	//
	MY_NVIC_Init(2,0,EXTI4_IRQn,0);	//	   
   
}












