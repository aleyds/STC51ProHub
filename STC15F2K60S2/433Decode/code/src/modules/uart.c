#include "GPIO.h"
#include "base_type.h"
#include <stdarg.h>
#include "uart.h"

static H_U8 g_busy = 0;
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
	ES = 1;

}

void IntUartIRQ_Handler() interrupt 4 using 1
{
	if(RI)
	{
		RI = 0;
		__UartSend(SBUF);
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


static void __UartPutStr(const H_U8 *str)
{
	while(*str != '\0')
	{
		__UartSend(*str++);
	}
}

static void __UartPutDec(H_U32 dec)
{

	H_U8 buffer[12] = { 0 };
	sprintf(buffer, "%d", dec);
	__UartPutStr(buffer);
}



static void __UartPutBin(H_U32 bin)
{

	H_U8 buffer[12] = { 0 };
	sprintf(buffer, "0x%B", bin);
	__UartPutStr(buffer);
}

static void __UartPutHex(H_U32 hex)
{
	H_U8 buffer[12] = { 0 };
	sprintf(buffer, "0x%x", hex);
	__UartPutStr(buffer);
}

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
				case 'b':
				case 'B':
					vargint = va_arg(vp, H_U32);
					DATAZERO(vargint);
					__UartPutBin(vargint);
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


