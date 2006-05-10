/*  Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  lock.c,v 1.3 2001/10/12 21:06:39 joel Exp
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

typedef volatile unsigned int volint;
volint *p = (volint *)0x20000000;
/*
 *  Shm_Initialize_lock
 *
 *  Initialize the lock for the specified locked queue.
 */

void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  /* nothing required -- done implicitly by device tree */
}

/*  void _Shm_Lock( &lq_cb )
 *
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */

void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{ int numero;
  numero = (int) lq_cb->owner;
  /*printf("\nC:%x\n",numero);*/
  while (p[numero]!=0);
  /*printf("\nO:%x\n",numero);*/
}

/*
 *  Shm_Unlock
 *
 *  Unlock the lock for the specified locked queue.
 */

void Shm_Unlock(
  Shm_Locked_queue_Control *lq_cb
)
{ int numero;
  numero = (int) lq_cb->owner;
  p[numero] = 0;
  /*printf("\nR:%x\n",numero);*/
}

