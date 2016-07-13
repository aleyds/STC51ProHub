
#include "stc15f2k60s2.h"
#include "GPIO.h"
#include "base_type.h"
#include "TDH6300.h"
#include "uart.h"


#define EnableAllINT()		{EA = 1;}


void main()
{
	//char xdata print[] = "Will Come To STC \n\r";
	_TDH6300Open();
	_UartOpen();
	EnableAllINT();
	LED_TX = 1;
	LED_RX = 1;
	_UartPutStr("Will Come To STC \n\r");
	
	while(1)
	{
		_CommandData();
	}
}