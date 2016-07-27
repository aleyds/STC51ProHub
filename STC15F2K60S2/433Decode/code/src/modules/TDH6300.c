#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include "TDH6300.h"
#include "EV1527.h"
#include "timer.h"
#include "IPCCmd.h"
#include <stdarg.h>

#define TDH6300_RECV_TIMES			(20)
#define TDH6300_DATA_LEN			(10)
#define TDH6300_TIMEOUT				(10)
#define SYNC_BYTE					(0xAA55)
#define READ_6300_DELAY				(90) //us

typedef struct _Tdh6300Recvst{
	
	 BYTE _Times;
	 BYTE _pBuff[TDH6300_DATA_LEN];
}__Tdh6300Recv_t; 
static xdata __Tdh6300Recv_t  g_Recv;
static xdata BYTE g_RecvInvalid = 0;//接收在一定时间内被无效，防止接收多次数据

extern void _Delayus(unsigned long us);
extern void _Delayms(unsigned long ms);


//static void ReadRFSignal(BYTE *_pDat);
static void __StartTimer(void);


BYTE _TDH6300ReadByte(void);

void _TDH6300Open(void)
{
	//TDH6300_VT = 0;
	//IT0 = 0;
	//EX0 = 1;
	memset(&g_Recv, 0, sizeof(__Tdh6300Recv_t));
}

static void _ResetRecvData(void)
{
	memset(&g_Recv, 0, sizeof(__Tdh6300Recv_t));
}


static void __DebugRecv(void)
{
	BYTE i = 0;
	for(i = 0; i < g_Recv._Times; i++ )
	{
		_UartPutDec( g_Recv._pBuff[i]);
	}
}

//待新协议处理接口
static void __RecvOption(void)
{
	BYTE DeviceID[3];
	BYTE StatusType;
	
	if((g_Recv._pBuff[0] == (SYNC_BYTE>>8)&0xff) &&
	(g_Recv._pBuff[1] == (SYNC_BYTE)&0xff))
	{
		DeviceID[0] = (g_Recv._pBuff[2]>>4)&0xf;
		DeviceID[1] = ((g_Recv._pBuff[2])&0xf<<4)|((g_Recv._pBuff[3]>>4)&0xf);
		DeviceID[2] = ((g_Recv._pBuff[3])&0xf<<4)|((g_Recv._pBuff[4]>>4)&0xf);
		StatusType = g_Recv._pBuff[3]&0xf;
		//switch(StatusType)
		{
			_IpcBackCmd(StatusType,DeviceID, H_NULL, 0);
		}
	}
}

static void __RecvTest(void)
{
	_UartPutDec(g_Recv._pBuff[0]);
	_EV1527Send4Bit(0xf);
}

void _TDH6300Recv(BYTE _Recv4Bit)
{
	if(g_RecvInvalid)
	{
		return ;
	}
	g_Recv._pBuff[(g_Recv._Times/2)] |= (_Recv4Bit&0xf)<<(g_Recv._Times%2)*4;
	g_Recv._Times++;
	if(g_Recv._Times >= TDH6300_RECV_TIMES)//接收完一个周期的数据
	{
		//__DebugRecv();
		//__RecvOption();
		
		__RecvTest();
		_ResetRecvData();
		g_RecvInvalid = 1;//接收一次正确数据后,一定时间内不再接收其他数据
		//__StartTimer();
		_Delayms(400);
		g_RecvInvalid = 0;
	}
	
}


static void __StopTimer(void)
{
	wy_timer_close(_TIMER0);
}


static void __TDH6300TimerCall(void)
{
	_UartPutStr(" Tiemr CAll\n\r");

	g_RecvInvalid = 0;//恢复数据无效标志
	//__StopTimer();
}

static void __StartTimer(void)
{
	__StopTimer();
	//wy_timer_open(_TIMER0, TDH6300_TIMEOUT,__TDH6300TimerCall);
	
}



void _TDH6300Scan(void)
{
	BYTE _RecvTmp = 0;
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		
		_Delayus(READ_6300_DELAY);
		
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		_RecvTmp = P1&0xf;
		_UartPutDec(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		
		//_TDH6300Recv(_RecvTmp);
		//_UartPutDec( RecvData[1]);
		//_UartPutDec( RecvData[2]);
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}

/*
void Int0IRQ_Handler() interrupt 0
{	
	//char xdata print[50] = "TDH6300 VT\n\r";

	_UartPutStr("INT0 IRQ \n\r");
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		//ReadRFSignal(RecvData);
		_UartPutDec( P1&0xf);
		//_UartPutDec( RecvData[1]);
		//_UartPutDec( RecvData[2]);
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
	
}
*/

BYTE _TDH6300ReadByte(void)
{
	BYTE i = 0;
	BYTE _ReadByte = 0;
	
	_ReadByte = P1&0xf;
	
	_ReadByte |= P1&0xf << 4;
	return _ReadByte;
}

