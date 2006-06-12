// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_TRANSPORT_QUE_ENDPOINT_H
#define ICEE_TRANSPORT_QUE_ENDPOINT_H

#include <IceE/Endpoint.h>

namespace IceInternal
{

const Ice::Short QueEndpointType = 1;

class QueEndpoint : public IceInternal::Endpoint
{
public:

    QueEndpoint(const InstancePtr& instance, void* addr, int sem, bool pub);
    QueEndpoint(const InstancePtr&, const std::string&);
    QueEndpoint(BasicStream*);

    virtual void streamWrite(BasicStream*) const;
    virtual std::string toString() const;
    virtual Ice::Short type() const;
    virtual Ice::Int timeout() const;
    virtual EndpointPtr timeout(Ice::Int) const;
    virtual bool unknown() const;
    virtual ConnectorPtr connector() const;
#ifndef ICEE_PURE_CLIENT
    virtual AcceptorPtr acceptor(EndpointPtr&) const;
    virtual bool publish() const;
#endif
    virtual std::vector<EndpointPtr> expand(bool) const;

    virtual bool operator==(const Endpoint&) const;
    virtual bool operator!=(const Endpoint&) const;
    virtual bool operator<(const Endpoint&) const;

private:

    //
    // All members are const, because endpoints are immutable.
    // FM: FIXME: Complete set of members
    //
    const InstancePtr _instance;
    const void* _addr;
    const int _sem;
    const bool _publish;
};

}

#endif
