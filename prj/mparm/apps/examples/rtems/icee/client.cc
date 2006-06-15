#define CONFIGURE_INIT
#undef NDEBUG
#include "system.h"
#define NDEBUG
#include "Application.h"
#include "MyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include <IceE/StaticMutex.h>

using namespace std;

class Client: public Ice::Application {
  int run(int argc, char* argv[]) {
    char* mem = (char*)malloc(1024);
    
    pr("Create proxy...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    Ice::ObjectPrx base = communicator()->stringToProxy("obj1:que");

    pr("Unchecked cast...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    MPARM::MyObjPrx obj = MPARM::MyObjPrx::uncheckedCast(base->ice_oneway());

    pr("Alloc buffer...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    string buf(mem, 1024);

    pr("Send buffer...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    obj->send(buf);

    pr("Shutting down...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    obj->shutdown();

    pr("All done...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    return 0;
  }
};


int 
main()
{
  char* args[]={"client", 0};
  Client app;
  
  pr("Entering main...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  return app.main(1,args);
}
