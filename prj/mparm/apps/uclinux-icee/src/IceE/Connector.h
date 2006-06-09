// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_CONNECTOR_H
#define ICEE_CONNECTOR_H

#include <IceE/ConnectorF.h>
#include <IceE/TransceiverF.h>
#include <IceE/InstanceF.h>
#include <IceE/TraceLevelsF.h>
#include <IceE/LoggerF.h>
#include <IceE/Shared.h>

namespace IceInternal
{

class Endpoint;

class ICE_API Connector : public ::IceUtil::Shared
{
public:
    
    virtual TransceiverPtr connect(int) = 0;
    virtual std::string toString() const = 0;
    
protected:
    virtual ~Connector() { }
};

}

inline void IceInternal::incRef(Connector* p) { p->__incRef(); }
inline void IceInternal::decRef(Connector* p) { p->__decRef(); }

#endif
