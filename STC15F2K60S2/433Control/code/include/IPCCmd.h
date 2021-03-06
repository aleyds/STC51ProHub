#ifndef __IPCCMD_H__
#define __IPCCMD_H__

typedef enum IPCCmd{
	_TYPE_6300_LEARN = 0,
	_TYPE_6300_CLEAR ,
	_TYPE_1527_TEST,
	_TYPE_1527_CONTROL,
}enIPCCmd;

typedef enum IPCBack{
	_BACK_NO_ALARM = 0,
	_BACK_ALARM,
	_BACK_BATTERY_LOW,
	_BACK_SEND_CMD,
	_BACK_DEPLOY,
	_BACK_DISARM,
	_BACK_HOME,
	_BACK_SOS,
}enIPCBack;

void __UartSend(H_U8 _ch);
void __TDH6300Learn(void);
void __TDH6300Clear(void);
void _EV1527Test(void);
void _EV1527Control(const BYTE *DeviceID, BYTE _Dat);
void _IpcBackCmd(BYTE Type, const BYTE *DeviceID, const BYTE *Data, BYTE _DatLen );
#endif