#include "usart.h"	  
#include "led.h"
#include "lcd.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化（适合STM32F10x系列）		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2010/1/1
//版本：V1.7
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//V1.6修改说明 20150109
//uart_init函数去掉了开启PE中断
//V1.7修改说明 20150322
//修改OS_CRITICAL_METHOD宏判断为：SYSTEM_SUPPORT_OS
////////////////////////////////////////////////////////////////////////////////// 	  
 
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定向fputc函数
//printf的输出，指向fputc，由fputc输出到串口
//这里使用串口1(USART1)输出printf信息
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
	USART1->DR = (u8) ch;      	//写DR,串口1将发送数据
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
extern u8 serialReceive;
extern int gameStatus;

// void USART1_IRQHandler(void)
// {
// 	u32 CR1; u32 buffer;
// 	if (!EN_USART1_RX) return;
// 	if ((USART1->SR & (1<<5)) && (gameStatus == 2)) //check if received data is ready to be read
// 	{
// 		buffer=USART1->DR + 0x30; //read the data value to buffer
// 		CR1=USART1->CR1;
// 		if (buffer== '0')
// 		{
// 			printString(120, 450, "The random number received is: 0", WHITE, RED);
// 		}
// 		if (buffer=='1')
// 		{
// 			printString(120, 450, "The random number received is: 1", WHITE, RED);
// 		}
// 		if (buffer=='2')
// 		{
// 			printString(120, 450, "The random number received is: 2", WHITE, RED);
// 			gameStatus = 3;
// 		}
// 		if (buffer=='3')
// 		{
// 			printString(120, 450, "The random number received is: 3", WHITE, RED);

// 		}
// 		if (buffer=='4')
// 		{
// 			printString(120, 450, "The random number received is: 4", WHITE, RED);
// 		}
// 		if (buffer=='5')
// 		{
// 			printString(120, 450, "The random number received is: 5", WHITE, RED);
// 		}
// 		if (buffer=='6')
// 		{
// 			printString(120, 450, "The random number received is: 6", WHITE, RED);
// 		}
// 		if (buffer=='7')
// 		{
// 			printString(120, 450, "The random number received is: 7", WHITE, RED);
// 		}
// 		printf(" my data:%d\r\n",gameStatus); //打印LCD ID 
// 		USART_RX_STA = 0;
// 	}
// 	return;
// } 

//void USART1_IRQHandler(void)
//{
//	u32 xxx = 1000000;
//	if ((USART1->SR & (1<<5)) && (gameStatus == 2)) //check if received data is ready to be read
//	{
//		serialReceive=USART1->DR + 0x30; //read the data value to buffer
//		if (serialReceive >= '0' || serialReceive <= '7') gameStatus = 2;
//		USART1->SR &=0xdf;
//		LED0 = !LED0;
//		EIE3810_TFTLCD_ShowChar2412(400, 400, serialReceive, YELLOW, GREEN);
//		printString(100, 600, "Helloi ajdfadf", YELLOW, GREEN);
//		LCD_ShowxNum(50, 700, serialReceive, 16, 15, 20);
//		while(xxx++ < 2000000);
//	}
//	LED1 = !LED1;
//}
#endif										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置 
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQn,0);//组2，最低优先级 
}
