#ifndef ICEE_QUE_CONNECTOR_H
#define ICEE_QUE_CONNECTOR_H

#include <IceE/Connector.h>

namespace IceInternal
{

class ICE_API QueConnector : public ::IceInternal::Connector
{
public:
    
    virtual TransceiverPtr connect(int);
    virtual std::string toString() const;
    
private:

    QueConnector(const InstancePtr&, void* addr, int sem);
    virtual ~QueConnector();
    friend class QueEndpoint;

    InstancePtr _instance;
    TraceLevelsPtr _traceLevels;
    ::Ice::LoggerPtr _logger;
    void* _addr;
    int _sem;
};

}

#endif
