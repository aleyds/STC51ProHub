#ifndef __GPIO_H__
#define __GPIO_H__
#include "stc15f2k60s2.h"
//LED
sibt  LED_TX=P2^6;  //��������ָʾ��
sbit  LED_RX=P2^7;  //��������ָʾ��

//TDH6300
sbit TDH6300_D0=P1^0;
sbit TDH6300_D1=P1^1;
sbit TDH6300_D2=P1^2;
sbit TDH6300_D3=P1^3;
sbit TDH6300_VT=P3^2;//��Ƶ������RF_IN���յ��ź�ʱ��  VT����ߵ�ƽ  D0-D3�����Ӧ���źŸ���Ƭ��

//EV1527
sbit EV1527_K0=P2^0;
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