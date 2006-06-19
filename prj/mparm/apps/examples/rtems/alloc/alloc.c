#include "mpsim.h"
#include <stdio.h>
#include <stdlib.h>

#define NDEBUG

#ifndef NDEBUG
# define DEBUGP(x) do { printf x ; fflush(stdout); } while(0)
#else
# define DEBUGP(x)
#endif

int
main()
{
  int i, j, k;

  start_metric();

  for (j=4*1024*1024; j>=4096; j=j/2) {
    char* chunks[DRAM_SIZE/j];

    pr("Allocating chunks of size", j,
       PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);

    for (i=0; i < DRAM_SIZE/j; ++i) {
      DEBUGP(("\r  %d\t", i));
      chunks[i] = malloc(j - 4); /* 4 bytes used by malloc */
      if (NULL == chunks[i]) {
	pr("No more memory", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
	break;
      }

      DEBUGP(("Writing..."));
      for (k=0; k<j; k+=1000)
	chunks[i][k] = '\0';
      DEBUGP(("\r  %d\t                    ", i));
    }

    for (k=0; k<i; ++k)
      free(chunks[k]);
  }
  stop_metric();
}
