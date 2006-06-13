#define CONFIGURE_INIT
#include "mpsim.h"
#include "system.h"
#include <stdio.h>

using namespace std;

struct A {
  A(int i): _i(i) {
    pr("A ctor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  ~A() {
    pr("A dtor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  int _i;
};

A inBss(1);

int
main()
{
  A inStack(2);

  A* inHeap = new A(3);
  delete inHeap;

  return 0;
}
