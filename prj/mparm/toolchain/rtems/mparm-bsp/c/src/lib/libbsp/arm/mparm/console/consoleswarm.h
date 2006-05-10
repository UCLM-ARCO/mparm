

/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 *
 * Copyright (c) Canon Research France SA.]
 * Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#ifndef _BSPUART_H
#define _BSPUART_H
void myprint(char *fmt, ...);
void convert(unsigned32 i,char* buf);
void BSP_uart_init(int uart, int baud, int hwFlow);
void BSP_uart_set_baud(int aurt, int baud);
void BSP_uart_intr_ctrl(int uart, int cmd);
void BSP_uart_throttle(int uart);
void BSP_uart_unthrottle(int uart);
int  BSP_uart_polled_status(int uart);
void BSP_uart_polled_write(int uart,char val);
int  BSP_uart_polled_read(int uart);
void BSP_uart_termios_set(int uart, void *ttyp);
int  BSP_uart_termios_write_com1(int minor, const char *buf, int len);
int  BSP_uart_termios_write_com2(int minor, const char *buf, int len);
void BSP_uart_termios_isr_com1();
void BSP_uart_termios_isr_com2();
void BSP_uart_dbgisr_com1(void);
void BSP_uart_dbgisr_com2(void); 
unsigned BSP_poll_char_via_serial(void);
void BSP_output_char_via_serial(char val);
#define BSPConsolePort 0
/*
 * Command values for BSP_uart_intr_ctrl(),
 * values are strange in order to catch errors 
 * with assert
 */
#define BSP_UART_INTR_CTRL_DISABLE  (0)
#define BSP_UART_INTR_CTRL_GDB      (0xaa) /* RX only */
#define BSP_UART_INTR_CTRL_ENABLE   (0xbb) /* Normal operations */
#define BSP_UART_INTR_CTRL_TERMIOS  (0xcc) /* RX & line status */

/* Return values for uart_polled_status() */
#define BSP_UART_STATUS_ERROR    (-1) /* No character */
#define BSP_UART_STATUS_NOCHAR   (0)  /* No character */
#define BSP_UART_STATUS_CHAR     (1)  /* Character present */
#define BSP_UART_STATUS_BREAK    (2)  /* Break point is detected */

/* PC UART definitions */
#define BSP_UART_COM1            (0)
#define BSP_UART_COM2            (1)



#endif /* _BSPUART_H */



