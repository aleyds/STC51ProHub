#include "GPIO.h"
#include "base_type.h"
#include <stdarg.h>

BYTE _TDH6300ReadByte(void)
{
	BYTE i = 0;
	BYTE _ReadByte = 0;
	for(i = 0; i < 2; i++)
	{
		_ReadByte = ((TDH6300_D0 << 3 ) | (TDH6300_D1 << 2) | (TDH6300_D2 << 1) | TDH6300_D3);
		_ReadByte = _ReadByte << 4;
	}
}

void _TDH6300Scan(void)
{
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}