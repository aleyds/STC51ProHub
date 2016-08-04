
#include "GPIO.h"
#include "intrins.h"
#include "base_type.h"

#define LOW_POWER_STEP		(200)
#define LOW_POWER_TIMER		(15)

#define EnableAllINT()		{EA = 1;}
static bit g_MotorTurn = 0;

static void __Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

static  void _delay(H_ULONG ms)
{
	while(ms--)
	{
		__Delay1ms();
	}

}

static void _MotorStop(void)
{
	MOTOR_PORTA = 0;
	MOTOR_PORTB = 0;
}

static void _MotorTurn(H_U8 _Direct)
{
	if(_Direct)
	{
		MOTOR_PORTB = 0;
		MOTOR_PORTA = 1;
	}else
	{
		MOTOR_PORTA = 0;
		MOTOR_PORTB = 1;
	}
	_delay(1000);
	_MotorStop();
}

static void _DisplayLed(H_U8 _Led)
{
	switch(_Led)
	{
		case 0:
			LED_LOW_VOLTAGE = 0;
			_delay(500);
			LED_LOW_VOLTAGE = 1;
			break;
		case 1:
			LED_CHANGE = 0;
			_delay(500);
			LED_CHANGE = 1;
			break;
		default:
			break;
	}
}

static void _LowPowerAlarm(void)
{
	H_U8 i = 0;
	for(i = 0; i < LOW_POWER_TIMER; i++)
	{
		LED_LOW_VOLTAGE = 0;
		_delay(LOW_POWER_STEP);
		LED_LOW_VOLTAGE = 1;
		_delay(LOW_POWER_STEP);
	}
}

static H_U8  _TDH6300SCAN(void)
{
	if(TDH6300_VT&0x01)
	{
		if(TDH6300_JDQ&0x01)
		{
			//_DisplayLed(0);
			//低电压检测
			if((!(LOW_POWER_DECT &0x1)) && (CHARG_DET&0x1))
			{
				_LowPowerAlarm();
				g_MotorTurn = 1;
			}
			if(g_MotorTurn)
			{
				_MotorTurn(0);
				g_MotorTurn = 0;
			}else
			{
				_MotorTurn(1);
				g_MotorTurn = 1;
			}
			return 1;
		}
	}
	return 0;
}

static void _PowerCheck(void)
{
	//充电检测
	if(!(CHARG_DET &0x1))
	{
		LED_CHANGE = 0;
	}
	else{
		LED_CHANGE = 1;
	}
}

static void _Sleep(void)
{
	PCON = 0x02;
}

static void _SleepInit(void)
{
	WKTCL = 49;//设置唤醒周期为488us*(49+1)=24.4ms
	WKTCH = 0x80;//使能掉电唤醒定时器
}

 

void main()
{
	//_DisplayLed(0);
	_SleepInit();
	EnableAllINT();
	while(1)
	{
		_Sleep();
		_TDH6300SCAN();
		_PowerCheck();
	}
}
