#ifndef ICEE_QUE_TRANSCEIVER_H
#define ICEE_QUE_TRANSCEIVER_H

#include <IceE/Transceiver.h>
#include <IceE/TransceiverF.h>
#include <IceE/InstanceF.h>
#include <IceE/TraceLevelsF.h>
#include <IceE/LoggerF.h>

namespace IceInternal
{

class ICE_API QueTransceiver : public ::IceInternal::Transceiver
{
public:

    virtual void setTimeouts(int, int);
    virtual void close();
    virtual void shutdownWrite();
    virtual void shutdownReadWrite();
    virtual void writeWithTimeout(Buffer&, int);
    virtual void readWithTimeout(Buffer&, int);
    virtual void write(Buffer& buf)
    {
	writeWithTimeout(buf, -1);
    }

    virtual void read(Buffer& buf)
    {
	readWithTimeout(buf, -1);
    }

    virtual std::string type() const;
    virtual std::string toString() const;

private:

    QueTransceiver(const InstancePtr&, void*, int);
    virtual ~QueTransceiver();
    friend class QueConnector;
    friend class QueAcceptor;

    const TraceLevelsPtr _traceLevels;
    const Ice::LoggerPtr _logger;
    
    const std::string _desc;
    void* _addr;
    int _sem;
};

}

#endif
