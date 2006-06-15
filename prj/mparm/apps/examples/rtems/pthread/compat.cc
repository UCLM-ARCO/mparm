/*
  This file is needed for compliance with libstdc++ interface when
  using RTEMS 4.6.  This can be safely be removed as soon as RTEMS 4.7
  is ported to MPSIM.
 */
extern "C" {
  
  void rtems_gxx_mutex_init(void *mutex);
  int  rtems_gxx_mutex_lock(void *mutex);
  int  rtems_gxx_mutex_trylock(void *mutex);
  int  rtems_gxx_mutex_unlock(void *mutex);

  void rtems_gxx_recursive_mutex_init(void *mutex) {
    rtems_gxx_mutex_init(mutex);
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
