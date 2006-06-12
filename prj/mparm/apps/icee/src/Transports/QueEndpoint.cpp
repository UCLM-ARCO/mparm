#include <Transports/QueEndpoint.h>
#include <Transports/QueConnector.h>
#include <IceE/Transceiver.h>
#include <IceE/BasicStream.h>
#include <IceE/LocalException.h>
#include <IceE/Instance.h>
#include <IceE/SafeStdio.h>
#ifndef ICEE_PURE_CLIENT
#   include <Transports/QueAcceptor.h>
#endif
#include "appsupport.h"

using namespace std;
using namespace Ice;
using namespace IceInternal;

IceInternal::QueEndpoint::QueEndpoint(const InstancePtr& instance, void* addr, int sem, bool pub) :
    _instance(instance),
    _addr(addr),
    _sem(sem),
    _publish(pub)
{
}

IceInternal::QueEndpoint::QueEndpoint(const InstancePtr& instance, const string& str) :
    _instance(instance),
    _addr(reinterpret_cast<void*>(SHARED_BASE+USER_SHM_OFFSET)),
    _sem(0),
    _publish(true)
{
    const string delim = " \t\n\r";

    string::size_type beg;
    string::size_type end = 0;

    while(true)
    {
	beg = str.find_first_not_of(delim, end);
	if(beg == string::npos)
	{
	    break;
	}
	
	end = str.find_first_of(delim, beg);
	if(end == string::npos)
	{
	    end = str.length();
	}

	string option = str.substr(beg, end - beg);
	if(option.length() != 2 || option[0] != '-')
	{
	    EndpointParseException ex(__FILE__, __LINE__);
	    ex.str = "que " + str;
	    throw ex;
	}

	string argument;
	string::size_type argumentBeg = str.find_first_not_of(delim, end);
	if(argumentBeg != string::npos && str[argumentBeg] != '-')
	{
	    beg = argumentBeg;
	    end = str.find_first_of(delim, beg);
	    if(end == string::npos)
	    {
		end = str.length();
	    }
	    argument = str.substr(beg, end - beg);
	}

	switch(option[1])
	{
	    // FM: FIXME: Put sensible args here
	    case 'a':
	    {
	    	const_cast<void*&>(_addr)
		  = reinterpret_cast<void*>(SHARED_BASE + atoi(argument.c_str()));
		break;
	    }
	    case 's':
	    {
	    	const_cast<Int&>(_sem) = atoi(argument.c_str());
		break;
	    }
	    default:
	    {
		EndpointParseException ex(__FILE__, __LINE__);
		ex.str = "que " + str;
		throw ex;
	    }
	}
    }
}

IceInternal::QueEndpoint::QueEndpoint(BasicStream* s) :
    _instance(s->instance()),
    _addr(0),
    _sem(0),
    _publish(true)
{
    s->startReadEncaps();

    // FM: FIXME: complete set of params
    int offset;
    s->read(offset);
    const_cast<void*&>(_addr) = reinterpret_cast<void*>(SHARED_BASE + offset);
    s->read(const_cast<Int&>(_sem));

    s->endReadEncaps();
}

void
IceInternal::QueEndpoint::streamWrite(BasicStream* s) const
{
    s->write(QueEndpointType);
    s->startWriteEncaps();

    // FM: FIXME: complete set of params
    s->write(reinterpret_cast<int>(_addr) - SHARED_BASE);
    s->write(_sem);

    s->endWriteEncaps();
}

string
IceInternal::QueEndpoint::toString() const
{
    string s;
    s += "que ";

    // FM: FIXME: complete set of params
    s += Ice::printfToString(" -a %d -s %d",
			     reinterpret_cast<int>(_addr) - SHARED_BASE, _sem);

    return s;
}

Short
IceInternal::QueEndpoint::type() const
{
    return QueEndpointType;
}

Int
IceInternal::QueEndpoint::timeout() const
{
    return -1;
}

EndpointPtr
IceInternal::QueEndpoint::timeout(Int) const
{
    // Ignore timeouts
    return 0;
}

bool
IceInternal::QueEndpoint::unknown() const
{
    return false;
}

ConnectorPtr
IceInternal::QueEndpoint::connector() const
{
    // FM: FIXME: Add params
    return new QueConnector(_instance, const_cast<void*>(_addr), _sem);
}

bool
IceInternal::QueEndpoint::operator==(const Endpoint& r) const
{
    const QueEndpoint* p = dynamic_cast<const QueEndpoint*>(&r);
    assert(p);

    if(this == p)
    {
	return true;
    }

    // FM: FIXME: Complete set of params
    if(_addr != p->_addr)
    {
	return false;
    }

    return true;
}

bool
IceInternal::QueEndpoint::operator!=(const Endpoint& r) const
{
    return !operator==(r);
}

bool
IceInternal::QueEndpoint::operator<(const Endpoint& r) const
{
    const QueEndpoint* p = dynamic_cast<const QueEndpoint*>(&r);
    assert(p);

    if(this == p)
    {
	return false;
    }

    // FM: FIXME: Complete set of params
    if(_addr < p->_addr)
    {
	return true;
    }
    else if(p->_addr < _addr)
    {
	return false;
    }

    return false;
}

vector<EndpointPtr>
IceInternal::QueEndpoint::expand(bool includeLoopback) const
{
    vector<EndpointPtr> v;
    return v; // Must configure address of all QueEndpoints
}

#ifndef ICEE_PURE_CLIENT

AcceptorPtr
IceInternal::QueEndpoint::acceptor(EndpointPtr& endp) const
{
    // FM: FIXME: Complete set of params
    Acceptor* p = new QueAcceptor(_instance, const_cast<void*>(_addr), _sem);
    endp = new QueEndpoint(_instance, const_cast<void*>(_addr), _sem, _publish);
    return p;
}

bool
IceInternal::QueEndpoint::publish() const
{
    return _publish;
}

#endif
