#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	  
//�������˿ڶ���
#define BEEP PBout(8)	// BEEP,�������ӿ�		   

void BEEP_Init(void);	//��������ʼ������	


 #define  proport  	10000 //Tclk/(psc+1)=72000000/(7199+1)
 #define  L1       ((proport/131)-1)//�͵���do �����ڸ���Tout= ((arr+1)*(psc+1))/Tclk�Ƴ�arrֵ���Ǳ���define�����ֵ��ToutΪ����Ƶ��131Hz�ĵ�����Tclk=72MHz
 #define  L2       ((proport/147)-1)//�͵���re ������
 #define  L3       ((proport/165)-1)//�͵���mi ������
 #define  L4       ((proport/176)-1)//�͵���fa ������
 #define  L5       ((proport/196)-1)//�͵���sol������
 #define  L6       ((proport/220)-1)//�͵���la ������
 #define  L7       ((proport/247)-1)//�͵���si ������

 #define  M1       ((proport/262)-1)//�е���do ������
 #define  M2       ((proport/296)-1)//�е���re ������
 #define  M3       ((proport/330)-1)//�е���mi ������ 
 #define  M4       ((proport/349)-1)//�е���fa ������
 #define  M5       ((proport/392)-1)//�е���sol������
 #define  M6       ((proport/440)-1)//�е���la ������
 #define  M7       ((proport/494)-1)//�е���si ������

 #define  H1       ((proport/523)-1)//�ߵ���do ������
 #define  H2       ((proport/587)-1)//�ߵ���re ������
 #define  H3       ((proport/659)-1)//�ߵ���mi ������
 #define  H4       ((proport/699)-1)//�ߵ���fa ������
 #define  H5       ((proport/784)-1)//�ߵ���sol������
 #define  H6       ((proport/880)-1)//�ߵ���la ������
 #define  H7       ((proport/988)-1)//�ߵ���si ������


//  int music[];

// int length;

#endif

















