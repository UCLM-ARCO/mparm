/*
 *  Mostly taken from armulator console
 *  francisco.moya@uclm.es
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/* from swi_calls.h in the MPARM source */

#define SWI_READ         0x00800100
#define SWI_WRITE        0x00800101
#define L(x) #x
#define l(x) L(x)

int _swiwrite (int fd, char *p, int len)
{
  int value;
  asm volatile ("mov r1, %1; mov r2, %2; mov r3, %3; swi " l(SWI_WRITE) "; mov %0, r0"
       : "=r" (value)
		: "r" (fd), "r" (p), "r" (len)
       : "r0", "r1", "r2", "r3", "lr");
  return value;
}

int _swiread (int fd, char *p, int len)
{
  int value;
  asm volatile ("mov r1, %1; mov r2, %2; mov r3, %3; swi " l(SWI_READ) "; mov %0, r0"
       : "=r" (value)
		: "r" (fd), "r" (p), "r" (len)
       : "r0", "r1", "r2", "r3", "lr");
  return value;
}

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
  return;
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  int nwritten;

  nwritten = _swiwrite (1, &ch , 1);

  /* error if (nwritten == -1 || nwritten == len) */
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  int nread;
  char c;

  nread = _swiread (0, &c, 1);
  if ( nread != 1 )
    return -1;

  return c;
}

#include <rtems/bspIo.h>

void MPARM_BSP_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = MPARM_BSP_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
