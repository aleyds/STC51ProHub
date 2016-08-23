#include "base_type.h"
#include <platform.h>
#include "intrins.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"

#define STARTALLIRQ()		{EA=1;}

/*
void Delay1us()		//@18.432MHz
{
	unsigned char i;

	i = 2;
	while (--i);
}
*/

void Delay1ms()		//@18.432MHz
{
	unsigned char i, j;

	i = 18;
	j = 235;
	do
	{
		while (--j);
	} while (--i);
}


void _Delayms(unsigned int ms)
{
	while(ms--)
	{
		Delay1ms();
	}
}


#define TIME_15MIN		(15*60*1000)

#define TOUCH_RESTORE_TIME		(3)
static xdata H_U8 g_TouchEnable = 0;
static xdata H_U32 g_TouchRestore = 0;
static xdata H_U32 g_CurrrentData = 0;
static xdata H_U8 g_TimerStop = 1;
static xdata H_U8 g_TimerSta = 0;
//static xdata H_U8 g_Switch3 = 0;
#define ADC_POWER_MAX		(0x8f)
#define ADC_POWER_MIN		(0x70)

typedef enum _eRunningStatus_{
	_RUNNING_INIT = 0,
	_RUNNING_A_STOP, //�Ӵ���A�㿪�أ�ֹͣ
	_RUNNING_B_STOP, //�Ӵ���B�㿪�أ�ֹͣ
	_RUNNING_ATOB, //A-B�˶�
	_RUNNING_BTOA, //B-A�˶�
	_RUNNING_ATOB_STOP, //A-B�˶���ֹͣ
	_RUNNING_BTOA_STOP, //B-A�˶���ֹͣ
}_eRunningStatus;

static xdata _eRunningStatus g_RunningStatus;

//����A LED����ʾ�ر�
static void LEDSwitchA(BYTE display)
{
	if(display)
	{
		SWITCH1_LED = 0;
	}else
	{
		SWITCH1_LED = 1;
	}
}


static void __Timer0Callback(void)
{
	//TODO:
	SendPrintf(" Timer Call\n\r ");
		
	if(RELAY_C&0x01)
	{
		RELAY_C  = 0;
		_TimerClose(0);//�ر���ƺ�رն�ʱ��
		g_TimerStop = 1;
	}else 
	{
		RELAY_C = 1;
	}
	
}

//��ȡ10����ƽ��ֵ
static H_U16 _ADCResultAverage(void)
{
	H_U8 i = 0;
	H_U32 sum = 0;
	for(i = 0; i < 20;i++)
	{
		sum += _ADCGetResult(0);
	}
	return (sum/20);
}

static void __MotorStop(void)
{
	//����15���Ӷ�ʱ��
	if(g_TimerStop)
	{
		_TimerCreat(0, TIME_15MIN, __Timer0Callback);
		_TimerStart(0);
		g_TimerStop = 0;
	}
	SendString("__MotorStop ");
	
	RELAY_A = 0;
	RELAY_B = 0;
	g_CurrrentData = 0;
}

static void __MotorStart(H_U8 _Turn)
{
	if(_Turn == 0)
	{
		RELAY_B = 0;
		RELAY_A = 1;
		
	}else
	{
		RELAY_A = 0;
		RELAY_B = 1;
	}
	SendString("__MotorStart ");

	g_CurrrentData = _ADCResultAverage();
	_TimerClose(0);//�ر���ƺ�رն�ʱ��
	g_TimerStop = 1;
}
 

static H_S8 _SwitchA(void)
{
	if(!(M_SWITCH1&0x01)) //A��1��΢�����أ��͵�ƽ��Ч
	{
		_Delayms(10);//��������
		if(!(M_SWITCH1&0x01))//���������պ�
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _SwitchB(void)
{
	if(!(M_SWITCH2&0x01)) //B��2��΢�����أ��͵�ƽ��Ч
	{
		_Delayms(10);//��������
		if(!(M_SWITCH2&0x01))//���������պ�
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _SwitchC(void)
{
	if(!(M_SWITCH3&0x01)) //C��3��΢�����أ��͵�ƽ��Ч
	{
		_Delayms(10);//��������
		if(!(M_SWITCH3&0x01))//���������պ�
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _TouchSwitch(void)
{
	if((T_SWITCH&0x01)) //�������أ��͵�ƽ��Ч
	{
		_Delayms(10);//��������
		if((T_SWITCH&0x01))//���������պ�
		{
			//�����ε�����
			return 1;
		}
	}
	return 0;
}

static H_U16 _ADCGetPoint(H_U16 current)
{
	if(current == 0)
	{
		return 0;
	}
	 return ((current - 511)>>3);
}

//�����尴������������
static void _TouchAlarm()
{
	ALARM_SWITCH=1;
	_Delayms(500);
	ALARM_SWITCH=0;
}

static BYTE _ElectricityDect(void)
{
	WORD _AdcData = 0;
	_AdcData = _ADCResultAverage();//P1.0��Ϊ��������
	_Delayms(200);//������ʱ
	
	
	if((g_CurrrentData != 0 )&&((_AdcData > (g_CurrrentData+2)) || (_AdcData < (g_CurrrentData - 2))))
	{
		SendString("Electricity test");
		
		g_CurrrentData = _AdcData;
		return 1;//����������ֵ
	}
	return 0;//��������
}

static void _EventHandler(void)
{
	g_TouchRestore = (g_TouchRestore > TOUCH_RESTORE_TIME)?TOUCH_RESTORE_TIME:g_TouchRestore;
	if(g_TouchRestore == 0)
	{
		g_TouchEnable = 0;
	}else 
	{
		g_TouchRestore--;
	}
	
	if(_TouchSwitch()&&g_TouchEnable==0)//������������
	{
		SendPrintf("Touch PullDown \n\r");
		g_TouchEnable = 1;
		g_TouchRestore = TOUCH_RESTORE_TIME;
		//1�����״̬��A��ֹͣ������A-B���˶���״̬��Ϊ _RUNNING_ATOB
		//2�����״̬ΪB��ֹͣ������B-A�˶���״̬��Ϊ _RUNNING_BTOA
		//3�����״̬Ϊ _RUNNING_ATOB ����ֹͣ�˶���״̬Ϊ _RUNNING_ATOB_STOP
		//4�����״̬Ϊ _RUNNING_BTOA����ֹͣ�˶���״̬Ϊ _RUNNING_BTOA_STOP
		//5�����״̬Ϊ_RUNNING_ATOB_STOP������B-A�����˶���״̬Ϊ_RUNNING_ATOB
		//6�����״̬Ϊ _RUNNING_BTOA_STOP������A-B�����ƶ���״̬Ϊ _RUNNING_BTOA
		switch(g_RunningStatus)//��ͬ״̬�£�������������Ӧ����
		{
			case _RUNNING_A_STOP:
				g_RunningStatus = _RUNNING_ATOB;
				__MotorStart(1);
				break;
			case _RUNNING_B_STOP:
				g_RunningStatus = _RUNNING_BTOA;
				__MotorStart(0);
				break;
			case _RUNNING_ATOB:
				g_RunningStatus = _RUNNING_ATOB_STOP;
				__MotorStop();
				break;
			case _RUNNING_BTOA:
				g_RunningStatus = _RUNNING_BTOA_STOP;
				__MotorStop();
				break;
			case _RUNNING_ATOB_STOP:
				g_RunningStatus = _RUNNING_ATOB;
				__MotorStart(0);
				break;
			case _RUNNING_BTOA_STOP:
				g_RunningStatus = _RUNNING_BTOA;
				__MotorStart(1);
				break;
			default:
				break;
		}
		_TouchAlarm();//���������ʱ�����Զ�����ɺ����������
		
	}
	
	if(_SwitchA())//A�㿪�رպ�
	{
		//ֹͣ����˶���״̬Ϊ_RUNNING_A_STOP
		__MotorStop();
		g_RunningStatus = _RUNNING_A_STOP;
		//1�ſ���LEDϨ��
		LEDSwitchA(0);
		SendPrintf("Switch A PullDown \n\r");
	}else if(!_SwitchA())
	{
		LEDSwitchA(1);
	}
	
	
	if(_SwitchB() )
	{
		//ֹͣ����˶���״̬Ϊ_RUNNING_A_STOP
		__MotorStop();
		g_RunningStatus = _RUNNING_B_STOP;
		SendPrintf("Switch B PullDown \n\r");
	
	}
	
	if ((!_SwitchC() && (g_RunningStatus == _RUNNING_ATOB)))
	{

		g_RunningStatus = _RUNNING_BTOA;
		__MotorStart(0);
		SendString("Switch C PullDown \n\r");
	}
	
	if( (_ElectricityDect() == 1)&& (g_RunningStatus == _RUNNING_BTOA))//��A��B�˶������У�����˲������5%,�����ת
	{
		//��תΪ��B���˶��� ״̬Ϊ _RUNNING_ATOB
		g_RunningStatus = _RUNNING_ATOB;
		__MotorStart(1);
		SendString("Electricity Gain\n\r");
	}
}


 /*
static void _Sleep(void)
{
	PCON = 0x02;
}

static void _SleepInit(void)
{
	WKTCL = 49;//���û�������Ϊ488us*(49+1)=24.4ms
	WKTCH = 0x80;//ʹ�ܵ��绽�Ѷ�ʱ��
}
*/

static void _IOInit()
{
	//P0M0 = 0x00;//����P1���ж˿�Ϊ׼˫���
	//P0M1 = 0x00;
	P1M0 = 0xff;//����P1���ж˿�Ϊ׼˫���
	P1M1 = 0x00;
	P2M0 = 0x00;//����P1���ж˿�Ϊ׼˫���
	P2M1 = 0x00;
	P3M0 = 0x00;//����P3���ж˿�Ϊ׼˫���
	P3M1 = 0x00;
	//P4M0 = 0x00;//����P3���ж˿�Ϊ׼˫���
	//P4M1 = 0x00;
	P5M0 = 0x00;//����P3���ж˿�Ϊ׼˫���
	P5M1 = 0x00;
	//P6M0 = 0x00;//����P3���ж˿�Ϊ׼˫���
	//P6M1 = 0x00;
	//P7M0 = 0x00;//����P3���ж˿�Ϊ׼˫���
	//P7M1 = 0x00;
	RELAY_C = 0;//����ƹر�
	ALARM_SWITCH = 0;//�������ر�
	T_SWITCH = 0;//�����������ͣ��ߵ�ƽ����
}




void main()
{
		_IOInit();
		InitUart();//���ڳ�ʼ��
		_ADCInit();//ADC��ʼ��
		//_SleepInit();//�͹���ģʽ����ʱ��ʹ��
		SendString("WillCome to 24V \n\r");
		//_TouchAlarm();//������������ʾ
	
		
		g_RunningStatus = _RUNNING_INIT;
		_MotorStart(0);//������A���˶�
		
		STARTALLIRQ();
		
		
		while(1)
		{
			//����Ӧ�ó���ģʽ
			
			//_Sleep();//�͹���ģʽ����ʱ��ʹ��
			_EventHandler();
			

			//ʹ�õ���IO�ڲ���
			/*
			P3 = 0x00;
			P1 = 0x00;
			//SendString("Looping1 \n\r  ");
			//SendHex(P1);
			_Delayms(3000);
			
			//RELAY_C = 1;
			//RELAY_A = 1;
			//RELAY_B = 1;
			//ALARM_SWITCH = 1;
			P3 = 0xff;
			P1 = 0xff;
			//SendString("Looping2 \n\r  "); 
			//SendHex(P1);
			_Delayms(3000);
			*/
			
			
			//ȫ��GPIO���Դ���
			//P0 = 0xFF;
			//P1 = 0xFF;
			//P2 = 0xFF;
			//P3 = 0xEF;
			//P4 = 0xFF;
			//P5 = 0xFF;
			//P6 = 0xFF;
			//P7 = 0xFF;
			//_Delayms(3000);
			//P0 = 0x00;
			//P1 = 0x00;
			//P2 = 0x00;
			//P3 = 0x00;
			//P4 = 0x00;
			//P5 = 0x00;
			//P6 = 0x00;
			//P7 = 0x00;
			//_Delayms(3000); 
			//SendString("Looping \n\r"); 

		}
}

