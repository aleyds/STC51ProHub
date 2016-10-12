
#include "GPIO.h"
#include "intrins.h"
#include "base_type.h"


#define FOSC 11059200L

#define T1MS (65536-FOSC/1000)      //1T模式
//#define T1MS (65536-FOSC/12/1000) //12T模式

#define COUNTER_10S			(10000)
static unsigned int  g_TimerCounter = 0;

#define EnableAllINT()		{EA = 1;}


void Delay1us()		//@11.0592MHz
{
	_nop_();
	_nop_();
	_nop_();
}


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


//绿灯点亮和熄灭
static void displayGreenLed(char type)
{
	if(type == 0){
		LED_GREEN = 1;
	}else{
		LED_GREEN = 0;
	}
}

//红灯点亮和熄灭
static void displayRedLed(char type)
{
	if(type == 0){
		LED_RED = 1;
	}else{
		LED_RED = 0;
	}
}

static void Timer0Init(void)		//1毫秒@11.0592MHz
{
	 AUXR |= 0x80;       //定时器0为1T模式
//  AUXR &= 0x7f;        //定时器0为12T模式

    TMOD &= 0xF0;		//设置定时器模式
    TL0 = T1MS;         //初始化计时值
    TH0 = T1MS >> 8;
	TF0 = 0;			//清除TF0标志
    TR0 = 1;            //定时器0开始计时
    ET0 = 1;            //使能定时器0中断
}

static void Timer0Stop(void)
{
	TR0 = 0; //
	ET0 = 0;  //
}


/* Timer0 interrupt routine */
void Timer0Handler() interrupt 1 using 1
{
	if(g_TimerCounter >= COUNTER_10S){
		displayRedLed(0);//红灯亮
		displayGreenLed(1);//绿灯熄灭
		g_TimerCounter = 0;
		Timer0Stop();
	}
    g_TimerCounter++;
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

static void _GpioInit()
{
    P3M0 = 0x00;
    P3M1 = 0x80;
}

void main()
{
	//_DisplayLed(0);
	_GpioInit();
	//_SleepInit();
	displayRedLed(0);//红灯熄灭
	displayGreenLed(1);//绿灯亮
	Timer0Init();//启动定会器
	EnableAllINT();
	while(1)
	{
		//_Sleep();
	
	}
}
