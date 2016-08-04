#include "base_type.h"
#include "uart.h"
#include "timer.h"

#define TH0HIGHT		(0xFC)   //��ʽ2  100uS
#define TH0LOW			(0x66)

typedef struct TimerSt{
	H_U32 _TimeInit;
	H_U32 _TimeCount;
	TimerCallback _Call;
}TimerSt_t;

typedef struct TimerManage{
	TimerSt_t _Timer0;
}TimerManage_t;

static xdata TimerManage_t g_TimerManage;
static  TimerManage_t* __TimerManage(void)
{
	return (TimerManage_t *)&g_TimerManage;
}

H_U32 _TimerCreat(H_U8 _Number, H_U32 _TimeOut, TimerCallback _Call)
{
	TimerManage_t* _Manage = __TimerManage();
	switch(_Number)
	{
		case _TYPE_TIMER_0:

			AUXR |= 0x80;                   //��ʱ��0Ϊ1Tģʽ
			//  AUXR &= 0x7f;               //��ʱ��0Ϊ12Tģʽ
			TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
			TL0 = T1MS;                     //��ʼ����ʱֵ
			TH0 = T1MS >> 8;
			_Manage->_Timer0._TimeInit = _TimeOut;
			_Manage->_Timer0._TimeCount = 0;
			_Manage->_Timer0._Call = _Call;
			SendPrintf("Creat Timer\n\r");
			break;
		default:
			break;
	}
	return H_SUCCESS;
}

H_U32 _TimerStart(H_U8 _Number)
{
	switch(_Number)
	{
		case _TYPE_TIMER_0:
			TR0=1;//������ʱ��0
			ET0=1;//������ʱ��0�ж�
			break;
		default:
			break;
	}
	return H_SUCCESS;
}

H_U32 _TimerClose(H_U8 _Number)
{
	switch(_Number)
	{
		case _TYPE_TIMER_0:
			ET0=0;//�رն�ʱ��0�ж�
			TR0=0;//�رն�ʱ��0
			SendPrintf("Close Timer\n\r");
			break;
		default:
			break;
	}
	return H_SUCCESS;
}

//��ʱ��0�жϻص�����
void Timer0_IRQHandler() interrupt 1 using 1
{
	
	TimerManage_t* _Manage = __TimerManage();
	//TH0 = TH0HIGHT; //���ö�ʱ��0��ֵ 5ms
	//TL0 = TH0LOW;
	if(_Manage->_Timer0._TimeCount >= _Manage->_Timer0._TimeInit)
	{
		_Manage->_Timer0._TimeCount = 0;//��������COUNTER
		if(_Manage->_Timer0._Call != H_NULL)
		{
			_Manage->_Timer0._Call();//ִ��Timer�ص�����
		}
	}
	_Manage->_Timer0._TimeCount++;
}