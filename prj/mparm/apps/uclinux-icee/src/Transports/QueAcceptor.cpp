#include <Transports/QueAcceptor.h>
#include <Transports/QueTransceiver.h>
#include <IceE/Instance.h>
#include <IceE/TraceLevels.h>
#include <IceE/LoggerUtil.h>
#include <IceE/Exception.h>
#include "appsupport.h"

using namespace std;
using namespace Ice;
using namespace IceInternal;

void
IceInternal::QueAcceptor::close()
{
    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "stopping to accept que connections at " << toString();
    }
}

void
IceInternal::QueAcceptor::listen()
{
    // FM: FIXME: actually listen to connections
    SIGNAL(_sem);
    WAIT(_sem+2);

    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "accepting que connections at " << toString();
    }
}

TransceiverPtr
IceInternal::QueAcceptor::accept()
{
    // FM: FIXME: actually accept connections

    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "accepted que connection\n";
    }

    return new QueTransceiver(_instance, _addr, _sem);
}

void
IceInternal::QueAcceptor::connectToSelf()
{
    // FM: FIXME: actually connect to self
}

string
IceInternal::QueAcceptor::toString() const
{
    // FM: FIXME: actually return something useful
    return "SomethingUseful";
}

IceInternal::QueAcceptor::QueAcceptor(const InstancePtr& instance, void* addr, int sem) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->logger()),
    _addr(addr),
    _sem(sem)
{
    // FM: FIXME: initialize the thing
}

IceInternal::QueAcceptor::~QueAcceptor()
{
}
