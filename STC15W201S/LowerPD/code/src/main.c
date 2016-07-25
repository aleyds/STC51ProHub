
#include "GPIO.h"
#include "base_type.h"

#define LOW_POWER_STEP		(200)
#define LOW_POWER_TIMER		(15)

#define EnableAllINT()		{EA = 1;}
static bit g_MotorTurn = 0;

static  void _delay(H_ULONG ms)
{
	H_ULONG i = 0;
	H_ULONG j = 0;
	for(i = ms; i> 0; i--)
	{
		for(j = 110; j > 0; j--)
		{
			;
		}
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

static H_U8 _PowerCheck(void)
{
	//低电压
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

static void EXINT0(void)  interrupt 0 
{
	PCON = 0;
}

void main()
{
	//_DisplayLed(0);
	EX0 = 1;
	
	while(1)
	{
		_TDH6300SCAN();
		_PowerCheck();
		//_Sleep();
	}
}
