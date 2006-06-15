#include "Application.h"
#include "mpsim.h"

int
Ice::Application::main(int argc, char* argv[])
{
  int status;
  try {
    pr("Create communicator...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    _communicator = Ice::initialize(argc, argv);
    pr("Entering run...", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    status= run(argc, argv);
  }
  catch(const Ice::Exception& ex) {
    fprintf(stderr, "%s\n", ex.toString().c_str());
    status = -1;
  }
  return status;
}
