#include <rtems.h>
#include <stdlib.h>
#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

static inline rtems_task Init(rtems_task_argument);

#include <confdefs.h>
//#include <rtems/confdefs.h>

extern int main();

#ifndef CORRECT_INITFINI

extern "C" {
  void _init (void);
  void _fini (void);
}

static inline rtems_task
Init(rtems_task_argument argument)
{
  atexit(_fini);
  _init();
  int status = main();
  exit(status);
}

#else

static inline rtems_task Init(rtems_task_argument argument) { exit(main()); }

#endif
