#include <stdio.h>
#include "appsupport.h"

int
main()
{
  int proc_id = get_proc_id();
  int proc_num = get_proc_num();
  printf("Proc %d of %d\n", proc_id, proc_num);
  exit(0);
}
