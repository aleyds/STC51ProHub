#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include <stdarg.h>

static xdata H_U8  g_TDH6300HaveData;

void _TDH6300Open(void)
{
	IT0 = 1;
	EX0 = 1;
}

void Int0IRQ_Handler() interrupt 0x0
{	
	//char xdata print[50] = "TDH6300 VT\n\r";

	_UartPutStr("INT0 IRQ \n\r");
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}

BYTE _TDH6300ReadByte(void)
{
	BYTE i = 0;
	BYTE _ReadByte = 0;
	
	for(i = 0; i < 2; i++)
	{
		//_ReadByte = P1&0xf;
		_ReadByte = P1&0xf << 4;
	}
	return 0;
}

