#define CONFIGURE_INIT
#include "mpsim.h"
#include "system.h"
#include <stdio.h>

using namespace std;

struct B {
  B() {
    pr("B ctor...", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  ~B() {
    pr("B dtor...", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
};

struct A {
  A(int i): _i(i) {
    pr("A ctor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  ~A() {
    pr("A dtor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  int _i;
  static B b;
};

A global(1);
B A::b;

int
main()
{
  A inStack(2);

  A* inHeap = new A(3);
  delete inHeap;

  return 0;
}
