#include "mpsim.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void*
thread(void* id)
{
  pr("Entered thread...", (unsigned)id,
     PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);

  int i, j=64*1024, k;
  char* chunks[DRAM_SIZE/j];

  pr("Allocating chunks in thread ", (unsigned)id,
     PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);

  for (i=0; i < DRAM_SIZE/j; ++i) {
    chunks[i] = (char*)malloc(j - 4); /* 4 bytes used by malloc */
    sleep(0);
    if (NULL == chunks[i]) {
      pr("No more memory", ((unsigned)id)*100000 + i,
	 PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
      break;
    }
    for (k=0; k<j; k+=1000)
      chunks[i][k] = '\0';
  }

  for (k=0; k<i; ++k)
    free(chunks[k]);
  
  return 0;
}

int 
main()
{
  pr("Entering main...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);

  int status;
  pthread_t th1, th2;

  status = pthread_create( &th1, 0, thread, (void*)1 );
  assert( !status );

  status = pthread_create( &th2, 0, thread, (void*)2 );
  assert( !status );

  pthread_join(th1, 0);
  pthread_join(th2, 0);

  pr("All done...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  return 0;
}
