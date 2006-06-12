#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>

rtems_task Init(rtems_task_argument ignored)
{
  printf( "\n\n*** HELLO WORLD TEST ***\n" );
  printf( "Hello World\n" );
  printf( "*** END OF HELLO WORLD TEST ***\n" );
  exit( 0 );
}
