#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include "TDH6300.h"
#include <stdarg.h>

static xdata H_U8  g_TDH6300HaveData;
extern void _delay(unsigned long ms);

BYTE _TDH6300ReadByte(void);

void _TDH6300Open(void)
{
	IT0 = 1;
	EX0 = 1;
}

void _TDH6300Scan(void)
{
	BYTE RecvData = 0;
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		RecvData = _TDH6300ReadByte();
		_UartPutDec( RecvData);
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}

void Int0IRQ_Handler() interrupt 0x0
{	
	//char xdata print[50] = "TDH6300 VT\n\r";

	_UartPutStr("INT0 IRQ \n\r");
	
}

BYTE _TDH6300ReadByte(void)
{
	BYTE i = 0;
	BYTE _ReadByte = 0;
	
	for(i = 0; i < 2; i++)
	{
		//_ReadByte = P1&0xf;
		_ReadByte += P1&0xf << 4*i;
	}
	return _ReadByte;
}

