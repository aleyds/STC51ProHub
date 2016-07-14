#include "GPIO.h"
#include "base_type.h"
#include "intrins.h"
#include "IPCCmd.h"

static void __delay(H_U32 ms)
{
	H_U32 i = 0;
	H_U32 j = 0;
	for(i = ms; i> 0; i--)
	{
		for(j = 110; j > 0; j--)
		{
			
		}
	}

}

void __TDH6300Learn(void)
{
	LEARNKEY = 0;
	__delay(1000);
	LEARNKEY = 1;
}

void __TDH6300Clear(void)
{
	LEARNKEY = 0;
	__delay(8000);
	LEARNKEY = 1;
}
