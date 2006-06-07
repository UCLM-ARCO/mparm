// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI API Types
//    - Classes and types which are visible to the external AXI API
//    - Use this header file in all AXI application
// ============================================================================

#ifndef _AXI_TYPES_H
#define _AXI_TYPES_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <new>
#include <math.h>

#include "systemc.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

// max number of masters/slave attached to an IC
const int AXI_MAX_PORT_SIZE = 32;

typedef unsigned int axi_addr_t;
typedef unsigned int  axi_data_bt;
typedef axi_data_bt* axi_data_t;
typedef unsigned int axi_wstrb_bt;
typedef axi_wstrb_bt* axi_wstrb_t;


// ----------------------------------------------------------------------------
//  Type for the ABURST signal.
// ----------------------------------------------------------------------------
typedef enum {
	ABURST_FIXED = 0,
	ABURST_INCR  = 1,
	ABURST_WRAP  = 2
} axi_aburst_t;

// ----------------------------------------------------------------------------
//  Type for the RRESP signal.
// ----------------------------------------------------------------------------
typedef enum {
	RESP_OKAY   = 0,
	RESP_EXOKAY = 1,
	RESP_SLVERR = 2,
	RESP_DECERR = 3
} axi_resp_t;

// ----------------------------------------------------------------------------
//  Type for the ALOCK signal.
// ----------------------------------------------------------------------------
typedef enum {
	ALOCK_NORMAL    = 0,
	ALOCK_EXCLUSIVE = 1,
	ALOCK_LOCKED    = 2,
	ALOCK_NO        = 3
} axi_alock_t;

// ----------------------------------------------------------------------------
//  Type for the ASIZE signal.
// ----------------------------------------------------------------------------
typedef enum {
	ASIZE_8    = 0,
	ASIZE_16   = 1,
	ASIZE_32   = 2,
	ASIZE_64   = 3,
	ASIZE_128  = 4,
	ASIZE_256  = 5,
	ASIZE_512  = 6,
	ASIZE_1024 = 7
} axi_asize_t;


class Achannel
{
public:

Achannel() { // constructor
  m_NumberBytes      = 0;
  m_BurstLength      = 0;
  m_AlignedAddress   = 0;
  m_TotalNumberBytes = 0;
  m_WrapBoundary     = 0;
  Reset(); // initialize
}

~Achannel(){}  // destructor

void Reset() // default values
{
  Avalid = false;
  Addr   = 0;
  Awrite = false;
  Alen   = 0;
  Asize  = ASIZE_32;
  Aburst = ABURST_INCR;
  Alock  = ALOCK_NORMAL;
  Acache = 0;
  Aprot  = 0;
  AID    = 0;
}

// Compute the address from the burst
axi_addr_t GetAddressN(int DataBusBytes, int N, 
                       int &LowerOffset, int &UpperOffset)
{
  axi_addr_t AddressN;
  if (N == 1) // start of a new burst
  {
    m_NumberBytes = 1 << Asize;
    switch(Aburst) {
      case ABURST_FIXED:
        AddressN = Addr; break;
      case ABURST_INCR:
        m_AlignedAddress = (Addr >> Asize) << Asize;
        AddressN = m_AlignedAddress; break;
      case ABURST_WRAP:
        m_BurstLength    = Alen + 1;
        m_AlignedAddress = (Addr >> Asize) << Asize;
        m_TotalNumberBytes = m_NumberBytes * m_BurstLength;
        m_WrapBoundary   = (Addr / m_TotalNumberBytes) * m_TotalNumberBytes;
        AddressN = m_AlignedAddress; break;
      default: AddressN = 0;
       cerr << "ERROR in address computation: Aburst hast invalid value"
            << endl; sc_assert(0); break;
    } // end of cases
  } // end of first address computation
  else if (N > 1) {
    switch(Aburst) {
      case ABURST_FIXED:
        AddressN = Addr; break;
      case ABURST_INCR:
        AddressN = m_AlignedAddress + (N - 1) * m_NumberBytes; break;
      case ABURST_WRAP:
        AddressN = m_AlignedAddress + (N - 1) * m_NumberBytes;
//        if (AddressN == (m_WrapBoundary + m_TotalNumberBytes))
//          AddressN = m_WrapBoundary; break;
        if (AddressN >= (m_WrapBoundary + m_TotalNumberBytes))
          AddressN -= m_TotalNumberBytes; break;
      default: AddressN = 0;
       cerr << "ERROR in address computation: Aburst hast invalid value"
            << endl; sc_assert(0); break;
    } // end of cases
  } else { // end of n-th address computation
    cerr << "ERROR in address computation: Beat number 0 is not allwed";
    cerr << "                              First beat number must be ONE"
         << endl; sc_assert(0);
  }
  LowerOffset = AddressN - (AddressN / DataBusBytes) * DataBusBytes;
  UpperOffset = LowerOffset + m_NumberBytes - 1;
  return(AddressN);
}
axi_addr_t GetAddressN(int N)
{
  axi_addr_t AddressN = 0;
  if (N == 1) // start of a new burst
  {
    m_NumberBytes = 1 << Asize;
    switch(Aburst) {
      case ABURST_FIXED:
        AddressN = Addr; break;
      case ABURST_INCR:
        m_AlignedAddress = (Addr >> Asize) << Asize;
        AddressN = m_AlignedAddress; break;
      case ABURST_WRAP:
        m_BurstLength    = Alen + 1;
        m_AlignedAddress = (Addr >> Asize) << Asize;
        m_TotalNumberBytes = m_NumberBytes * m_BurstLength;
        m_WrapBoundary   = (Addr / m_TotalNumberBytes) * m_TotalNumberBytes;
        AddressN = m_AlignedAddress; break;
      default: AddressN = 0;
       cerr << "ERROR in address computation: Aburst hast invalid value"
            << endl; sc_assert(0); break;
    } // end of cases
  } // end of first address computation
  else if (N > 1) {
    switch(Aburst) {
      case ABURST_FIXED:
        AddressN = Addr; break;
      case ABURST_INCR:
        AddressN = m_AlignedAddress + (N - 1) * m_NumberBytes; break;
      case ABURST_WRAP:
        AddressN = m_AlignedAddress + (N - 1) * m_NumberBytes;
//        if (AddressN == (m_WrapBoundary + m_TotalNumberBytes))
//          AddressN = m_WrapBoundary; break;
        if (AddressN >= (m_WrapBoundary + m_TotalNumberBytes))
          AddressN -= m_TotalNumberBytes; break;
      default: AddressN = 0;
       cerr << "ERROR in address computation: Aburst hast invalid value"
            << endl; sc_assert(0); break;
    } // end of cases
  } else { // end of n-th address computation
    cerr << "ERROR in address computation: Beat number 0 is not allwed";
    cerr << "                              First beat number must be ONE"
         << endl; sc_assert(0);
  }
  return(AddressN);
}

// Overload the copy operator.
Achannel& operator = (const Achannel& B)
{
  Avalid = B.Avalid;
  Addr   = B.Addr;
  Awrite = B.Awrite;
  Alen   = B.Alen;
  Asize  = B.Asize;
  Aburst = B.Aburst;
  Alock  = B.Alock;
  Acache = B.Acache;
  Aprot  = B.Aprot;
  AID    = B.AID;
  return(*this);
}

  bool Avalid;
  axi_addr_t Addr;
  bool Awrite;
  int Alen;
  axi_asize_t Asize;
  axi_aburst_t Aburst;
  axi_alock_t Alock;
  int Acache;
  int Aprot;
  int AID;

private:
  int m_NumberBytes;
  int m_BurstLength;
  axi_addr_t m_AlignedAddress;
  int m_TotalNumberBytes;
  axi_addr_t m_WrapBoundary;
};

class Wchannel
{
public:

Wchannel(int DataBusBytes, int DataBusWords) { // constructor
  WdataBusBytes = DataBusBytes;
  WdataBusWords = (DataBusWords < 1) ? 1 : DataBusWords;
  WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  for (int i = 0; i < 32; i++) Wdata[i] = 0;
  for (int i = 0; i < 4; i++) Wstrb[i] = ~0;
  Reset(); // initialize
}

~Wchannel(){}  // destructor

void Reset() // default values
{
  Wvalid = false;
  Wlast  = false;
  WID    = 0;
}

// Overload the copy operator.
Wchannel& operator = (const Wchannel& B)
{
  Wvalid = B.Wvalid;
  Wlast  = B.Wlast;
  for (int i = 0; i < WdataBusWords; i++) Wdata[i] = B.Wdata[i];
  for (int i = 0; i < WstrbWords; i++) Wstrb[i] = B.Wstrb[i];
  WID    = B.WID;
  return(*this);
}
  int WdataBusBytes;
  int WdataBusWords;
  int WstrbWords;
  bool Wvalid;
  bool Wlast;
  axi_data_bt Wdata[32];
  axi_wstrb_bt Wstrb[4];
  int WID;
};

class Rchannel
{
public:

Rchannel(int DataBusBytes, int DataBusWords) { // constructor
  RdataBusBytes = DataBusBytes;
  RdataBusWords = (DataBusWords < 1) ? 1 : DataBusWords;
  for (int i = 0; i < 32; i++) Rdata[i] = 0;
  Reset(); // initialize
}

~Rchannel(){}  // destructor

void Reset() // default values
{
  Rvalid = false;
  Rlast  = false;
  Rresp  = RESP_OKAY;
  RID    = 0;
}

// Overload the copy operator.
Rchannel& operator = (const Rchannel& B)
{
  Rvalid = B.Rvalid;
  Rlast  = B.Rlast;
  for (int i = 0; i < RdataBusWords; i++) Rdata[i] = B.Rdata[i];
  Rresp  = B.Rresp;
  RID    = B.RID;

  return(*this);
}
  int RdataBusBytes;
  int RdataBusWords;
  bool Rvalid;
  bool Rlast;
  axi_data_bt Rdata[32];
  axi_resp_t Rresp;
  int RID;
};

class Bchannel
{
public:

Bchannel() { // constructor
  Reset(); // initialize
}

~Bchannel(){}  // destructor

void Reset() // default values
{
  Bvalid = false;
  Bresp  = RESP_OKAY;
  BID    = 0;
}

// Overload the copy operator.
Bchannel& operator = (const Bchannel& B)
{
  Bvalid = B.Bvalid;
  Bresp  = B.Bresp;
  BID    = B.BID;
  return(*this);
}
  bool Bvalid;
  axi_resp_t Bresp;
  int BID;
};

class AreadySig
{
public:
AreadySig(){ Reset();}
~AreadySig(){}
void Reset(){ Aready = false;}
AreadySig& operator = (const AreadySig& B){ Aready = B.Aready; return(*this);}
bool Aready;
};
class WreadySig
{
public:
WreadySig(){ Reset();}
~WreadySig(){}
void Reset(){ Wready = false;}
WreadySig& operator = (const WreadySig& B){ Wready = B.Wready; return(*this);}
bool Wready;
};
class RreadySig
{
public:
RreadySig(){ Reset();}
~RreadySig(){}
void Reset(){ Rready = false;}
RreadySig& operator = (const RreadySig& B){ Rready = B.Rready; return(*this);}
bool Rready;
};
class BreadySig
{
public:
BreadySig(){ Reset();}
BreadySig(int a, int b){ Reset();}
~BreadySig(){}
void Reset(){ Bready = false;}
BreadySig& operator = (const BreadySig& B){ Bready = B.Bready; return(*this);}
bool Bready;
};


// ------------------------------------------------------------------------
//  Class : AXI_AddressRegion
//
//  Struct containing address regions of slaves.
// ------------------------------------------------------------------------

class AXI_AddressRegion
{
public:
  int SlaveID;
  axi_addr_t StartAddr;
  axi_addr_t EndAddr;

  // single address map
  AXI_AddressRegion(int iSlaveID,
                    axi_addr_t iStartAddr,
                    axi_addr_t iEndAddr)
                   : SlaveID   (iSlaveID)
                   , StartAddr (iStartAddr)
                   , EndAddr   (iEndAddr)
  {};
};

// ------------------------------------------------------------------------
//  Class : axi address map
//
//  Simple vector class for address regions.
// ------------------------------------------------------------------------

class AXI_AddressMap
{
public:
  AXI_AddressMap(int alloc = 16)
  	: m_MaxSize  (0)
  	, m_CurrSize (0)
        , m_DefaultSlave (-1)
  	, m_Data     (0)
  {
    Resize(alloc);
  }

  ~AXI_AddressMap()
  {
    for(int i = 0; i < m_CurrSize; i++) delete m_Data[i];
    if (m_Data) delete[] m_Data;
  }

  void PushBack(const AXI_AddressRegion& AddrRegion)
  {
    if (m_CurrSize == m_MaxSize) Resize(m_MaxSize + 16);
    m_Data[m_CurrSize] = new AXI_AddressRegion(AddrRegion);
    m_CurrSize++;
  }

  int GetSize() const { return(m_CurrSize); }

  const AXI_AddressRegion& operator[](int i) const
  {
    sc_assert((i >= 0) && (i < m_CurrSize));
    return(*m_Data[i]);
  }

  void PutDefaultSlave(int& Id) { m_DefaultSlave = Id; }
  int GetDefaultSlave() const { return(m_DefaultSlave); }

private:
  void Resize(int NewSize)
  {
    AXI_AddressRegion** NewData;
    if (NewSize <= m_MaxSize) return;

    m_MaxSize = NewSize;
    NewData = new AXI_AddressRegion*[NewSize];
    for (int i = 0; i < m_CurrSize; i++)
      NewData[i] = m_Data[i];
    if (NewData) delete[] m_Data;
    m_Data = NewData;
  }

  int m_MaxSize;
  int m_CurrSize;
  int m_DefaultSlave;
  AXI_AddressRegion** m_Data;
};
//-------------------------- End of address classes ----------------------

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_TYPES_H
