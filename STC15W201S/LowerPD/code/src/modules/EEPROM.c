#include "GPIO.h"
#include "base_type.h"
#include "intrins.h"
#include "EEPROM.h"

static void __IapIdle()
{
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TRIG = 0;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0;
}

//EEPROM 写入数据
void _IapWriteByte(WORD _Addr, BYTE _Dt)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_READ;
	IAP_ADDRL = _Addr&0xff;
	IAP_ADDRH = (_Addr>>8)&0xff;
	IAP_DATA = _Dt;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	__IapIdle();
}

//读取EEPROM中的Byte数据 
BYTE _IapReadByte(WORD _Addr)
{
	BYTE dt = 0 ;
	
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_READ;
	IAP_ADDRL = _Addr&0xff;
	IAP_ADDRH = (_Addr>>8)&0xff;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	dt = IAP_DATA;
	__IapIdle();
	return dt;
}

/*擦除扇区*/
void _IapEraseSector(WORD _Addr)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_ERASE;
	IAP_ADDRL = _Addr&0xff;
	IAP_ADDRH = (_Addr>>8)&0xff;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	__IapIdle();
}