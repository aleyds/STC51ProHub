#include "GPIO.h"
#include "base_type.h"
#include "uart.h"
#include "TDH6300.h"
#include "IPCCmd.h"
#include <stdarg.h>

#define TDH6300_RECV_TIMES			(10)  //接收10次每次4Bit  总共接收5Byte数据
#define TDH6300_DATA_LEN			(5)  //5Byte数据
#define TDH6300_TIMEOUT				(10)
#define SYNC_BYTE					(0xAA55)
#define READ_6300_DELAY				(50) //us

typedef struct _Tdh6300Recvst{
	
	 BYTE _Times;
	 BYTE _pBuff[TDH6300_DATA_LEN];
}__Tdh6300Recv_t; 
static xdata __Tdh6300Recv_t  g_Recv;
static xdata BYTE g_RecvInvalid = 0;//接收在一定时间内被无效，防止接收多次数据
static xdata BYTE g_isDefense = 0;

extern void _Delayus(unsigned long us);
extern void _Delayms(unsigned long ms);
extern void _EV1527Send4Bit(BYTE BitDat);


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
	for(i = 0; i < g_Recv._Times/2; i++ )
	{
		__UartSend( g_Recv._pBuff[i]);
	}
}

//待新协议处理接口
static void __RecvOption(void)
{
	BYTE DeviceID[3];
	BYTE StatusType;
	if((g_Recv._pBuff[0] == 0xAA &&(g_Recv._pBuff[1] == 0x55)))
	{
		DeviceID[0] = ((g_Recv._pBuff[2]>>4)&0xf) | (((g_Recv._pBuff[2])&0xf)<<4);
		DeviceID[1] = ((g_Recv._pBuff[3]>>4)&0xf) | (((g_Recv._pBuff[3])&0xf)<<4);
		DeviceID[2] = (g_Recv._pBuff[4])&0xf;
		StatusType = (g_Recv._pBuff[4]>>4)&0xf;
		__UartSend(StatusType);
		GPIO0 = (StatusType>>3)&0x01;
		GPIO1 = (StatusType>>2)&0x01;
		GPIO2 = (StatusType>>1)&0x01;
		GPIO3 = (StatusType)&0x01;
		_EV1527Send4Bit(StatusType);
	}
}

static void __RecvTest(void)
{
	//_UartSendData(g_Recv._pBuff[0]);
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
		__RecvOption();
		
		//__RecvTest();
		_ResetRecvData();
		g_RecvInvalid = 1;//接收一次正确数据后,一定时间内不再接收其他数据
		_Delayms(400);
		g_RecvInvalid = 0;
	}
	
}


void _TDH6300Scan(void)
{
	BYTE _RecvTmp = 0;
	if(TDH6300_VT&0x01) //VT为高电平，说明RF_IN有数据
	{
		LED_RX = 0; //点亮接收信号灯
		_RecvTmp = P1&0xf;
		_TDH6300Recv(_RecvTmp);
		_Delayus(READ_6300_DELAY);
		
	}else  //没有数据时VT为低电平
	{
		LED_RX = 1;//关闭接收信号灯
	}
}



