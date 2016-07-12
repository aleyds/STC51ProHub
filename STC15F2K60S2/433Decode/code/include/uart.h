#ifndef __UART_H__
#define __UART_H__

#define SOC_FREQ			(18432000L)
#define UART_BAUD			(115200)

#define NONE_PARITY			(0)
#define ODD_PARITY			(1)
#define EVEN_PARITY			(2)
#define MARK_PARITY			(3)
#define SPACE_PARITY		(4)

#define PARITYBIT			(NONE_PARITY)
void _UartOpen(void);
void _UartPrintf(H_U8 *fmt, ...);
#endif
