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

#if 1

/*
  The linker script should be able to properly setup the .init ELF
  section. Symbols _init and _fini should be provided by the linker
  also.
*/

extern "C" {

  typedef void (*voidfn)(void);

  static inline void
  _init()
  {
    extern voidfn ___CTOR_LIST__[];
    extern voidfn ___CTOR_END__[];
    for (voidfn* ptr = ___CTOR_END__ - 1; ptr != ___CTOR_LIST__; --ptr) {
      if (*ptr == 0 || *ptr == (voidfn)-1)
	continue;
#ifndef NDEBUG
      pr("Global ctor at", (unsigned)*ptr,
	 PR_CPU_ID | PR_STRING | PR_HEX | PR_NEWL);
#endif
      (*ptr)();
    }
  }

  static inline void
  _fini()
  {
    extern voidfn ___DTOR_LIST__[];
    extern voidfn ___DTOR_END__[];
    for (voidfn* ptr = ___DTOR_LIST__ + 1; ptr != ___DTOR_END__; ++ptr) {
      if (*ptr == 0 || *ptr == (voidfn)-1)
	continue;
#ifndef NDEBUG
      pr("Global dtor at", (unsigned)*ptr,
	 PR_CPU_ID | PR_STRING | PR_HEX | PR_NEWL);
#endif
      (*ptr)();
    }
  }
}

#else

extern "C" {
  void _init (void);
  void _fini (void);
}

#endif

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
