#ifndef __GPIO_H__
#define __GPIO_H__
#include "stc15f2k60s2.h"
//LED
sbit  LED_TX=P3^3;  //发送数据指示灯
sbit  LED_RX=P5^5;  //接收数据指示灯

//TDH6300译码器
sbit TDH6300_D0=P1^0; //译码输出
sbit TDH6300_D1=P1^1;
sbit TDH6300_D2=P1^2;
sbit TDH6300_D3=P1^3;
sbit TDH6300_VT=P3^2;//高频接收器RF_IN接收到信号时候  VT输出高电平  D0-D3输出相应的信号给单片机

//EV1527 编码器
sbit EV1527_K0=P2^0;  //编码输入
sbit EV1527_K1=P2^1;
sbit EV1527_K2=P2^2;
sbit EV1527_K3=P2^3;

//GPIO
sbit GPIO0=P3^4;
sbit GPIO1=P3^5;
sbit GPIO2=P3^6;
sbit GPIO3=P3^7;

sbit LEARNKEY=P2^4;
sbit RXD=P1^6;
sbit TXD=P1^7;


#endif