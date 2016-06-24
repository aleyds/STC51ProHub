#include "GPIO.h"
#include "base_type.h"
#include "intrins.h"
#include "IPCCmd.h"


bit busy;
void _IpcOpen(void)
{
	TMOD = TMOD | 0x20;
	TH1=0xFD; //������Ϊ9600
	TL1=0xFD;
	PCON=0x00;//�����ʲ�����
#if (PARITY_TYPE == PARITY_NONE)
	SCON = 0x50;
#elif ((PARITY_TYPE == PARITY_ODD) || (PARITY_TYPE == PARITY_EVEN) || (PARITY_TYPE == PARITY_MARK))
	SCON = 0xda;
#elif (PARITY_TYPE == PARITY_SPACE)
	SCON = 0xd2
#endif
	TR1=1;
	AUXR = 0x14;
	AUXR |= 0x01;
	AUXR1 |= 0x40; //�л�����1��P1.6 P1.7��  ʹ���ڲ�ʱ��
	ES = 1;
	EA = 1;
}

static void __SendByte(BYTE dat)
{
	while(busy);
	ACC = dat;
	if(P)
	{
#if (PARITY_TYPE == PARITY_ODD)
		TB8 = 0;
#elif (PARITY_TYPE == PARITY_EVEN)
		TB8 = 1;
#endif
	}
	else 
	{
#if (PARITY_TYPE == PARITY_ODD)
		TB8 = 1;
#elif (PARITY_TYPE == PARITY_EVEN)
		TB8 = 0;
#endif		
	}
	busy = 1;
	SBUF = ACC;
}

void _SendData(BYTE *pDat, BYTE len)
{
	BYTE i = 0;
	for(i = 0; i < len; i++)
	{
		__SendByte(*(pDat+i));
	}
}

//���ڽ���
static void __UartIRQ_Handler(void) interrupt 4 using 1
{
	if(RI)
	{
		RI = 0;
		//���ݽ���
	}
	if(TI)
	{
		TI = 0;
		busy = 0;
	}
}

