#ifndef ICEE_TCP_CONNECTOR_H
#define ICEE_TCP_CONNECTOR_H

#include <IceE/Connector.h>

#ifdef _WIN32
#  include <winsock2.h>
#else
#  include <netinet/in.h> // For struct sockaddr_in
#endif

namespace IceInternal
{

class ICE_API TcpConnector : public ::IceInternal::Connector
{
public:
    
    virtual TransceiverPtr connect(int);
    virtual std::string toString() const;
    
private:
    
    TcpConnector(const InstancePtr&, const std::string&, int);
    virtual ~TcpConnector();
    friend class TcpEndpoint;

    InstancePtr _instance;
    TraceLevelsPtr _traceLevels;
    ::Ice::LoggerPtr _logger;
    struct sockaddr_in _addr;
};

}

#endif
