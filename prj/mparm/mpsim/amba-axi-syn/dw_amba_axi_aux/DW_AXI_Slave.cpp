// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//
//  Description : AXI TL Layer-1 slave example.
//    - Uses external slave state machine
//    - The 4 AXI channels (address, read, write, response)
//      are served in parallel
// ============================================================================

#include "DW_AXI_Slave.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_Slave<NegEdge, Verbose>::
DW_AXI_Slave(sc_module_name Name
      , int ID
      , axi_addr_t StartAddress
      , axi_addr_t EndAddress
      , int AddrDepth
      , int IntrlDepth
      , int ReadReorderDepth
      , int TimeOut
      , int NumWriteWaits
      , int NumReadWaits
      , const sc_string& FileName
      , bool BigEndian
      , bool ExclusiveAccess
      , bool DefaultSlave
      , bool MonitorOn
      , int DataBusBytes
      , double ClockFrequency
      )
      : AXI_MasterIFimpl<NegEdge>(Name, ID, 1, AXI_IM_SP, DataBusBytes,
                         (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes / AXI_WORDSIZE_BYTES, ClockFrequency)
      , SlaveP(ID)
      , m_ID (ID)
      , m_StartAddress (StartAddress)
      , m_EndAddress   (EndAddress)
      , m_IntrlDepth   (IntrlDepth)
      , m_DefaultSlave (DefaultSlave)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
{
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert(AddrDepth >= 1);
  sc_assert(IntrlDepth >= 1);
  sc_assert((EndAddress + 1 - StartAddress) >= 0x1000);
  sc_assert(AddrDepth >= IntrlDepth);
  sc_assert(NumWriteWaits >= 0);
  sc_assert(NumReadWaits >= 0);
  sc_assert(ReadReorderDepth == 1);
  sc_assert(TimeOut >= 0);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_MemoryImpl = new AXI_MemoryImpl(Name,
                     StartAddress, EndAddress, FileName, BigEndian,
                     ExclusiveAccess, DefaultSlave,
                     MonitorOn, DataBusBytes, m_DataBusWords);
  m_SlaveFSM = new AXI_SlRequest<AXI_MemoryImpl, NegEdge, Verbose>(Name,
                   ID, AddrDepth, IntrlDepth, ReadReorderDepth, TimeOut,
                   NumWriteWaits, NumReadWaits,
                   0 /* PID */, SlaveP,
                   0 /* Idx */, static_cast<AXI_MasterIFimpl<NegEdge>*>(this),
                   m_MemoryImpl, DataBusBytes, m_DataBusWords);

  m_A = new Achannel;
  m_W = new Wchannel(DataBusBytes, m_DataBusWords);
  m_R = new Rchannel(DataBusBytes, m_DataBusWords);
  m_B = new Bchannel;

  SC_METHOD(Slave);
  this->sensitive << this->Aclk.pos();
  this->dont_initialize();
  SC_METHOD(AresetSl);
  this->sensitive << this->Aresetn;
  this->dont_initialize();
}

template <bool NegEdge, bool Verbose> DW_AXI_Slave<NegEdge, Verbose>::
~DW_AXI_Slave()
{
  delete m_MemoryImpl;
  delete m_SlaveFSM;
  delete m_A; delete m_W; delete m_B; delete m_R;
}

template <bool NegEdge, bool Verbose> void DW_AXI_Slave<NegEdge, Verbose>::
Slave()
{
  if (!m_Reset) {
    m_SlaveFSM->GetArequest(*m_A);
    m_SlaveFSM->GetWrequest(*m_W);
    m_SlaveFSM->PutRresponse(*m_R, 0, 0);
    m_SlaveFSM->PutBresponse(*m_B);
  }
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_Slave<NegEdge, Verbose>::
Reset()
{
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  Ar.Aready = false; SlaveP.PutChannel(Ar); // set Aready low
  Wr.Wready = false; SlaveP.PutChannel(Wr); // set Wready low
  B.Bvalid = false; SlaveP.PutChannel(B); // Set Bvalid low
  R.Rvalid = false; SlaveP.PutChannel(R); // Set Rvalid low
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_Slave<NegEdge, Verbose>::
AresetSl()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_MemoryImpl->Reset();
    m_SlaveFSM->Reset();
    m_Reset = true;
  }
}

// ----------------------------------------------------------------------------
// Direct access methods
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_Slave<NegEdge, Verbose>::
GetSlaveParam(int Idx, int& ItrlDepth, AXI_AddressMap& M)
{
  AXI_AddressRegion* mr = new AXI_AddressRegion(m_ID, m_StartAddress,
                                                      m_EndAddress);
  M.PushBack(*mr); delete(mr);
  ItrlDepth = m_IntrlDepth;
  if (m_DefaultSlave) M.PutDefaultSlave(m_ID);
  return(true);
}
template <bool NegEdge, bool Verbose> bool DW_AXI_Slave<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, const Wchannel &W,
           axi_resp_t& Status)
{
  return (m_MemoryImpl->PutDirect(ID, BeatNumber, A, W, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_Slave<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, Rchannel &R,
           axi_resp_t& Status)
{
  return (m_MemoryImpl->PutDirect(ID, BeatNumber, A, R, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_Slave<NegEdge, Verbose>::
PutDirect(int ID, bool Write, axi_addr_t Addr,
                        axi_data_t Data, int NumBytes)
{
  return (m_MemoryImpl->PutDirect(ID, Write,Addr, Data, NumBytes));
}
// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_Slave<true, true >;
template class DW_AXI_Slave<false, true >;
template class DW_AXI_Slave<true, false >;
template class DW_AXI_Slave<false, false >;
