#include "mpsim.h"
#include <stdio.h>

using namespace std;

void f() { throw "Exception caught"; }

struct A {
  A(int i): _i(i) {
    pr("A ctor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  void excep() {
    f();
  }
  ~A() {
    pr("A dtor...", _i, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  }
  int _i;
};

int
main()
{
  try {
    A inStack(1);
    inStack.excep();
    pr("Never should be printed", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  catch (const char* str) {
    pr((char*)str, 0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  return 0;
}
