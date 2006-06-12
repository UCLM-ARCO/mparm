#ifndef ICEE_TCP_TRANSCEIVER_H
#define ICEE_TCP_TRANSCEIVER_H

#include <IceE/Transceiver.h>
#include <IceE/TransceiverF.h>
#include <IceE/InstanceF.h>
#include <IceE/TraceLevelsF.h>
#include <IceE/LoggerF.h>

#ifdef _WIN32
#   include <winsock2.h>
typedef int ssize_t;
#else
#   define SOCKET int
#endif

namespace IceInternal
{

class ICE_API TcpTransceiver : public ::IceInternal::Transceiver
{
public:

    virtual void setTimeouts(int, int);

    SOCKET fd();
    virtual void close();
    virtual void shutdownWrite();
    virtual void shutdownReadWrite();
    virtual void writeWithTimeout(Buffer&, int);
    virtual void readWithTimeout(Buffer&, int);
    virtual void write(Buffer& buf)
    {
	writeWithTimeout(buf, _writeTimeout);
    }

    virtual void read(Buffer& buf)
    {
	readWithTimeout(buf, _readTimeout);
    }

    virtual std::string type() const;
    virtual std::string toString() const;

private:

    TcpTransceiver(const InstancePtr&, SOCKET);
    virtual ~TcpTransceiver();
    friend class TcpConnector;
    friend class TcpAcceptor;

#ifdef ICEE_USE_SELECT_FOR_TIMEOUTS
    void doSelect(bool, int);
#endif

    const TraceLevelsPtr _traceLevels;
    const Ice::LoggerPtr _logger;
    
    SOCKET _fd;
    int _readTimeout;
    int _writeTimeout;

#ifdef ICEE_USE_SELECT_FOR_TIMEOUTS
#ifdef _WIN32
    WSAEVENT _event;
    WSAEVENT _readEvent;
    WSAEVENT _writeEvent;
#else
    fd_set _wFdSet;
    fd_set _rFdSet;
#endif
#endif

    const std::string _desc;
#ifdef _WIN32
    const bool _isPeerLocal;
#endif
};

}

#endif
