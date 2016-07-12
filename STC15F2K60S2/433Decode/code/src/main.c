
#include "stc15f2k60s2.h"
#include "GPIO.h"
#include "base_type.h"
#include "TDH6300.h"
#include "uart.h"

#define EnableAllINT()		{EA = 1;}


void main()
{
	_TDH6300Open();
	_UartOpen();
	EnableAllINT();
	LED_TX = 0;
	LED_RX = 0;

	_UartPrintf("Will Come To\n\r");
	while(1);
}