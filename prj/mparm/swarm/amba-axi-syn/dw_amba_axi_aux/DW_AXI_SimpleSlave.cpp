// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 Simple Slave Example
//    - Example for usage of Slave IF method calls (PutChannel()) and
//      Master IF implementation class methods (GetChannel()) 
//    - Pure sequential slave
//    - Can accept 1 address
//    - Delay between receiving and accepting requests is > 0
// ============================================================================

#include "DW_AXI_SimpleSlave.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_SimpleSlave<NegEdge, Verbose>::
DW_AXI_SimpleSlave(sc_module_name Name
      , int ID
      , axi_addr_t StartAddress
      , axi_addr_t EndAddress
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
      , m_Name         (Name)
      , m_ID (ID)
      , m_StartAddress (StartAddress)
      , m_EndAddress   (EndAddress)
      , m_DefaultSlave (DefaultSlave)
      , m_MonitorOn    (MonitorOn)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
{
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert((EndAddress + 1 - StartAddress) >= 0x1000);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  m_WstrbBytes = m_WstrbWords * AXI_WSTRB_BYTES;
  m_BitMask    = DataBusBytes - 1;

  m_MemoryImpl = new AXI_MemoryImpl(Name,
                     m_StartAddress, m_EndAddress, FileName, BigEndian,
                     ExclusiveAccess, DefaultSlave,
                     m_MonitorOn, m_DataBusBytes, m_DataBusWords);
  SC_THREAD(SeqSlave);
  this->sensitive << this->Aclk.pos();
  SC_METHOD(Areset_SimpleSlave);
  this->sensitive << this->Aresetn;
  this->dont_initialize();
}
template <bool NegEdge, bool Verbose> DW_AXI_SimpleSlave<NegEdge, Verbose>::
~DW_AXI_SimpleSlave()
{
  delete m_MemoryImpl;
}

template <bool NegEdge, bool Verbose> void DW_AXI_SimpleSlave<NegEdge, Verbose>::SeqSlave()
{
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  RreadySig Rr;
  BreadySig Br;

  while(true) {
  if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
    Ar.Aready = true; SlaveP.PutChannel(Ar);// set Aready high
    AXI_WAIT_HS(A, A.Avalid); // wait for valid address
    if (!m_Reset) {
      Ar.Aready = false; SlaveP.PutChannel(Ar); // set Aready low
      GetChannel(0, A); // sample the address channel
      int N = 0;           // initialize beat counter
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
         << " AID=" << A.AID
         << " Awrite=" << A.Awrite << " Alen=" << A.Alen
         << " Asize=" << A.Asize << " Aburst=" << A.Aburst
         << " Alock=" << A.Alock << " Aprot=" << A.Aprot
         << " Acache=" << A.Acache
         << " at " << sc_time_stamp().to_seconds() << endl;
}
      if (A.Awrite) { // write request
        axi_resp_t Eflag = RESP_OKAY;
        do {
          Wr.Wready = true; SlaveP.PutChannel(Wr);// set Wready high
          AXI_WAIT_HS(W, W.Wvalid); // wait for Wvalid = high
          if (m_Reset) break;
          Wr.Wready = false; SlaveP.PutChannel(Wr); // set Wready low
          GetChannel(0, W); // sample the write channel
          if (!(PutDirect(m_ID, N + 1, A, W, B.Bresp))) // write data into memory
            Eflag = B.Bresp;
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.GetAddressN(N+1)
         << " WID=" << W.WID
         << " Wdata=" << axi_hexprint(W.Wdata, W.Wstrb, m_DataBusBytes)
         << " Wlast=" << W.Wlast
         << " at " << sc_time_stamp().to_seconds() << endl;
}
          if (N == A.Alen) { // last transfer in a transaction
            B.Bvalid = true; B.BID = W.WID;
            if (A.Alen > 0) B.Bresp = Eflag;
            SlaveP.PutChannel(B); // write to response channel
if (Verbose) {
    cout << m_Name << " ID=" << m_ID
         << " BID=" << B.BID << " Bresp=" << B.Bresp
         << " at " << sc_time_stamp().to_seconds() << endl;
}
            AXI_WAIT_HS(Br, Br.Bready); // wait for Bready = high
            if (m_Reset) break;
            B.Bvalid = false; SlaveP.PutChannel(B); // Set Bvalid low
          } // end of if last branch
        } while (N++ < A.Alen);
      } // end of write branch
      else { // read request
        do {
          R.Rvalid = true; 
          R.Rlast = (N == A.Alen) ? true : false;
          R.RID = A.AID;
          PutDirect(m_ID, N + 1, A, R, R.Rresp);
          SlaveP.PutChannel(R);
if (Verbose) {
    axi_wstrb_bt m_Wstrb[4];
    axi_get_wstrb(m_Wstrb, A.GetAddressN(N+1) & m_BitMask,
                 (1 << A.Asize), m_WstrbBytes, m_WstrbWords);
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.GetAddressN(N+1)
         << " RID=" << R.RID 
         << " Rdata=" << axi_hexprint(R.Rdata, m_Wstrb, m_DataBusBytes)
         << " Rlast=" << R.Rlast << " Rresp=" << R.Rresp
         << " at " << sc_time_stamp().to_seconds() << endl;
}
          AXI_WAIT_HS(Rr, Rr.Rready); // wait for Rready = high
          if (m_Reset) break;
        } while (N++ < A.Alen);
        R.Rvalid = false; SlaveP.PutChannel(R); // Set Rvalid low
      } // end of read request
    } // end of if (!m_Reset) branch
  } // end of not Reset branch
  } // end of thread loop
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_SimpleSlave<NegEdge, Verbose>::
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
template <bool NegEdge, bool Verbose> void DW_AXI_SimpleSlave<NegEdge, Verbose>::
Areset_SimpleSlave()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_Reset = true;
  }
}
// ----------------------------------------------------------------------------
// Direct access methods
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_SimpleSlave<NegEdge, Verbose>::GetSlaveParam(int Idx, int& ItrlDepth, AXI_AddressMap& M)
{
  AXI_AddressRegion* mr = new AXI_AddressRegion(m_ID, m_StartAddress,
                                                      m_EndAddress);
  M.PushBack(*mr); delete(mr);
  ItrlDepth = 1;
  if (m_DefaultSlave) M.PutDefaultSlave(m_ID);
  return(true);
}
template <bool NegEdge, bool Verbose> bool DW_AXI_SimpleSlave<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, const Wchannel &W,
           axi_resp_t& Status)
{
  return (m_MemoryImpl->PutDirect(ID, BeatNumber, A, W, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_SimpleSlave<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, Rchannel &R,
           axi_resp_t& Status)
{
  return (m_MemoryImpl->PutDirect(ID, BeatNumber, A, R, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_SimpleSlave<NegEdge, Verbose>::
PutDirect(int ID, bool Write, axi_addr_t Addr,
                        axi_data_t Data, int NumBytes)
{
  return (m_MemoryImpl->PutDirect(ID, Write,Addr, Data, NumBytes));
}
// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_SimpleSlave<true, true >;
template class DW_AXI_SimpleSlave<false, true >;
template class DW_AXI_SimpleSlave<true, false >;
template class DW_AXI_SimpleSlave<false, false >;
