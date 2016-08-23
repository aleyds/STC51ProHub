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
	_RUNNING_A_STOP, //接触到A点开关，停止
	_RUNNING_B_STOP, //接触到B点开关，停止
	_RUNNING_ATOB, //A-B运动
	_RUNNING_BTOA, //B-A运动
	_RUNNING_ATOB_STOP, //A-B运动中停止
	_RUNNING_BTOA_STOP, //B-A运动中停止
}_eRunningStatus;

static xdata _eRunningStatus g_RunningStatus;

//开关A LED灯显示关闭
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
		_TimerClose(0);//关闭外灯后关闭定时器
		g_TimerStop = 1;
	}else 
	{
		RELAY_C = 1;
	}
	
}

//读取10次求平均值
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
	//开启15分钟定时器
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
	_TimerClose(0);//关闭外灯后关闭定时器
	g_TimerStop = 1;
}
 

static H_S8 _SwitchA(void)
{
	if(!(M_SWITCH1&0x01)) //A点1号微动开关，低电平有效
	{
		_Delayms(10);//按键抖动
		if(!(M_SWITCH1&0x01))//开关真正闭合
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _SwitchB(void)
{
	if(!(M_SWITCH2&0x01)) //B点2号微动开关，低电平有效
	{
		_Delayms(10);//按键抖动
		if(!(M_SWITCH2&0x01))//开关真正闭合
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _SwitchC(void)
{
	if(!(M_SWITCH3&0x01)) //C点3号微动开关，低电平有效
	{
		_Delayms(10);//按键抖动
		if(!(M_SWITCH3&0x01))//开关真正闭合
		{
			return 1;
		}
	}
	return 0;
}

static H_S8 _TouchSwitch(void)
{
	if((T_SWITCH&0x01)) //触摸开关，低电平有效
	{
		_Delayms(10);//按键抖动
		if((T_SWITCH&0x01))//开关真正闭合
		{
			//发出滴的声音
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

//触摸板按键，蜂鸣器响
static void _TouchAlarm()
{
	ALARM_SWITCH=1;
	_Delayms(500);
	ALARM_SWITCH=0;
}

static BYTE _ElectricityDect(void)
{
	WORD _AdcData = 0;
	_AdcData = _ADCResultAverage();//P1.0口为电流检测口
	_Delayms(200);//测试延时
	
	
	if((g_CurrrentData != 0 )&&((_AdcData > (g_CurrrentData+2)) || (_AdcData < (g_CurrrentData - 2))))
	{
		SendString("Electricity test");
		
		g_CurrrentData = _AdcData;
		return 1;//电流超过阈值
	}
	return 0;//电流正常
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
	
	if(_TouchSwitch()&&g_TouchEnable==0)//触发触摸开关
	{
		SendPrintf("Touch PullDown \n\r");
		g_TouchEnable = 1;
		g_TouchRestore = TOUCH_RESTORE_TIME;
		//1、如果状态在A点停止，则由A-B点运动，状态改为 _RUNNING_ATOB
		//2、如果状态为B点停止，则由B-A运动，状态改为 _RUNNING_BTOA
		//3、如果状态为 _RUNNING_ATOB ，则停止运动，状态为 _RUNNING_ATOB_STOP
		//4、如果状态为 _RUNNING_BTOA，则停止运动，状态为 _RUNNING_BTOA_STOP
		//5、如果状态为_RUNNING_ATOB_STOP，则由B-A返回运动，状态为_RUNNING_ATOB
		//6、如果状态为 _RUNNING_BTOA_STOP，则由A-B返回云顶，状态为 _RUNNING_BTOA
		switch(g_RunningStatus)//不同状态下，按触摸按键对应操作
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
		_TouchAlarm();//采用软件延时，所以动作完成后再响蜂鸣器
		
	}
	
	if(_SwitchA())//A点开关闭合
	{
		//停止电机运动，状态为_RUNNING_A_STOP
		__MotorStop();
		g_RunningStatus = _RUNNING_A_STOP;
		//1号开关LED熄灭
		LEDSwitchA(0);
		SendPrintf("Switch A PullDown \n\r");
	}else if(!_SwitchA())
	{
		LEDSwitchA(1);
	}
	
	
	if(_SwitchB() )
	{
		//停止电机运动，状态为_RUNNING_A_STOP
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
	
	if( (_ElectricityDect() == 1)&& (g_RunningStatus == _RUNNING_BTOA))//从A向B运动过程中，电流瞬间增大5%,电机反转
	{
		//反转为向B点运动， 状态为 _RUNNING_ATOB
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
	WKTCL = 49;//设置唤醒周期为488us*(49+1)=24.4ms
	WKTCH = 0x80;//使能掉电唤醒定时器
}
*/

static void _IOInit()
{
	//P0M0 = 0x00;//设置P1所有端口为准双向口
	//P0M1 = 0x00;
	P1M0 = 0xff;//设置P1所有端口为准双向口
	P1M1 = 0x00;
	P2M0 = 0x00;//设置P1所有端口为准双向口
	P2M1 = 0x00;
	P3M0 = 0x00;//设置P3所有端口为准双向口
	P3M1 = 0x00;
	//P4M0 = 0x00;//设置P3所有端口为准双向口
	//P4M1 = 0x00;
	P5M0 = 0x00;//设置P3所有端口为准双向口
	P5M1 = 0x00;
	//P6M0 = 0x00;//设置P3所有端口为准双向口
	//P6M1 = 0x00;
	//P7M0 = 0x00;//设置P3所有端口为准双向口
	//P7M1 = 0x00;
	RELAY_C = 0;//紫外灯关闭
	ALARM_SWITCH = 0;//蜂鸣器关闭
	T_SWITCH = 0;//触摸开关拉低，高电平触发
}




void main()
{
		_IOInit();
		InitUart();//串口初始化
		_ADCInit();//ADC初始化
		//_SleepInit();//低功耗模式，暂时不使用
		SendString("WillCome to 24V \n\r");
		//_TouchAlarm();//蜂鸣器开机提示
	
		
		g_RunningStatus = _RUNNING_INIT;
		_MotorStart(0);//启动向A点运动
		
		STARTALLIRQ();
		
		
		while(1)
		{
			//正常应用程序模式
			
			//_Sleep();//低功耗模式，暂时不使用
			_EventHandler();
			

			//使用到的IO口测试
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
			
			
			//全部GPIO测试代码
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

