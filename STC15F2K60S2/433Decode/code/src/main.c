
#include "stc15f2k60s2.h"
#include "GPIO.h"
#include "base_type.h"
#include <intrins.h>
#include "TDH6300.h"
#include "EV1527.h"
#include "uart.h"


#define EnableAllINT()		{EA = 1;}

static void _Delay1us(void)		//@11.0592MHz
{
	_nop_();
	_nop_();
	_nop_();
}

static void _Delay1ms(void)		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void _Delayus(unsigned long us)
{
	while(us--)
	{
		_Delay1us();
	}
}

void _Delayms(unsigned long ms)
{
	while(ms--)
	{
		_Delay1ms();
	}
}

/*
 void _delay(unsigned long ms)
{
	unsigned long i = 0;
	unsigned long j = 0;
	for(i = ms; i> 0; i--)
	{
		for(j = 110; j > 0; j--)
		{
			
		}
	}

}
*/

BYTE _VerifyData(BYTE *_Dat, BYTE len)
{
	BYTE i = 0;
	BYTE RetCode = 0;
	for( i = 0; i < len ; i++)
	{
		RetCode ^= _Dat[i];
	}
	return RetCode;
}

void main()
{
	//char xdata print[] = "Will Come To STC \n\r";
	//_TDH6300Open();
	_Ev1527Open();
	_UartOpen();
	EnableAllINT();
	LED_TX = 1;
	LED_RX = 1;
	_UartPutStr("Will Come To STC \n\r");
	
	while(1)
	{
		_IPCCmdRecv();
		_TDH6300Scan();
		/*
		if(TDH6300_VT&0x01)
		{
			_UartPutStr("111 \n\r");
		}
		else
		{
			//_UartPutStr("222 \n\r");
		}*/
	}
}