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
#include "sim_support_flags.h"

extern volatile int           *semaphores;
extern volatile unsigned char *shared;
extern volatile unsigned int  *interruptdevice;
extern volatile unsigned int  *freqdevice;
extern unsigned int  NODE_NUMBER;
extern unsigned int  NNODES;


/* -------------------------------
 * Memory-mapped support functions
 * -------------------------------
 */

void pr(char *msg, unsigned long int value, unsigned long int mode);

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

void end_boot();
void stop_simulation();
void dump_metric();
void dump_light_metric();
void clear_metric();
unsigned int get_argc();
char **get_argv();
char **get_envp();
unsigned long long int get_time();
unsigned long long int get_cycle();
unsigned long int access_file(char *filename, unsigned long int mode);

/* ---------------------------
 * Frequency scaling functions
 * ---------------------------
 */
void scale_this_core_frequency(unsigned short int divider);
void scale_device_frequency(unsigned short int divider, int ID);
unsigned short int get_this_core_frequency();
unsigned short int get_device_frequency(int ID);


/* ---------------------------------
 * Shared memory allocation function
 * ---------------------------------
 */
void *shared_alloc(int size);


/* -------------------------
 * Synchronization functions
 * -------------------------
 */
inline static int TEST_AND_SET(int ID);
inline static void WAIT(int ID);
inline static void SIGNAL(int ID);
void send_interrupt(int ID);
//void interrupt_function() __attribute__((interrupt));
void WAIT_FOR_INITIALIZATION();
void INITIALIZATION_DONE();
void BARINIT(int ID);
void BARRIER(int ID, int n_proc);


/* This is to prevent the compiler from optimizing away the polling code */
#define dummy(a) (a)

extern volatile int *lock;

/*
 * TEST_AND_SET - Returns the old value of a lock
 */
inline static int
TEST_AND_SET(int ID)
{
  int a;
  a = lock[ID];
  return a;
}

/*
 * WAIT - Spins on a lock
 */
inline static void
WAIT(int ID)
{
  while (dummy(lock[ID]))
    ;
}

/*
 * SIGNAL - Releases a lock
 */
inline static void
SIGNAL(int ID)
{
  lock[ID] = 0;
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


/* normalize_address - Aligns address to the next 0x10 boundary */
#define normalize_address(A)  ( ( ((unsigned int)A) + 0xe) & (~0xf) )

#define SHM_IN_FLAG_OFFSET    0x00000000
#define SHM_VARNEXT_OFFSET    0x00000010
#define INIT_FLAG_OFFSET      0x00000200
#define BARRIER_OFFSET        0x00000300
#define ALLOCABLE_SHM_OFFSET  0x00001000
#define USER_SHM_OFFSET       0x00080000

#define HWLOCK_SPLIT          0xa

#define BARRIER_SIZE          (2 * sizeof(int))


/* ---------------
 * Debug functions
 * ---------------
 */
#ifdef DEBUG
# ifndef NOCORE
#  define DIE(a) abort();            // CORE DUMP!!!
# else
#  define DIE(a) exit(a);
# endif

# define ASSERT(cond) if (!(cond))                                           \
                       {                                                     \
                         pr("Assert failed [" #cond "] on file "             \
                            __FILE__ " - line " __LINE__ "", 0x0,            \
                            PR_CPUID, PR_STRING, PR_NEWL);                   \
                         DIE(0xdeadbeef);                                    \
                       }
# define SHOW_DEBUG(x)     pr(x, PR_CPUID, PR_STRING, PR_NEWL)
# define SHOW_DEBUG_INT(x) pr(x, PR_CPUID, PR_INT, PR_NEWL)
#else
# define ASSERT(cond)
# define SHOW_DEBUG(x)
# define SHOW_DEBUG_INT(x)
#endif



#endif
