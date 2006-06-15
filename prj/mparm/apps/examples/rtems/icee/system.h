#include "mpsim.h"
#include <bsp.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS              20
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  5
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              40
#define CONFIGURE_MAXIMUM_SEMAPHORES                 30

#define TASK_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_EXTRA_TASK_STACKS  ((RTEMS_MINIMUM_STACK_SIZE*2)*3)

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

static inline void* POSIX_Init(void*_argument);

//#include <rtems/confdefs.h> // RTEMS 4.7
#include <confdefs.h> // RTEMS 4.6

extern int main();

#ifndef CORRECT_GCC_BEHAVIOR

extern "C" {
  void _init (void);
  void _fini (void);
}

static inline void*
POSIX_Init(void* argument)
{
  atexit(_fini);
  _init();
  int status = main();
  exit(status);
}

#else

static inline rtems_task
POSIX_Init(rtems_task_argument argument) {
  exit(main());
}

#endif
