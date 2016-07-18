#include "GPIO.h"
#include "base_type.h"
#include "intrins.h"
#include "EV1527.h"
#include "IPCCmd.h"

extern void _delay(H_U32 ms);


void __TDH6300Learn(void)
{
	LEARNKEY = 0;
	_delay(100);
	LEARNKEY = 1;
}

void __TDH6300Clear(void)
{
	LEARNKEY = 0;
	_delay(20000);
	LEARNKEY = 1;
}

void _EV1527Test(void)
{
	BYTE buf[1] = {0x12};
	_EV1527SendData(buf,1);
}
