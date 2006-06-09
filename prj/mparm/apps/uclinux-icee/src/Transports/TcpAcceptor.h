#ifndef ICEE_TCP_ACCEPTOR_H
#define ICEE_TCP_ACCEPTOR_H

#include <IceE/Acceptor.h>

#ifdef _WIN32
#   include <winsock2.h>
typedef int ssize_t;
#else
#   define SOCKET int
#   include <netinet/in.h> // For struct sockaddr_in
#endif

namespace IceInternal
{

class Endpoint;

class ICE_API TcpAcceptor : public ::IceInternal::Acceptor
{
public:

    SOCKET fd();
    virtual void close();
    virtual void listen();
    virtual TransceiverPtr accept();
    virtual void connectToSelf();
    virtual std::string toString() const;

    int effectivePort();

private:

    TcpAcceptor(const InstancePtr&, const std::string&, int);
    virtual ~TcpAcceptor();
    friend class TcpEndpoint;

    InstancePtr _instance;
    TraceLevelsPtr _traceLevels;
    ::Ice::LoggerPtr _logger;
    SOCKET _fd;
    int _backlog;
    struct sockaddr_in _addr;
};

}

#endif
