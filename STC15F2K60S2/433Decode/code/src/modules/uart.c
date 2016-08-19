#include "GPIO.h"
#include "base_type.h"
#include <stdarg.h>
#include "IPCCmd.h"
#include "uart.h"

static  bit g_busy = 0;
static  bit g_RecvOver = 0;
static xdata H_U8 g_RecvBuff[8] = { 0 };
static xdata H_U8 g_RecvIndex = 0;

extern BYTE _VerifyData(BYTE *_Dat, BYTE len);
static void __UartSend(H_U8 _ch);
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

#define DATAZERO(data)\
do{\
	if(data == 0)\
	{\
		__UartSend((H_U8)('0'));\
		break;\
	}\
}while(0)

void _UartOpen(void)
{
#if (PARITYBIT == NONE_PARITY)
	SCON=0x50;
#elif ((PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY))
	SCON=0xda;
#elif (PARITYBIT == SPACE_PARITY)
	SCON=0xd2;
#endif
	T2L = (65536 - (SOC_FREQ/4/UART_BAUD));
	T2H = (65536 - (SOC_FREQ/4/UART_BAUD))>>8;
	AUXR = 0x14;
	AUXR |= 0x01;
	//将串口1切换到P1.6 RX  P1.7 TX默认在  P3.0 RX  P3.1 TX,调试采用默认，实际需要切换
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
	ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
	P_SW1 = ACC;  
	ES = 1;
	g_RecvIndex = 0;
}

static void __ClearBuffer(void)
{
	memset(g_RecvBuff, 0, sizeof(g_RecvBuff));
	g_RecvIndex = 0;
	g_RecvOver = 0;
}

static void _RecvUart(H_U8 Byte)
{
	if(g_RecvOver) //上一次接收的未处理完不接受新数据 
	{
		//__UartSend(0x11);
		return;
	}
	g_RecvBuff[g_RecvIndex] = Byte;
	if((g_RecvIndex == 0)&&(g_RecvBuff[0] != 0xAA))
	{
		__ClearBuffer();
	}else if((g_RecvIndex == 1)&&(g_RecvBuff[1] != 0x55))
	{
		__ClearBuffer();
	}
	g_RecvIndex++;
	if((g_RecvIndex) >= (g_RecvBuff[2]+3))
	{
		//if(_VerifyData(g_RecvBuff, (g_RecvBuff[2]+2)) == g_RecvBuff[g_RecvBuff[2]+2])
		{
			g_RecvOver = 1;
			
		}
		
	}
}

static void __IpcStuParse(_IpcCmd_t *_pIpcCmd)
{
	_pIpcCmd->len = g_RecvBuff[2];
	_pIpcCmd->type = g_RecvBuff[3];
	memcpy(_pIpcCmd->device_id, &(g_RecvBuff[4]),3);
	if((_pIpcCmd->len > 5) && (_pIpcCmd->len < 7))
	{
		memcpy(_pIpcCmd->_pdata, &(g_RecvBuff[7]),2);
	}
}

void _IPCCmdRecv(void)
{
	_IpcCmd_t _stIpcCmd;
	if(g_RecvOver)
	{
		memset(&_stIpcCmd, 0, sizeof(_IpcCmd_t));
		__IpcStuParse(&_stIpcCmd);
		switch(_stIpcCmd.type)
		{
			case _TYPE_6300_LEARN:
				_UartPutStr("Study \n\r");
				__TDH6300Learn();
				break;
			case _TYPE_6300_CLEAR:
				_UartPutStr("Clear \n\r");
				__TDH6300Clear();
			    break;
			 case _TYPE_1527_TEST:
			 	_UartPutStr("EV1527 \n\r");
			 	_EV1527Test();
			 	break;
			 case _TYPE_1527_CONTROL:
			 	_EV1527Control(_stIpcCmd.device_id,_stIpcCmd._pdata[0]);
			 	break;
			default:
				_UartPutStr("Other: \n\r");
				_UartPutDec(_stIpcCmd.type);
				break;
		}
		__ClearBuffer();
	}
	
}

void IntUartIRQ_Handler(void) interrupt 4 
{
	H_U8 tmp = 0;
	if(RI)
	{
		RI = 0;
		tmp = SBUF;
		_RecvUart(tmp);
		//SBUF = tmp;
	}
	if(TI)
	{
		TI = 0;
		g_busy = 0;
	}
}

 void __UartSend(H_U8 _ch)
{
	while(g_busy);
	ACC = _ch;
	if(P)
	{
#if (PARITYBIT == ODD_PARITY)
	TB8 = 0;
#elif (PARITYBIT == EVEN_PARITY)
	TB8 = 1;
#endif
	}
	else 
	{
#if (PARITYBIT == ODD_PARITY)
	TB8 = 1;
#elif (PARITYBIT == EVEN_PARITY)
	TB8 = 0;
#endif	
	}
	g_busy = 1;
	SBUF = ACC;

}


void _UartSendData(const BYTE *_pDat, BYTE Len)
{
	BYTE i = 0;
	for(i = 0; i < Len; i++)
	{
		__UartSend(*(_pDat + i ));
	}
	
}

void _UartPutStr(const H_U8 *str)
{
	while(*str != '\0')
	{
		__UartSend(*str++);
	}
}

 void _UartPutDec(H_U32 dec)
{

	H_U8 buffer[12] = { 0 };
	sprintf(buffer, "%d", dec);
	_UartPutStr(buffer);
}

static void __UartPutHex(H_U32 hex)
{
	H_U8 buffer[12] = { 0 };
	sprintf(buffer, "0x%x", hex);
	_UartPutStr(buffer);
}



