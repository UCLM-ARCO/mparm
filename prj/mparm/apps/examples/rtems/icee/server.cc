#define CONFIGURE_INIT
#include "system.h"
#include "mpsim.h"
#include "Application.h"
#include "MyObj.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class MyObjI: public MPARM::MyObj {
public:
  virtual void send(const string& str, const Ice::Current&) const {
    pr("Send called!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  
  virtual void shutdown(const Ice::Current&) {
    pr("Shutdown called!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    stop_simulation();
  }
};


class Server: public Ice::Application {
  int run(int argc, char* argv[]) {
    pr("Create adapter...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    Ice::ObjectAdapterPtr adapter
      = communicator()->createObjectAdapter("MyAdapter");

    pr("Create servant...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    adapter->add(new MyObjI(), Ice::stringToIdentity("obj1"));

    pr("Activate adapter...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    adapter->activate();

    pr("Waiting for invocations...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    communicator()->waitForShutdown();

    pr("All done...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    return 0;
  }
};

int
main()
{
  char* args[]={"server", 0};
  Server app;
  
  pr("Entering main...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  return app.main(1, args);
}
