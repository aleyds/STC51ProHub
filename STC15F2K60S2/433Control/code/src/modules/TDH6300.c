#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include "TDH6300.h"
#include "EV1527.h"
#include <stdarg.h>

#define TDH6300_RECV_TIMES			(1)
#define TDH6300_DATA_LEN			(1)
typedef struct _Tdh6300Recvst{
	
	BYTE _Times;
	BYTE _pBuff[TDH6300_DATA_LEN];
}__Tdh6300Recv_t; 
static xdata __Tdh6300Recv_t  g_Recv;
static xdata BYTE g_RecvInvalid = 0;//接收在一定时间内被无效，防止接收多次数据
extern void _delay(unsigned long ms);

static _ExternalControl(const BYTE *_pBuf);

BYTE _TDH6300ReadByte(void);

void _TDH6300Open(void)
{
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
		__DebugRecv();
		_ExternalControl(g_Recv._pBuff);
		_ResetRecvData();
		g_RecvInvalid = 1;//接收一次正确数据后,一定时间内不再接收其他数据
		//__StartTimer();
		_delay(400);
		g_RecvInvalid = 0;
	}
	
}



void _TDH6300Scan(void)
{
	///BYTE RecvData[3] = {0};
	BYTE _RecvTmp = 0;
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		//ReadRFSignal(RecvData);
		_RecvTmp = P1&0xf;
		
		_TDH6300Recv(_RecvTmp);
		
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}

static _ExternalControl(const BYTE *_pBuf)
{
	unsigned long _DeviceID = ( _pBuf[0]<<12 )|(_pBuf[1] << 4)|(_pBuf[2]>>4)&0xf;
	BYTE Control = (_pBuf[2]&0xF);
	GPIO0 = Control&0x1;
	GPIO0 = (Control>>1)&0x1;
	GPIO0 = (Control>>2)&0x1;
	GPIO0 = (Control>>3)&0x1;
	_EV1527SendData(_pBuf, 3);
}

void Int0IRQ_Handler() interrupt 0x0
{	
	//char xdata print[50] = "TDH6300 VT\n\r";

	_UartPutStr("INT0 IRQ \n\r");
	
}


