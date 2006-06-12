#ifndef ICEE_QUE_ACCEPTOR_H
#define ICEE_QUE_ACCEPTOR_H

#include <IceE/Acceptor.h>

namespace IceInternal
{

class Endpoint;

class ICE_API QueAcceptor : public ::IceInternal::Acceptor
{
public:

    virtual void close();
    virtual void listen();
    virtual TransceiverPtr accept();
    virtual void connectToSelf();
    virtual std::string toString() const;

private:

    QueAcceptor(const InstancePtr&, void*, int);
    virtual ~QueAcceptor();
    friend class QueEndpoint;

    InstancePtr _instance;
    TraceLevelsPtr _traceLevels;
    ::Ice::LoggerPtr _logger;
    void* _addr;
    int _sem;
};

}

#endif
