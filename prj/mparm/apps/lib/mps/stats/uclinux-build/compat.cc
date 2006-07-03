#include "simctrl.h"

struct X {
  X() {}
  ~X(){ stop_simulation(); }
};

static X x;
