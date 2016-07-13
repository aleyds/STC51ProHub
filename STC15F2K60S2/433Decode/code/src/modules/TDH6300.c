#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include <stdarg.h>

void _TDH6300Open(void)
{
	IT0 = 1;
	EX0 = 1;
}

void Int0IRQ_Handler() interrupt 0x0
{	
	//char xdata print[50] = "TDH6300 VT\n\r";

	
	if(TDH6300_VT&0x01) //VTΪ�ߵ�ƽ��˵��RF_IN������
	{
		LED_RX = 0; //���������źŵ�
	}else  //û������ʱVTΪ�͵�ƽ
	{
		LED_RX = 1;//�رս����źŵ�
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
