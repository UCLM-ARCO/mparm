// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_ACCEPTOR_H
#define ICEE_ACCEPTOR_H

#include <IceE/TransceiverF.h>
#include <IceE/InstanceF.h>
#include <IceE/TraceLevelsF.h>
#include <IceE/LoggerF.h>
#include <IceE/AcceptorF.h>

#include <IceE/Shared.h>

namespace IceInternal
{

class Endpoint;

class ICE_API Acceptor : public ::IceUtil::Shared
{
public:
    virtual void close() = 0;
    virtual void listen() = 0;
    virtual TransceiverPtr accept() = 0;
    virtual void connectToSelf() = 0;
    virtual std::string toString() const = 0;

protected:
    virtual ~Acceptor() { }
};

}

inline void IceInternal::incRef(Acceptor* p) { p->__incRef(); }
inline void IceInternal::decRef(Acceptor* p) { p->__decRef(); }

#endif
