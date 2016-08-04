
#include "stc15f2k60s2.h"
#include "GPIO.h"
#include "base_type.h"
#include <intrins.h>
#include "TDH6300.h"
#include "uart.h"


#define EnableAllINT()		{EA = 1;}

/*****************软件延时程序**********************/
static void _Delay1us(void)		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 4;
	while (--i);
}

static void _Delay1ms(void)		//@11.0592MHz
{
	unsigned char i, j;

	i = 27;
	j = 64;
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
/****************************************************************/

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

/*******************EV1527***********************/

void _Ev1527Init(void);
static void __EV1527Reset(void);
void _EV1527Send4Bit(BYTE BitDat);
void _EV1527SendByte(BYTE _Byte);



void _Ev1527Init(void)
{
	__EV1527Reset();
	P2M1 = 0x00;//EV1527输出为强推挽输出
	P2M0 = 0x0f;
}

static void __EV1527Reset(void)
{
	EV1527_K0 = 0;
	EV1527_K1 = 0;
	EV1527_K2 = 0;
	EV1527_K3 = 0;
}

void _EV1527Send4Bit(BYTE BitDat)
{
	LED_TX = 0;//数据发送灯点亮
	_Delayms(500);
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
/*******************Sleep*********************/
/*
static void _Sleep(void)
{
	PCON = 0x02;
}

static void _SleepInit(void)
{
	WKTCL = 49;//设置唤醒周期为488us*(49+1)=24.4ms
	WKTCH = 0x80;//使能掉电唤醒定时器
}
*/

void main()
{
	//char xdata print[] = "Will Come To STC \n\r";
	//_TDH6300Open();
	_UartOpen();
	_Ev1527Init();
	EnableAllINT();
	LED_TX = 1;
	LED_RX = 1;
	//_UartPutStr("Will Come To STC \n\r");
	
	while(1)
	{
		_IPCCmdRecv();
		_TDH6300Scan();
		
	}
}
