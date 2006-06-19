#include "mpsim.h"
#include <stdlib.h>

struct X {
  X() {}
  ~X(){ stop_simulation(); }
};

static X x;
