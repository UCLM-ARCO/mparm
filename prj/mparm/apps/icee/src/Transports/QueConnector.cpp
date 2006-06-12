#include <Transports/QueConnector.h>
#include <Transports/QueTransceiver.h>
#include <IceE/Instance.h>
#include <IceE/TraceLevels.h>
#include <IceE/LoggerUtil.h>
#include <IceE/Exception.h>
#include "appsupport.h"

using namespace std;
using namespace Ice;
using namespace IceInternal;

TransceiverPtr
QueConnector::connect(int timeout)
{
    if(_traceLevels->network >= 2)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "trying to establish que connection to " << toString();
    }

    // FM: FIXME: actually connect
    SIGNAL(_sem+2); // triggers the acceptor

    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "que connection established\n";
    }

    return new QueTransceiver(_instance, _addr, _sem);
}

string
QueConnector::toString() const
{
    // FM: FIXME: textual representation of QueConnector
    return "UsefulRepresentationOfQueConnector";
}

QueConnector::QueConnector(const InstancePtr& instance, void* addr, int sem) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->logger()),
    _addr(addr),
    _sem(sem)
{
}

QueConnector::~QueConnector()
{
}
