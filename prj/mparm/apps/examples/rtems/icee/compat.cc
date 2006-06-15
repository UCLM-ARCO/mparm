#include "mpsim.h"

/*
  This file is needed for compliance with pthreads interface when
  using RTEMS 4.6.  This can be safely be removed as soon as RTEMS 4.7
  is ported to MPSIM.
 */
extern "C" {
  
  void rtems_gxx_mutex_init(void *mutex);
  int  rtems_gxx_mutex_lock(void *mutex);
  int  rtems_gxx_mutex_trylock(void *mutex);
  int  rtems_gxx_mutex_unlock(void *mutex);

  void rtems_gxx_recursive_mutex_init(void *mutex) {
    pr("rtems_gxx_recursive_mutex_init",
       (unsigned)mutex, PR_CPU_ID | PR_STRING | PR_NEWL);
    rtems_gxx_mutex_init(mutex);
    pr("End of rtems_gxx_recursive_mutex_init",
       (unsigned)mutex, PR_CPU_ID | PR_STRING | PR_NEWL);
  }

  int rtems_gxx_recursive_mutex_lock(void *mutex) {
    return rtems_gxx_mutex_lock(mutex);
  }

  int rtems_gxx_recursive_mutex_trylock(void *mutex) {
    return rtems_gxx_mutex_trylock(mutex);
  }

  int rtems_gxx_recursive_mutex_unlock(void *mutex) {
    return rtems_gxx_mutex_unlock(mutex);
  }
}
