#ifndef __TDH6300_H__
#define __TDH6300_H__

typedef enum Tdh6300Cmd{
	_TDH_CMD_DOPLOY = 1,
	_TDH_CMD_DISARM = 2,
	_TDH_CMD_HOME = 4,
	_TDH_CMD_SOS = 8,
	_TDH_CMD_DOOR = 14,
}enTdh6300Cmd;

void _TDH6300Open(void);
void _TDH6300Scan(void);
#endif