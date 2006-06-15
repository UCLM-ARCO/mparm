#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>

class Hello {
public:
  Hello() {
    printf( "\n\n*** HELLO WORLD TEST (ctor) ***\n" );
  }
  ~Hello() {
    printf( "*** END OF HELLO WORLD TEST (dtor) ***\n" );
    exit(0);
  }
  void doHello() {
    printf( "Hello World (method)\n" );
  }
};


rtems_task Init(rtems_task_argument ignored)
{
  Hello app;
  app.doHello();
}
