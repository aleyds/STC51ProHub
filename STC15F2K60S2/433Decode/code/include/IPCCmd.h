#ifndef __IPCCMD_H__
#define __IPCCMD_H__

#define PARITY_NONE			0	//无奇偶校验
#define PARITY_ODD			1	//奇校验
#define PARITY_EVEN			2	//偶校验
#define PARITY_MARK			3
#define PARITY_SPACE		4
#define PARITY_TYPE			(PARITY_NONE)

void _SendData(BYTE *pDat, BYTE len);
#endif