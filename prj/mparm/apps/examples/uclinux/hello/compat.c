#include "mpsim.h"
#include <stdlib.h>

void f() __attribute__((constructor));

void f() { atexit(stop_simulation); }
