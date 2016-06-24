#include "GPIO.h"
#include "base_type.h"
#include <stdarg.h>

void _EV1527SendByte(BYTE _Byte)
{
	BYTE i = 0;
	EV1527_K0 = (_Byte >> 7)&0x01;
	EV1527_K1 = (_Byte >> 6)&0x01;
	EV1527_K2 = (_Byte >> 5)&0x01;
	EV1527_K3 = (_Byte >> 4)&0x01;
	
	EV1527_K0 = (_Byte >> 3)&0x01;
	EV1527_K1 = (_Byte >> 2)&0x01;
	EV1527_K2 = (_Byte >> 1)&0x01;
	EV1527_K3 = (_Byte >> 0)&0x01;
}

void _EV1527SendData(BYTE *_pDat, BYTE len)
{
	BYTE i = 0;
	LED_TX = 0;//���ݷ��͵Ƶ���
	for(i = 0; i < len; i++)
	{
		_EV1527SendByte(*(_pDat+i))
	}
	LED_TX=1;//���ݷ��͵�Ϩ��
}