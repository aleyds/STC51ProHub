#ifndef __GPIO_H__
#define __GPIO_H__
#include "STC15W201S.h"
//LED
sbit LED_LOW_VOLTAGE=P1^5;
sbit LED_CHANGE=P5^4;
//TDH6300
sbit TDH6300_VT = P3^3;
sbit TDH6300_JDQ = P3^2;
//Motor
sbit MOTOR_PORTA = P1^0;
sbit MOTOR_PORTB = P1^1;
//Other
sbit LOW_POWER_DECT=P3^7;
sbit CHARG_DET=P3^6;

#endif