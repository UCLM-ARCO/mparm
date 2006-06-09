// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Transports/TcpConnector.h>
#include <Transports/TcpTransceiver.h>
#include <Transports/Network.h>
#include <IceE/Instance.h>
#include <IceE/TraceLevels.h>
#include <IceE/LoggerUtil.h>
#include <IceE/Exception.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

TransceiverPtr
TcpConnector::connect(int timeout)
{
    if(_traceLevels->network >= 2)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "trying to establish tcp connection to " << toString();
    }

    SOCKET fd = createSocket();
    setBlock(fd, false);
    doConnect(fd, _addr, timeout);
#ifndef ICEE_USE_SELECT_FOR_TIMEOUTS
    setBlock(fd, true);
#endif

    if(_traceLevels->network >= 1)
    {
	Trace out(_logger, _traceLevels->networkCat);
	out << "tcp connection established\n" << fdToString(fd);
    }

    return new TcpTransceiver(_instance, fd);
}

string
TcpConnector::toString() const
{
    return addrToString(_addr);
}

TcpConnector::TcpConnector(const InstancePtr& instance, const string& host, int port) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->logger())
{
    getAddress(host, port, _addr);
}

TcpConnector::~TcpConnector()
{
}
