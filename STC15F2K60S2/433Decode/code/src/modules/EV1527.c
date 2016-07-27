#include "GPIO.h"
#include "base_type.h"
#include "EV1527.h"
#include <stdarg.h>

extern void _Delayus(unsigned long us);
extern void _Delayms(unsigned long ms);

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
	P2M1 = 0x00;
	P2M0 = 0x0f;
}

void _EV1527Send4Bit(BYTE BitDat)
{
	LED_TX = 0;//数据发送灯点亮
	_Delayms(1000);
	EV1527_K0 = (BitDat >> 3)&0x01;
	EV1527_K1 = (BitDat >> 2)&0x01;
	EV1527_K2 = (BitDat >> 1)&0x01;
	EV1527_K3 = (BitDat >> 0)&0x01;

	LED_TX=1;//数据发送灯熄灭

	_Delayms(200);
	__EV1527Reset();

}

void _EV1527SendByte(BYTE _Byte)
{
	BYTE i = 0;

	_EV1527Send4Bit((_Byte>>4)&0xF);

	_EV1527Send4Bit((_Byte)&0xF);
	
}

void _EV1527SendData(BYTE *_pDat, BYTE len)
{
	BYTE i = 0;
	LED_TX = 0;//数据发送灯点亮
	_Delayms(1000);
	
	for(i = 0; i < len; i++)
	{
		_EV1527SendByte(*(_pDat+i));
	}
	LED_TX=1;//数据发送灯熄灭
	_Delayms(100);
	__EV1527Reset();
}