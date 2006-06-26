#include <MPS/Cond.h>
#include <MPS/Mutex.h>
#include <unistd.h>
#include "simctrl.h"

void
test_cond()
{
  pr("** Test Cond:", 0, PR_CPU_ID | PR_NEWL | PR_STRING);

  volatile int& flag = MPS::ShMem::memory_base[10];
  int id = get_proc_id() - 1;
  int n = get_proc_num();
  MPS::ShMem::Mutex mutex(5);
  bool failed = false;
  
  while (flag != id)
    sleep(0);

  {
    MPS::ShMem::Mutex::Lock lock(mutex);
    flag = id + 1;
    sleep(1);
    if (flag != id + 1) {
      pr("** Failed lock", flag,
	 PR_CPU_ID | PR_NEWL | PR_STRING | PR_DEC);
      failed = true;
    }
  }
  
  sleep(1);
  if (id + 1 == flag && id != n - 1) {
      pr("** Next deadlocked", flag,
	 PR_CPU_ID | PR_NEWL | PR_STRING | PR_DEC);
      failed = true;
  }

  if (failed)
    pr("Test Cond: FAILED", 0, PR_CPU_ID | PR_NEWL | PR_STRING);
  else
    pr("Test Cond: PASSED", 0, PR_CPU_ID | PR_NEWL | PR_STRING);

}

#ifndef COMBINE_TESTS

int
main()
{
  test_cond();

  stop_simulation();
}

#endif

