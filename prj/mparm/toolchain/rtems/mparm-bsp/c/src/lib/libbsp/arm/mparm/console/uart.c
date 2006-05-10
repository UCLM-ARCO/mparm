/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 *
 *  COPYRIGHT (c) 2000 Canon Research France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 * The license and distribution terms for this file may be
 * found in found in the file LICENSE in this distribution or at
 * http://www.OARcorp.com/rtems/license.html.
 *
 *  uart.c,v 1.1 2000/07/27 01:04:10 joel Exp
 *
 *  Da questo codice sono passati in molti prima la Canon poi Joel e poi finalmente io: Poletti Francesco
 *  Il tutto e' stato riadattato per funzionare sul simulatore swarm
 */

#include <bsp.h>
#include <assert.h>
#include <string.h> /*strcmp*/
#include <stdarg.h>
#include <stdio.h>
#include <rtems/bspIo.h>

static void
cprintNum(long unsigned int num, int base, int sign, int maxwidth, int lead)
{
  long unsigned int n;
  int count;
  char toPrint[20];

  if ( (sign == 1) && ((long)num <  0) ) {
    BSP_output_char('-');
    num = -num;
    if (maxwidth) maxwidth--;
  }
  
  count = 0;
  while ((n = num / base) > 0) {
    toPrint[count++] = (num - (n*base));
    num = n ;
  }
  toPrint[count++] = num;

  if (maxwidth) {
    for (n=maxwidth-count ; n ; n-- )
      BSP_output_char(lead);
  }

  for (n = 0; n < count; n++){
    BSP_output_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
} /* cprintNum */

void convertk(unsigned32 i,char* buf)
{int j,c,max;
 char a;
 for(j=0;i>0;j++)
  {buf[j]=(i%10)+'0';
   i=i/10;
  }
  buf[j]=0;
  max=(j-(j%2))/2;
  j--;
  for (c=0;c<max;c++,j--)
  {a=buf[c];
   buf[c]=buf[j];
   buf[j]=a;
  }
}

void
vmyprint(char *fmt, va_list ap)
{ rtems_interrupt_level level;
  char     c, *str;
  int      lflag, base,base1, sign, width, lead;
  _CPU_ISR_Disable(level);
 
  for (; *fmt != '\0'; fmt++)
  {
    lflag = 0;
    base  = 0;
    base1=1;
    sign = 0;
    width = 0;
    lead = ' ';
    if (*fmt == '%')
    {
      fmt++;
      if (*fmt == '0' ) {
        lead = '0';
        fmt++;
      }
      while (*fmt >= '0' && *fmt <= '9' ) {
        
        width += ((*fmt - '0')*base1);
        fmt++;
        base1 *= 10;
      }

      if ((c = *fmt) == 'l')
      {
        lflag = 1;
        c = *++fmt;
      }
      switch (c)
      {
        case 'o': case 'O': base = 8; sign = 0; break;
        case 'd': case 'D': base = 10; sign = 1; break;
        case 'u': case 'U': base = 10; sign = 0; break;
        case 'x': case 'X': base = 16; sign = 0; break;
        case 's':
          for (str = va_arg(ap, char *); *str; str++) 
            BSP_output_char(*str);
          break;
        case 'c':
        BSP_output_char(va_arg(ap, char));
          break;
        default:
          BSP_output_char(c);
          break;
      } /* switch*/

      if (base)
        cprintNum(lflag ? va_arg(ap, long int) : (long int)va_arg(ap, int),
                 base, sign, width, lead);
    }
    else
    {
      BSP_output_char(*fmt);
    }
  }
  _CPU_ISR_Enable(level);
} /* vmyprint */

void
myprint(char *fmt, ...)
{
  va_list  ap;      /* points to each unnamed argument in turn */
 
  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  vmyprint(fmt, ap);
  va_end(ap); /* clean up when done */
} /* myprint */


struct uart_data
{
  int hwFlow;
  int baud;
};

static struct uart_data uart_data[2];

/* 
 * Macros to read/wirte register of uart, if configuration is
 * different just rewrite these macros (fatto! F.P.) 
 * Attenzione uread e uwrite implementate per una sola console! 
 */ 

#define uread(uart,reg,let) let = *((volatile unsigned long *)reg);

#define uwrite(uart,reg,val)  *((volatile unsigned long *)reg) = val;


#ifdef UARTDEBUG
    static void
uartError(int uart)
{
  
}
#else
inline void uartError(int uart)
{
}
#endif

/* 
 Di fatto non fa niente se non inizializzare la struttura del device e vuotare il buffer dalle "porcherie" di minicom... 
 
 */

/* 
 Di fatto non fa niente se non inizializzare la struttura del device e vuotare il buffer dalle "porcherie" di minicom... 
 
 */
void
BSP_uart_init(int uart, int baud, int hwFlow)
{ unsigned int stato;
  char c;
  uread (uart,SWARM_UART0_SR,stato);
  stato &= 0x01;
  while ( stato ) 
  { 
    uread(uart,SWARM_UART0_RXBASE,c);
    uread (uart,SWARM_UART0_SR,stato);
    stato &= 0x01;
  }
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  switch(baud)
    {
    case 50:
    case 75:
    case 110:
    case 134:
    case 300:
    case 600:
    case 1200:
    case 2400:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      break;
    default:
      assert(0);
      return;
    }
  
  /* Remember state */
  uart_data[uart].hwFlow     = hwFlow;
  uart_data[uart].baud       = baud;
  return;
}

/* 
 * un'altra scatola vuota...
 */
void
BSP_uart_set_baud(int uart, int baud)
{
  
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  /* 
   * This function may be called whenever TERMIOS parameters
   * are changed, so we have to make sure that baud change is 
   * indeed required
   */

  if(baud == uart_data[uart].baud)
    {
      return;
    }

  BSP_uart_init(uart, baud, uart_data[uart].hwFlow);

  
  return;
}



/*
 * Enable/disable interrupts 
   non usata in quanto la seriale non ha l'int programmabile
void 
BSP_uart_intr_ctrl(int uart, int cmd)
{

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  switch(cmd)
    {
    case BSP_UART_INTR_CTRL_DISABLE:
      uwrite(uart, IER, INTERRUPT_DISABLE);
      break;
    case BSP_UART_INTR_CTRL_ENABLE:
      uwrite(uart, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE
	      )
	     );
      break;
    case BSP_UART_INTR_CTRL_TERMIOS:
      uwrite(uart, IER,
	     (RECEIVE_ENABLE  |
	      RECEIVER_LINE_ST_ENABLE
	      )
	     );
      break;
    case BSP_UART_INTR_CTRL_GDB:
      uwrite(uart, IER, RECEIVE_ENABLE);
      break;
    default:
      assert(0);
      break;
    }
 
  return;
}

*/

/*Polled mode write function*/

void 
BSP_uart_polled_write(int uart,char val)
{
  
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  uwrite(uart,SWARM_UART0_TXBASE, val & 0xff );
      
  return;
}


void
BSP_output_char_via_serial(char val)
{
  BSP_uart_polled_write(BSPConsolePort, val);
  if (val == '\n') BSP_uart_polled_write(BSPConsolePort,'\r');
}

/* 
 * Polled mode read function
 */
int 
BSP_uart_polled_read(int uart)
{
  unsigned int val;
  unsigned32 stato; 
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  for(;;)
    {uread (uart,SWARM_UART0_SR,stato);
     stato &= 0x01;
     if (stato) break; /*bit 0 settato*/

    }
  
  uread(uart,SWARM_UART0_RXBASE,val);

  return (unsigned int) (val & 0xff);
}

unsigned 
BSP_poll_char_via_serial()
{
 return BSP_uart_polled_read(BSPConsolePort);
}


/* ================ Termios support  =================*/

static volatile int  termios_stopped_com1        = 0;
static volatile int  termios_tx_active_com1      = 0;
static void*	     termios_ttyp_com1           = NULL;
static char          termios_tx_hold_com1        = 0;
static volatile char termios_tx_hold_valid_com1  = 0;

static volatile int  termios_stopped_com2        = 0;
static volatile int  termios_tx_active_com2      = 0;
static void*	     termios_ttyp_com2           = NULL;
static char          termios_tx_hold_com2        = 0;
static volatile char termios_tx_hold_valid_com2  = 0;

/*
 * Set channel parameters 
 */
void
BSP_uart_termios_set(int uart, void *ttyp)
{
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  if(uart == BSP_UART_COM1)
    {
      termios_stopped_com1 = 0;
      termios_tx_active_com1      = 0;
      termios_ttyp_com1           = ttyp;
      termios_tx_hold_com1        = 0; 
      termios_tx_hold_valid_com1  = 0;
    }
  else
    {
      termios_stopped_com2 = 0;
      termios_tx_active_com2      = 0;
      termios_ttyp_com2           = ttyp;
      termios_tx_hold_com2        = 0; 
      termios_tx_hold_valid_com2  = 0;
    }

  return;
}

int
BSP_uart_termios_write_com1(int minor, const char *buf, int len)
{ unsigned int i;
  assert(buf != NULL);
  if(len <= 0) return 0;
  for(i=0;i<len;i++)
  {
   uwrite(BSP_UART_COM1,SWARM_UART0_TXBASE,*buf & 0xff);
   buf++;
  }

  return len;
}

void
BSP_uart_termios_isr_com1(void)
{ 
  unsigned char buf;
  uread(BSP_UART_COM1,SWARM_UART0_RXBASE,buf);
  rtems_termios_enqueue_raw_characters(termios_ttyp_com1, (char *) &buf, 1);
}


	  
