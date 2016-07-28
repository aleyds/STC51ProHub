#ifndef __UART_H__
#define __UART_H__

#define SOC_FREQ			(26975000L)//(11051000L)//(18432000L)
#define UART_BAUD			(115200)

#define NONE_PARITY			(0)
#define ODD_PARITY			(1)
#define EVEN_PARITY			(2)
#define MARK_PARITY			(3)
#define SPACE_PARITY		(4)

#define PARITYBIT			(NONE_PARITY)

typedef struct _IpcCmd{
	BYTE len;
	BYTE type;
	BYTE device_id[3];
	BYTE _pdata[2];
}_IpcCmd_t;

extern void _UartOpen(void);
extern void _UartPutStr(const H_U8 *str);
extern void _UartPutDec(H_U32 dec);
extern void _IPCCmdRecv(void);
extern void _UartSendData(const BYTE *_pDat, BYTE Len);



#endif
