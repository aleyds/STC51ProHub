#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include "TDH6300.h"
#include <stdarg.h>

static xdata H_U8  g_TDH6300HaveData;
extern void _delay(unsigned long ms);

static void ReadRFSignal(BYTE *_pDat);


BYTE _TDH6300ReadByte(void);

void _TDH6300Open(void)
{
	IT0 = 1;
	EX0 = 1;
}

void _TDH6300Scan(void)
{
	BYTE RecvData[3] = {0};
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		ReadRFSignal(RecvData);
		_UartPutDec(0x11);
		_UartPutDec( RecvData[0]);
		_UartPutDec( RecvData[1]);
		_UartPutDec( RecvData[2]);
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

//接收24字节数据
static void ReadRFSignal(BYTE *_pDat)
{
	BYTE i = 0;
	for(i = 0; i < 3; i++)
	{
		_pDat[i] = _TDH6300ReadByte();
	}
}

BYTE _TDH6300ReadByte(void)
{
	BYTE i = 0;
	BYTE _ReadByte = 0;
	
	_ReadByte = P1&0xf;
	
	_ReadByte |= P1&0xf << 4;
	return _ReadByte;
}

