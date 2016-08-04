#ifndef __PLATFOEM_H__
#define __PLATFOEM_H__
#include "stc15f2k60s2.h"

#define FOSC		(18432000L) //16M 芯片晶振大小Hz

//微动开关
sbit M_SWITCH1=P3^3;//A点1号微动开关
sbit M_SWITCH2=P3^7;//B点2号微动开关
sbit M_SWITCH3=P3^6;//C点3号微动开关
sbit T_SWITCH =P3^2;//触摸开关

//LED	
sbit EXTERNAL_LED=P1^5; //测试使用LED灯
sbit SWITCH1_LED=P1^7;//1号微动开关显示灯，离开开关时LED点亮  接触1号开关时LED熄灭
sbit SWITCH2_LED=P1^6; //

sbit ALARM_SWITCH=P3^4;

//继电器开关
sbit RELAY_C = P3^5;  //控制紫外灯，继电器
sbit RELAY_A = P1^3;  //控制电机运动方向  1
sbit RELAY_B = P1^4;  //控制电机运动方向  2

#endif