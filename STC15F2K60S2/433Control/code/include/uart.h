#ifndef __UART_H__
#define __UART_H__

#define SOC_FREQ			(11051000L)//(18432000L)
#define UART_BAUD			(9600)

#define NONE_PARITY			(0)
#define ODD_PARITY			(1)
#define EVEN_PARITY			(2)
#define MARK_PARITY			(3)
#define SPACE_PARITY		(4)

#define PARITYBIT			(NONE_PARITY)
void _UartOpen(void);
void _UartPutStr(const H_U8 *str);
void _UartPutDec(H_U32 dec);
void _CommandData(void);
void _UartSendData(const BYTE *_pDat, BYTE Len);



#endif
