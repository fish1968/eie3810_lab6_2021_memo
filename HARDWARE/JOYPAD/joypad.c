#include  "joypad.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//游戏手柄驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//初始化手柄接口.	 
const u8*JOYPAD_SYMBOL_TBL[8]={"Right","Left","Down","Up","Start","Select","A","B"};//手柄按键符号定义

void 	JOYPAD_Init(void)
{	
 	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟    
 	RCC->APB2ENR|=1<<5;//先使能外设PORTD时钟    	
	GPIOB->CRH&=0XFFFF00FF;
	GPIOB->CRH|=0X00003800;     
	GPIOB->ODR|=3<<10; 
	GPIOD->CRL&=0XFFFF0FFF;	//PD3推挽输出
	GPIOD->CRL|=0X00003000;     
	GPIOD->ODR|=1<<3; 
}
//手柄延迟函数
//t:要延迟的时间
void JOYPAD_Delay(u16 t)
{
	while(t--);
}
//读取手柄按键值.
//FC手柄数据输出格式:
//每给一个脉冲,输出一位数据,输出顺序:
//A->B->SELECT->START->UP->DOWN->LEFT->RIGHT.
//总共8位,对于有C按钮的手柄,按下C其实就等于A+B同时按下.
//按下是1,松开是0.
//返回值:
//[7]:右
//[6]:左
//[5]:下
//[4]:上
//[3]:Start
//[2]:Select
//[1]:B
//[0]:A
u8 JOYPAD_Read(void)
{
	vu8 temp=0;
 	u8 t;
	JOYPAD_LAT=1;	//锁存当前状态
 	JOYPAD_Delay(80);
	JOYPAD_LAT=0; 
	for(t=0;t<8;t++)
	{
		temp>>=1;	 
		if(JOYPAD_DAT==0)temp|=0x80;//LOAD之后，就得到第一个数据
		JOYPAD_CLK=1;			   	//每给一次脉冲，收到一个数据
		JOYPAD_Delay(80);
		JOYPAD_CLK=0;	
		JOYPAD_Delay(80); 
	}
	return temp;
}





