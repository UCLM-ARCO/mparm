#ifndef SYSTEM_H
#define SYSTEM_H
#include <rtems.h>

static inline rtems_task Init(rtems_task_argument ignored);

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

#include <confdefs.h>
//#include <rtems/confdefs.h>

#ifdef CONFIGURE_INIT
#include <stdlib.h>
extern int main();
static inline rtems_task Init(rtems_task_argument ignored) { exit(main()); }
#endif

#endif
