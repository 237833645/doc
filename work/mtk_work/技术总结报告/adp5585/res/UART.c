/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Author: PU20 Department. 
//  Tel: 03-5770066#8043
//  E-Mail: MicroC-8bit@nuvoton.com
//***********************************************************************************************************
//  Application : UART Function
//  RXD => P1.1 ; TXD => P1.0 (default)
//
//  Output : Uart receive a byte and transmit the same byte to PC
//***********************************************************************************************************

//------------------------- <<< Use Configuration Wizard in Context Menu >>> --------------------------------
// <h> UART pin Select
//     <o0.6> Uart pin
//          <0=> Select P1.0, P1.1 as UART pin(default)
// </h>
//-------------------------------- <<< end of configuration section >>> -------------------------------------

#define Uart_Port_Sel   0x00

#include <stdio.h>
#include "N79E81x.h"
#include "Typedef.h"
#include "Define.h"
#include "Common.h"
#include "Delay.h"
#include<intrins.h>

UINT8 u8Uart_Data,u8Uart_Data1,u8Uart_Data2,temp1,temp2,temp3;

UINT32 u32Count;



void delay()
{ ;; }
void start()  //开始信号
{	
	P13=1;				//P13==SDA
	_nop_();			
	P12=1;			    //P12==SCL
	_nop_();
	P13=0;
	_nop_();
}

void stop()   //停止
{
	P13=0;			 //P13==SDA]
	_nop_();
	P12=1;			 //P12==SCL
	_nop_();
	P13=1;
	_nop_();
}

void respons()  //应答
{
	UINT8 i=0;
	P12=1;			   //P12==SCL
	_nop_();
	while((P13==1)&&(i<250))i++;	   //&&(i<250))i++
	P12=0;			   //P12==SCL
	_nop_();
}

void init()
{
	P13=1;
	P12=1;
}

void write_byte(UINT8 date)
{
	UINT8 i,temp;
	temp=date;

	for(i=0;i<8;i++)
	{
		temp=temp<<1;
		P12=0;	  // P12==SCL
	    _nop_();
		P13=CY;	  //P13==SDA
		_nop_();
		P12=1;
		_nop_();
	}
	P12=0;
	P13=1;
}

UINT8 read_byte()
{
	UINT8 i,k;
	P12=0;
	P13=1;
	for(i=0;i<8;i++)
	{
		P12=1;
		_nop_();			
		k=(k<<1)|P13;
		P12=0;
		_nop_();	
	}
	return k;
}

void delay1(UINT8 x)
{
	UINT8 a,b;
	for(a=x;a>0;a--)
	 for(b=200;b>0;b--);
}

void write_add(UINT8 address,UINT8 date)
{
	start();
	write_byte(0x60);
	respons();
	write_byte(address);
	respons();
	write_byte(date);
	respons();
	stop();
}

UINT8 read_add(UINT8 address)
{
	UINT8 date;
	start();
	write_byte(0x60);
	respons();
	write_byte(address);
	respons();
	start();
	write_byte(0x61);
	respons();
	date=read_byte();
	stop();
	return date;
}

void main()
{
	AUXR1 |= Uart_Port_Sel;             // Select P10/P11 as UART pin(default)                                  
    ES = 1;                             // Enable serial interrupt  
    EA = 1;                             // Enable global interrupt
    InitialUART0_Timer1(9600);          // 9600 Baud Rate @ 11.0592MHz 	
	delay1(100);
    u8Uart_Data=0;

    init();
	start();
	write_byte(0xa0);
	respons();
	write_byte(0x03);
	respons();
	write_byte(0xaa);
	respons();
	_nop_();
	stop();
    Delay1ms(10);

	init();
	start();
	write_byte(0xa0);
	respons();
	write_byte(0x03);
	respons();
	start();
	respons();
	write_byte(0xa1);
	respons();
//	SBUF=read_byte();
//	while(TI==0);
//	TI=0;
	_nop_();
	stop();
    Delay1ms(10);

	SBUF=0x0d;
	while(TI==0);
	TI=0;
	SBUF=0x0a;
	while(TI==0);
	TI=0;

	u8Uart_Data=read_add(0x00);			   //read ID number
//	SBUF=read_add(0x00);
//	while(TI==0);
//	TI=0;
	if(0x00<u8Uart_Data<0xff)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

    write_add(0x37,0x01);				 //set scan time   20ms
	Delay1ms(10);
	u8Uart_Data=read_add(0x37);
//	SBUF=read_add(0x37);
//	while(TI==0);
//	TI=0;
	if(u8Uart_Data==0x01)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';					 
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

    write_add(0x38,0x1f);			   //setR4,R3,R2,R1,R0
	Delay1ms(10);
	u8Uart_Data=read_add(0x38);
//	SBUF=read_add(0x38);
//	while(TI==0);
//	TI=0;
	if(u8Uart_Data==0x1f)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

    write_add(0x39,0x1f);				 //setC4,C3,C2,C1,C0
	Delay1ms(10);
	u8Uart_Data=read_add(0x39);
//	SBUF=read_add(0x39);
//	while(TI==0);
//	TI=0;
	if(u8Uart_Data==0x1f)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

    write_add(0x3b,0xa2);				 //set OSC AND INT VALUE 10100000 
	Delay1ms(10);
	u8Uart_Data=read_add(0x3b);
//	SBUF=read_add(0x3b);
//	while(TI==0);
//	TI=0;
	if(u8Uart_Data==0xa2)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

    write_add(0x3c,0x05);				 //set event_int	00000101
	Delay1ms(10);
	u8Uart_Data=read_add(0x3c);
//	SBUF=read_add(0x3c);
//	while(TI==0);
//	TI=0;
	if(u8Uart_Data==0x05)
	{
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	}
	else
	{
	  SBUF='E';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='r';
	  while(TI==0);
	  TI=0;
	}
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;

	while(1)
	{
    if(P31==0)
	{
//	 P04=0;
	 u8Uart_Data1=read_add(0x02);
//	 SBUF=read_add(0x02);			 //发送EC计数
//	 while(TI==0);
//	 TI=0;
	 u8Uart_Data2=read_add(0x03);
/*	 temp1=0;
	 temp1=u8Uart_Data2&0x80;
	 if(temp1==0x80)
	 {
	 SBUF='P';				//发送按键还是松开
	 while(TI==0);
	 TI=0;
	 }
	 else
	 {
	 SBUF='R';				
	 while(TI==0);
	 TI=0;	  
	 }
	  SBUF=0x0d;
	  while(TI==0);
	  TI=0;
	  SBUF=0x0a;
	  while(TI==0);
	  TI=0;				   */

	 temp1=0;
	 temp1=u8Uart_Data2&0x7f;
//	 SBUF=temp1;			  //发送按键识别码
//	 while(TI==0);
//	 TI=0;
	 switch(temp1)
	 {
	  case 24:				 //发送按键值
	  SBUF='D';
	  while(TI==0);
	  TI=0;
	  SBUF='a';
	  while(TI==0);
	  TI=0;
	  SBUF='i';
	  while(TI==0);
	  TI=0;
	  SBUF='l';
	  while(TI==0);
	  TI=0;
	  break;

	  case 22:				 //发送按键值
	  SBUF='S';
	  while(TI==0);
	  TI=0;
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='S';
	  while(TI==0);
	  TI=0;
	  break;

	  case 14:				 //发送按键值
	  SBUF='1';
	  while(TI==0);
	  TI=0;
	  break;
	  case 9:				 //发送按键值
	  SBUF='4';
	  while(TI==0);
	  TI=0;
	  break;
	  case 4:				 //发送按键值
	  SBUF='7';
	  while(TI==0);
	  TI=0;
	  break;
	  case 19:				 //发送按键值
	  SBUF='*';
	  while(TI==0);
	  TI=0;
	  break;
	  case 12:				 //发送按键值
	  SBUF='2';
	  while(TI==0);
	  TI=0;
	  break;
	  case 7:				 //发送按键值
	  SBUF='5';
	  while(TI==0);
	  TI=0;
	  break;
	  case 2:				 //发送按键值
	  SBUF='8';
	  while(TI==0);
	  TI=0;
	  break;
	  case 17:				 //发送按键值
	  SBUF='0';
	  while(TI==0);
	  TI=0;
	  break;
	  case 23:				 //发送按键值
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='F';
	  while(TI==0);
	  TI=0;
	  SBUF='F';
	  while(TI==0);
	  TI=0;
	  break;
	  case 13:				 //发送按键值
	  SBUF='3';
	  while(TI==0);
	  TI=0;
	  break;
	  case 8:				 //发送按键值
	  SBUF='6';
	  while(TI==0);
	  TI=0;
	  break;
	  case 3:				 //发送按键值
	  SBUF='9';
	  while(TI==0);
	  TI=0;
	  break;
	  case 18:				 //发送按键值
	  SBUF='#';
	  while(TI==0);
	  TI=0;
	  break;
	  case 25:				 //发送按键值
	  SBUF='U';
	  while(TI==0);
	  TI=0;
	  SBUF='P';
	  while(TI==0);
	  TI=0;
	  break;
	  case 15:				 //发送按键值
	  SBUF='B';
	  while(TI==0);
	  TI=0;
	  SBUF='a';
	  while(TI==0);
	  TI=0;
	  SBUF='c';
	  while(TI==0);
	  TI=0;
	  SBUF='k';
	  while(TI==0);
	  TI=0;
	  break;

	  case 10:				 //发送按键值
	  SBUF='L';
	  while(TI==0);
	  TI=0;
	  SBUF='e';
	  while(TI==0);
	  TI=0;
	  SBUF='f';
	  while(TI==0);
	  TI=0;
	  SBUF='t';
	  while(TI==0);
	  TI=0;
	  break;
	  case 6:				 //发送按键值
	  SBUF='D';
	  while(TI==0);
	  TI=0;
	  SBUF='o';
	  while(TI==0);
	  TI=0;
	  SBUF='w';
	  while(TI==0);
	  TI=0;
	  SBUF='n';
	  while(TI==0);
	  TI=0;
	  break;
	  case 5:				 //发送按键值
	  SBUF='M';
	  while(TI==0);
	  TI=0;
	  SBUF='e';
	  while(TI==0);
	  TI=0;
	  SBUF='u';
	  while(TI==0);
	  TI=0;
	  SBUF='n';
	  while(TI==0);
	  TI=0;
	  break;
	  case 16:				 //发送按键值
	  SBUF='O';
	  while(TI==0);
	  TI=0;
	  SBUF='K';
	  while(TI==0);
	  TI=0;
	  break;
	  case 20:				 //发送按键值
	  SBUF='R';
	  while(TI==0);
	  TI=0;
	  SBUF='i';
	  while(TI==0);
	  TI=0;
	  SBUF='g';
	  while(TI==0);
	  TI=0;
	  SBUF='t';
	  while(TI==0);
	  TI=0;
	  default:
	  break;
	 }	 	  
	}
	else
	{
	 P04=1;
	}
   Delay1ms(200);
  }
}

void UART_ISR(void) interrupt 4 
{
    if (RI == 1) 
    {                                   // If reception occur 
        RI = 0;                         // Clear reception flag for next reception
        u8Uart_Data = SBUF;             // Read receive data 
        SBUF = u8Uart_Data;             // Send back same data on uart
    }
    else TI = 0;                        // If emission occur 
                                        // Clear emission flag for next emission     
}
