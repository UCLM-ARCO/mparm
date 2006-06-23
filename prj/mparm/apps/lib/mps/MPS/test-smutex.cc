#include <MPS/SMutex.h>
#include <unistd.h>
#include "simctrl.h"

void*
smutex_thread(void* th_id)
{
  int tid = reinterpret_cast<int>(th_id);
  
  pr("** Test SMutex:", tid,
     PR_CPU_ID | PR_NEWL | PR_STRING | PR_HEX);

  volatile int& flag = MPS::ShMem::memory_base[10];
  int id = get_proc_id() - 1;
  int n = get_proc_num();
  MPS::ShMem::SMutex mutex(5);
  bool failed = false;
  
  while (flag & 0xf < id)
    sleep(0);

  int v;
  
  {
    MPS::ShMem::SMutex::Lock lock(mutex);
    v = flag & 0xf;
    if (v <= id)
      flag = (id + 1) | tid;
    else
      flag = v | tid;

    v = flag;
    sleep(1);
    if (flag != v) {
      pr("** Failed lock", (v << 8) | flag,
	 PR_CPU_ID | PR_NEWL | PR_STRING | PR_HEX);
      failed = true;
    }
  }
  
  sleep(2);
  if (v == flag && id != n - 1) {
      pr("** Next deadlocked", tid,
	 PR_CPU_ID | PR_NEWL | PR_STRING | PR_HEX);
      failed = true;
  }

  if (failed)
    pr("Test SMutex: FAILED", tid,
       PR_CPU_ID | PR_NEWL | PR_STRING | PR_HEX);
  else
    pr("Test SMutex: PASSED", tid,
       PR_CPU_ID | PR_NEWL | PR_STRING | PR_HEX);

  return 0;
}

void
test_smutex()
{
  int status;
  pthread_t th1, th2;

  status = pthread_create( &th1, 0, smutex_thread, (void*)0x10 );
  assert( !status );

  status = pthread_create( &th2, 0, smutex_thread, (void*)0x20 );
  assert( !status );

  pthread_join(th1, 0);
  pthread_join(th2, 0);
}

#ifndef COMBINE_TESTS

int
main()
{
  test_smutex();

  stop_simulation();
}

#endif

