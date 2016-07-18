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
	//AUXR1 |= (0x2<<2);//将串口1切换到P1.6 RX  P1.7 TX默认在  P3.0 RX  P3.1 TX,调试采用默认，实际需要切换
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
		if(_VerifyData(g_RecvBuff, (g_RecvBuff[2]+2)) == g_RecvBuff[g_RecvBuff[2]+2])
		{
			g_RecvOver = 1;
			
		}
		
	}
}

void _CommandData(void)
{
	if(g_RecvOver)
	{
		switch(g_RecvBuff[3])
		{
			case 0:
				_UartPutStr("Study \n\r");
				__TDH6300Learn();
				break;
			case 1:
				_UartPutStr("Clear \n\r");
				__TDH6300Clear();
			    break;
			 case 2:
			 	_UartPutStr("EV1527 \n\r");
			 	_EV1527Test();
			 	break;
			default:
				_UartPutStr("Other: \n\r");
				_UartPutDec(g_RecvBuff[3]);
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

static void __UartSend(H_U8 _ch)
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

/*
void _UartPrintf(H_U8 *fmt, ...)
{

	H_U32 vargint = 0;
	H_U8 *vargpch = H_NULL;
	H_U8 vargch = 0;
	H_U8 *pfmt = H_NULL;

	va_list vp;
	pfmt = fmt;
	va_start(vp, fmt);
	while(*pfmt)
	{
		if(*pfmt == '%')
		{
			switch(*(++pfmt))
			{
				case 'c':
					vargch = va_arg(vp,H_U32);
					__UartSend((H_U8)vargch);
					break;
				case 'd':
				case 'i':
					vargint = va_arg(vp, H_U32);
					DATAZERO(vargint);
					__UartPutDec(vargint);
					break;
				case 's':
					vargpch = va_arg(vp, H_U8 *);
					__UartPutStr(vargpch);
					break;
				case 'x':
				case 'X':
					vargint = va_arg(vp, H_U32);
					DATAZERO(vargint);
					__UartPutHex(vargint);
					break;
				case '%':
					__UartSend('%');
					break;
			}
			pfmt++;
		}
		else
		{
			__UartSend(*pfmt++);
		}
	}
	va_end(vp);

}
*/

