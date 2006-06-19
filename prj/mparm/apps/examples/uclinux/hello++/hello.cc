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


int
main()
{
  Hello app;
  app.doHello();
}
