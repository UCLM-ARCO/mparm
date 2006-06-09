#ifndef APPSUPPORT_H
#define APPSUPPORT_H

/*
 * Copyright 2003 DEIS - Universita' di Bologna
 * 
 * name         appsupport.h
 * author       DEIS - Universita' di Bologna
 * author       DEIS - Universita' di Bologna
 *              Federico Angiolini - fangiolini@deis.unibo.it
 *              Mirko Loghi - mloghi@deis.unibo.it
 *              Francesco Poletti - fpoletti@deis.unibo.it
 * info         Provides support for testbench compilation
 *
 */

#include "config.h"

#define ALLOCABLE_SHM_OFFSET  0x00001000
#define USER_SHM_OFFSET       0x00080000

#define HWLOCK_SPLIT          0xa


/* start_metric - Starts statistic collection for this processor */
inline static void
start_metric()
{
  volatile char *ptr = (char *)(SIMSUPPORT_BASE + START_METRIC_ADDRESS);
  *ptr = 1;
}

/* stop_metric - Stops statistic collection for this processor */
inline static void
stop_metric()
{
  volatile char *ptr = (char *)(SIMSUPPORT_BASE + STOP_METRIC_ADDRESS);
  *ptr = 1;
}

inline static void
time_start(int r)
{
  (*(volatile int *)(SIMSUPPORT_BASE+DUMP_TIME_START)) = r;
}

inline static void
time_stop(int r)
{
  (*(volatile int *)(SIMSUPPORT_BASE+DUMP_TIME_STOP)) = r;
}

/* get_proc_id - Allows getting the processor's ID (from 1 onwards) */
inline static unsigned int
get_proc_id()
{
  char *ptr = (char *)(SIMSUPPORT_BASE + GET_CPU_ID_ADDRESS);
  return (*(unsigned long int *)ptr);
}

/* get_proc_num - Allows getting the number of processors in the platform */
inline static unsigned int
get_proc_num()
{
  char *ptr = (char *)(SIMSUPPORT_BASE + GET_CPU_CNT_ADDRESS);
  return (*(unsigned long int *)ptr);
}

/* This is to prevent the compiler from optimizing away the polling code */
#define dummy(a) (a)


#define LOCK(i) (*(volatile int*)(SEMAPHORE_BASE + SEMAPHORE_SIZE \
				  - HWLOCK_SPLIT + i))

/*
 * TEST_AND_SET - Returns the old value of a lock
 */
inline static int
TEST_AND_SET(int ID)
{
  int a;
  a = LOCK(ID);
  return a;
}

/*
 * WAIT - Spins on a lock
 */
inline static void
WAIT(int ID)
{
  while (LOCK(ID))
    ;
}

/*
 * SIGNAL - Releases a lock
 */
inline static void
SIGNAL(int ID)
{
  LOCK(ID) = 0;
}


/* --------------------------------------------------------------------
   Shared Memory map:
      0x0000: flag to notify shared allocator initialization
      0x0010: pointer to the next memory block to be allocated
      0x0200: flag to notify system initialization
      0x0300: barriers (a barrier requires 8 bytes)
      0x1000: start of the dynamically allocatable shared memory
      0x80000: start of the user memory
 
  Hardware Lock map:
      first -> last - 0x14: user-available (semaphores[])
      last - 0x10 -> last: used by WAIT, SIGNAL, TEST_AND_SET (lock[])
      The lock[] array points to
        ((int*)(SEMAPHORE_BASE + SEMAPHORE_SIZE)) - HWLOCK_SPLIT
      Being HWLOCK_SPLIT == 0xa, lock[] has six negative locations and
      ten positive. Locks with negative IDs (-6 to -1) are reserved for
      the functions of the support library; locks with positive IDs (0 
      to 9) are again user-available

  Hardware Locks used by the support library map:
      -6  :    (reserved for future use)
      -5  :    (reserved for future use)
      -4  :    (reserved for future use)
      -3  :  synchronizes the shared memory allocator
      -2  :  synchronizes accesses to barriers
      -1  :    (reserved for future use)
-------------------------------------------------------------------- */

#endif
