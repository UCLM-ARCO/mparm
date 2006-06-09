#include "appsupport.h"

int
main()
{
  printf("consumer: starting...\n");
  printf("consumer: proc_id  %d\n", get_proc_id());
  printf("consumer: proc_num %d\n", get_proc_num());
  exit(0);
}
