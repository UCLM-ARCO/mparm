#include "mpsim.h"

int main() {
  pr("Hello, World!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  stop_simulation();
  return 0;
}
