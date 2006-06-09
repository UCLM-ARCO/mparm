#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

extern "C" {
#  include "appsupport.h"
}

int
main()
{
  int proc_id = get_proc_id();
  int proc_num = get_proc_num();

  unsigned* data = (unsigned*)(SHARED_BASE + USER_SHM_OFFSET);
  
  if (proc_id & 1) {
    printf("Proc %d, sending data...\n", proc_id);
    start_metric();
    for (unsigned sz=1; sz<2048 ; sz=sz*2) {
      time_start(sz);
      for (int j=0; j<256; ++j)
	data[j] = sz;
      SIGNAL(1);
      time_stop(sz);
      WAIT(0);
    }
    stop_metric();
  }
  else {
    TEST_AND_SET(0);
    TEST_AND_SET(1);
    printf("Proc %d, receiving data...\n", proc_id);
    for (int sz=1; sz<2048 ; sz=sz*2) {
      WAIT(1);
      time_start(sz);
      for (int j=0; j<256; ++j)
	if (data[j] != sz) {
	  printf("ERROR(%d,%d)\n", sz, j);
	  break;
	}
      SIGNAL(0);
      time_stop(sz);
    }
  }
  
  exit(0);
}
