// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_TRANSCEIVER_H
#define ICEE_TRANSCEIVER_H

#include <IceE/TransceiverF.h>
#include <IceE/Shared.h>

namespace IceInternal
{

class Buffer;

class ICE_API Transceiver : public ::IceUtil::Shared
{
public:

    virtual void setTimeouts(int, int) = 0;

    virtual void close() = 0;
    virtual void shutdownWrite() = 0;
    virtual void shutdownReadWrite() = 0;
    virtual void writeWithTimeout(Buffer&, int) = 0;
    virtual void readWithTimeout(Buffer&, int) = 0;
    virtual void write(Buffer& buf) = 0;
    virtual void read(Buffer& buf) = 0;

    virtual std::string type() const = 0;
    virtual std::string toString() const = 0;

protected:
    virtual ~Transceiver() { }
};

}

inline void IceInternal::incRef(Transceiver* p) { p->__incRef(); }
inline void IceInternal::decRef(Transceiver* p) { p->__decRef(); }

#endif
