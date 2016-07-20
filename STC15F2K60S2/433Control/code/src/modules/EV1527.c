#include "GPIO.h"
#include "base_type.h"
#include "EV1527.h"
#include <stdarg.h>

extern void _delay(unsigned long ms);

static void __EV1527Reset(void)
{
	EV1527_K0 = 0;
	EV1527_K1 = 0;
	EV1527_K2 = 0;
	EV1527_K3 = 0;
}

void _Ev1527Open(void)
{
	__EV1527Reset();
}

void _EV1527Send4Bit(BYTE BitDat)
{
	LED_TX = 0;//数据发送灯点亮
	_delay(1000);
	EV1527_K0 = (BitDat >> 3)&0x01;
	EV1527_K1 = (BitDat >> 2)&0x01;
	EV1527_K2 = (BitDat >> 1)&0x01;
	EV1527_K3 = (BitDat >> 0)&0x01;

	LED_TX=1;//数据发送灯熄灭

	__EV1527Reset();

}

void _EV1527SendByte(BYTE _Byte)
{
	BYTE i = 0;
	
	_EV1527Send4Bit((_Byte>>4));
	
	_EV1527Send4Bit(_Byte);

}

void _EV1527SendData(BYTE *_pDat, BYTE len)
{
	BYTE i = 0;
	for(i = 0; i < len; i++)
	{
		_EV1527SendByte(*(_pDat+i));
	}

	__EV1527Reset();
}