#include <Transports/QueTransceiver.h>
#include <IceE/Instance.h>
#include <IceE/TraceLevels.h>
#include <IceE/LoggerUtil.h>
#include <IceE/Buffer.h>
#include <IceE/LocalException.h>
#include <IceE/SafeStdio.h>
#include "appsupport.h"

using namespace std;
using namespace Ice;
using namespace IceInternal;

void
IceInternal::QueTransceiver::setTimeouts(int, int)
{
    // Ignore timeouts
}

void
IceInternal::QueTransceiver::close()
{
    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "closing que connection\n" << toString();
    }

    // FM: FIXME: close transceiver
}

void
IceInternal::QueTransceiver::shutdownWrite()
{
    if(_traceLevels->network >= 2)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "shutting down que connection for writing\n" << toString();
    }

    // FM: FIXME: Shutdown socket for writes
}

void
IceInternal::QueTransceiver::shutdownReadWrite()
{
    if(_traceLevels->network >= 2)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "shutting down que connection for reading and writing\n" << toString();
    }

    // FM: FIXME: Shutdown socket for reads/writes
}

void
IceInternal::QueTransceiver::writeWithTimeout(Buffer& buf, int timeout)
{
    Buffer::Container::difference_type packetSize = 
        static_cast<Buffer::Container::difference_type>(buf.b.end() - buf.i);
    
    while(buf.i != buf.b.end())
    {
	ssize_t ret = packetSize;

        // FM: FIXME: at least a ping pong buffer!!

	WAIT(_sem);
	time_start(packetSize);
	*(int*)_addr = packetSize;
	::memcpy(_addr, reinterpret_cast<const char*>(&*buf.i), packetSize);
	SIGNAL(_sem+1);
	time_stop(packetSize);
	
	if(ret == 0)
	{
	  ConnectionLostException ex(__FILE__, __LINE__);
	  ex.error = 0;
	  throw ex;
	}

	if(ret < 0)
	{
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = -1;
	    throw ex;
	}

	if(_traceLevels->network >= 3)
	{
	    Trace out(_logger, _traceLevels->networkCat);
	    out << Ice::printfToString("sent %d of %d", ret, packetSize) << " bytes via que\n" << toString();
	}

	buf.i += ret;

	if(packetSize > buf.b.end() - buf.i)
	{
	    packetSize = static_cast<Buffer::Container::difference_type>(buf.b.end() - buf.i);
	}
    }
}

void
IceInternal::QueTransceiver::readWithTimeout(Buffer& buf, int timeout)
{
    Buffer::Container::difference_type packetSize = 
	static_cast<Buffer::Container::difference_type>(buf.b.end() - buf.i);

    while(buf.i != buf.b.end())
    {
        // FM: FIXME: At least a ping pong buffer
	    
	WAIT(_sem+1);
	time_start(packetSize);
	ssize_t ret = *(int*)_addr;
	::memcpy(reinterpret_cast<char*>(&*buf.i),
		 const_cast<void*>(_addr),
		 ret);
	SIGNAL(_sem);
	time_stop(packetSize);

	if(ret == 0)
	{
	    //
	    // If the connection is lost when reading data, we shut
	    // down the write end of the socket. This helps to unblock
	    // threads that are stuck in send() or select() while
	    // sending data. Note: I don't really understand why
	    // send() or select() sometimes don't detect a connection
	    // loss. Therefore this helper to make them detect it.
	    //
		
	    ConnectionLostException ex(__FILE__, __LINE__);
	    ex.error = 0;
	    throw ex;
	}
	    
	if(ret < 0)
	{
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = -1;
	    throw ex;
	}
	    
	if(_traceLevels->network >= 3)
	{
	    Trace out(_logger, _traceLevels->networkCat);
	    out << Ice::printfToString("received %d of %d", ret, packetSize) << " bytes via que\n" << toString();
	}
	    
	buf.i += ret;
	    
	if(packetSize > buf.b.end() - buf.i)
	{
	    packetSize = static_cast<Buffer::Container::difference_type>(buf.b.end() - buf.i);
	}
    }
}

string
IceInternal::QueTransceiver::type() const
{
    return "que";
}

string
IceInternal::QueTransceiver::toString() const
{
    return _desc;
}

IceInternal::QueTransceiver::QueTransceiver(const InstancePtr& instance,
					    void* addr,
					    int sem) :
    _traceLevels(instance->traceLevels()),
    _logger(instance->logger()),
    _desc("SomethingSensible"),
    _addr(addr),
    _sem(sem)
{
    // FM: FIXME: transceiver initialization
}

IceInternal::QueTransceiver::~QueTransceiver()
{
    // FM: FIXME: transceiver finalization
}
