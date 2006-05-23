/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the PowerPC PSIM.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  No interrupt support.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  getcfg.c,v 1.4 1999/11/30 19:57:54 joel Exp
 */
  
#include <rtems.h>
#include <shm_driver.h>
#include <irq.h>
#include <rtems/bspIo.h>       /* printk */
#include "registers.h"

/*Communication using interrupt*/ 
#define INTERRUPT 1
#define POLLING   0                   

shm_config_table BSP_shm_cfgtbl;

/*base address of the slave1 for the interruption*/
volatile int *p1 = (int *)SLAVE1_BASE_ADDRESS;

void Shm_Get_configuration(
  rtems_rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{  int status;
   /*base address of the shared memory*/
   BSP_shm_cfgtbl.base         = (rtems_rtems_unsigned32 *)SHARED_BASE_ADDRESS;
   /*lenght of the shared memory*/
   BSP_shm_cfgtbl.length       = RTEMS_SHARED_SIZE; 
   BSP_shm_cfgtbl.format       = SHM_LITTLE;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

   /*In questo modo nella shared_driver.h  c'e' una macro che non fa altro che ritornare l'indirizzo ongi volta viene invocata
     la Shm_Convert*/ 
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;

#if (POLLING==1)
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address = &p1[Shm_Local_node];
   BSP_shm_cfgtbl.Intr.value   = 0x1;
   BSP_shm_cfgtbl.Intr.length  = 4; /*FOUR BYTE*/
#endif

   *shmcfg = &BSP_shm_cfgtbl;
}
