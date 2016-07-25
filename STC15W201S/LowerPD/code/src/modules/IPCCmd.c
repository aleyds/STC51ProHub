#include "GPIO.h"
#include "base_type.h"
#include "intrins.h"
#include "EV1527.h"
#include "uart.h"
#include "IPCCmd.h"


extern void _delay(unsigned long ms);
extern BYTE _VerifyData(BYTE *_Dat, BYTE len);

void __TDH6300Learn(void)
{
	LEARNKEY = 0;
	_delay(200);
	LEARNKEY = 1;
}

void __TDH6300Clear(void)
{
	LEARNKEY = 0;
	_delay(12000);
	LEARNKEY = 1;
}

void _EV1527Test(void)
{
	BYTE buf[2] = {0x12,0x13};
	_EV1527SendData(buf,2);
	//_EV1527Send4Bit(0x7);
}

void _EV1527Control(const BYTE *DeviceID, BYTE _Dat)
{
	BYTE _SendBuf[3] = { 0 };
	_SendBuf[0] = (DeviceID[0] << 4)|(DeviceID[1] >> 4);
	_SendBuf[1] = (DeviceID[1] << 4)|(DeviceID[2] >> 4);
	_SendBuf[2] = (DeviceID[2] << 4)|(_Dat&0xf);
	_EV1527SendData(_SendBuf, 3);
}

void _IpcBackCmd(const BYTE Type, const BYTE *DeviceID, const BYTE *Data, BYTE _DatLen ,BYTE *_pCmd)
{
	BYTE i = 0;
	if(_pCmd == H_NULL)
	{
		return ;
	}
	_pCmd[0] = 0xAA;
	_pCmd[1] = 0x55;
	_pCmd[2] = _DatLen+5;
	_pCmd[3] = Type;
	memcpy(&(_pCmd[4]),DeviceID, 3);
	
	if((H_NULL != Data) && (_DatLen > 0 ))
	{
		for(i = 0; i < _DatLen; i++)
		{
			_pCmd[i+7] = Data[i];
		}
	}
	_pCmd[i+7] = _VerifyData(_pCmd, (_pCmd[2]+2));
	_UartSendData(_pCmd,(_pCmd[2]+3));
}

