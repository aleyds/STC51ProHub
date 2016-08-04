#ifndef __TDH6300_H__
#define __TDH6300_H__

typedef enum Tdh6300Cmd{
	_TDH_CMD_DOPLOY = 4,//布防
	_TDH_CMD_DISARM = 8, //撤防
	_TDH_CMD_HOME = 2,//情景模式
	_TDH_CMD_SOS = 1,//SOS功能
	_TDH_CMD_DOOR = 7,//门磁检测
	_TDH_CMD_LOW_POWER = 5,//欠压
	_TDH_CMD_BODY = 9,
}enTdh6300Cmd;

void _TDH6300Open(void);
void _TDH6300Scan(void);
#endif