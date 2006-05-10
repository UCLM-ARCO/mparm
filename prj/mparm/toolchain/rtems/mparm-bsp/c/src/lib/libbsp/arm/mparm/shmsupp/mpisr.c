/* 
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  mpisr.c,v 1.3 2001/10/12 21:06:39 joel Exp
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>
#include <irq.h>

/*  void _Shm_setvec( )
 *
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */

void exton(const rtems_irq_connect_data *irq);
void extoff(const rtems_irq_connect_data *irq);
int extison(const rtems_irq_connect_data *irq);
static rtems_irq_connect_data ext_int_data = {  EXT_INT,
  						Shm_isr,
  						exton,
  						extoff,
  						extison,
  						0,
  						0
					       };

void exton (const rtems_irq_connect_data* irq)
{
 son(irq); /*abilito l'int nell'int controller*/
}
void extoff (const rtems_irq_connect_data* irq)
{
 soff(irq); /*disabilito l'int nell'int controller*/
}
int extison (const rtems_irq_connect_data* irq)
{return (sison(irq));
}


void Shm_setvec()
{  int status;
    status = BSP_install_rtems_irq_handler(&ext_int_data);
    ext_int_data.on(&ext_int_data);

}
