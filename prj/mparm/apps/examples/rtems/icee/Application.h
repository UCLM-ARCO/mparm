#ifndef APPLICATION_H
#define APPLICATION_H

#include <IceE/IceE.h>

namespace Ice {
  class Application {
  public:
    Application() {}
    virtual ~Application() {}
    CommunicatorPtr communicator() { return _communicator; }
    virtual int run(int argc, char* argv[]) = 0;
    int main(int argc, char* argv[]);
  private:
    Ice::CommunicatorPtr _communicator;
  };
}

#endif
