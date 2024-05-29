#include "usart.h"	  
#include "led.h"
#include "lcd.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ�����ʺ�STM32F10xϵ�У�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2010/1/1
//�汾��V1.7
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//V1.6�޸�˵�� 20150109
//uart_init����ȥ���˿���PE�ж�
//V1.7�޸�˵�� 20150322
//�޸�OS_CRITICAL_METHOD���ж�Ϊ��SYSTEM_SUPPORT_OS
////////////////////////////////////////////////////////////////////////////////// 	  
 
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART1->DR = (u8) ch;      	//дDR,����1����������
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
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
// 		printf(" my data:%d\r\n",gameStatus); //��ӡLCD ID 
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
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬���� 
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQn,0);//��2��������ȼ� 
}
