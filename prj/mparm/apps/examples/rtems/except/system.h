#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems.h>
#include <bsp.h>

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

static inline rtems_task Init(rtems_task_argument);

#include <confdefs.h>
//#include <rtems/confdefs.h>

#include <stdlib.h>

extern "C" {
  void _init (void);
  void _fini (void);
}

extern int main();

static inline rtems_task
Init(rtems_task_argument argument)
{
  atexit(_fini);
  _init();
  int status = main();
  exit(status);
}

#endif
