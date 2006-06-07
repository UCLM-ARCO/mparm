// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 Simple Master Example
//    - Example for usage of Master IF method calls (PutChannel()) and
//      Slave IF implementation class methods (GetChannel()) 
//    - Sequential master
// ============================================================================

#include "DW_AXI_SimpleMaster.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_SimpleMaster<NegEdge, Verbose>::
DW_AXI_SimpleMaster(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority // Priority of the master.
               // Must satisfy: Priority >= 0
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system.
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      )
      : AXI_SlaveIFimpl<NegEdge>(Name, ID, 1, AXI_IM_SP, DataBusBytes, 
                        (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes / AXI_WORDSIZE_BYTES, ClockFrequency)
      , MasterP        (ID)
      , m_Name         (Name)
      , m_ID           (ID)
      , m_Priority     (Priority)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
{
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert(Priority >= 0);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  m_WstrbBytes = m_WstrbWords * AXI_WSTRB_BYTES;
  m_BitMask    = DataBusBytes - 1;

  SC_THREAD(Send);
  this->sensitive << this->Aclk.pos();
  SC_METHOD(Areset_SimpleMaster);
  this->sensitive << this->Aresetn;
  this->dont_initialize();
}
template <bool NegEdge, bool Verbose> DW_AXI_SimpleMaster<NegEdge, Verbose>::
~DW_AXI_SimpleMaster()
{
}

template <bool NegEdge, bool Verbose>
void DW_AXI_SimpleMaster<NegEdge, Verbose>::Send()
{
  int i;
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  RreadySig Rr;
  BreadySig Br;

  axi_data_bt Data[1024];
  int NumWrites;
  int Len;
  axi_asize_t Size;
  axi_wstrb_bt Wstrb;
  int Aid;
  axi_addr_t Addr;
  axi_aburst_t Burst;
  axi_alock_t Lock;

  // put some data into data buffer
  for (i = 0; i < 1024; i++) Data[i] = i;

  // define a few masters
  switch(m_ID) {
    case 1: NumWrites = 100;
            Len       = 9;
            Size      = ASIZE_32;
            Wstrb     = 0xF;
            Aid       = 1;
            Addr      = 0x4;
            Burst     = ABURST_INCR;
            Lock      = ALOCK_NORMAL;
            break;
    case 2: NumWrites = 50;
            Len       = 15;
            Size      = ASIZE_16;
            Wstrb     = 0x3;
            Aid       = 2;
            Addr      = 0x1004;
            Burst     = ABURST_WRAP;
            Lock      = ALOCK_NORMAL;
            break;
    case 3: NumWrites = 10;
            Len       = 10;
            Size      = ASIZE_8;
            Wstrb     = 0x1;
            Aid       = 3;
            Addr      = 0x4;
            Burst     = ABURST_INCR;
            Lock      = ALOCK_NORMAL;
            break;
    case 4: NumWrites = 3;
            Len       = 10;
            Size      = ASIZE_8;
            Wstrb     = 0x1;
            Aid       = 4;
            Addr      = 0x4;
            Burst     = ABURST_INCR;
            Lock      = ALOCK_LOCKED;
            break;
    default: NumWrites = 100;
            Len       = 0;
            Size      = ASIZE_32;
            Wstrb     = 0x1;
            Aid       = m_ID;
            Addr      = 0x4;
            Burst     = ABURST_INCR;
            Lock      = ALOCK_NORMAL;
            break;
  }

  int ta = 0;
  while(ta < NumWrites) { // write transactions
  if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
    ta++;
    // Address channel
    A.Awrite = true; A.Avalid = true; 
    A.Alen = Len; A.Asize = Size; A.AID = Aid; A.Addr = Addr;
    A.Aburst = Burst; A.Alock = (ta >= NumWrites) ? ALOCK_NORMAL : Lock;
    MasterP.PutChannel(A);
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
         << " AID=" << A.AID
         << " Awrite=" << A.Awrite << " Alen=" << A.Alen
         << " Asize=" << A.Asize << " Aburst=" << A.Aburst
         << " Alock=" << A.Alock << " Aprot=" << A.Aprot
         << " Acache=" << A.Acache
         << " at " << sc_time_stamp().to_seconds() << endl;
}
    AXI_WAIT_HS(Ar, Ar.Aready); // wait for Aready high
    A.Avalid = false; MasterP.PutChannel(A);
    if (!m_Reset) {
      // Write data channel
      for (i = 0; i < A.Alen+1; i++) {
        if (i == A.Alen) W.Wlast = true; else W.Wlast = false;
        W.Wvalid = true;
        W.Wdata[0] = Data[i];
        W.WID = A.AID; W.Wstrb[0] = Wstrb;
        MasterP.PutChannel(W);
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.GetAddressN(i+1)
         << " WID=" << W.WID
         << " Wdata=" << axi_hexprint(W.Wdata, W.Wstrb, m_DataBusBytes)
         << " Wlast=" << W.Wlast
         << " at " << sc_time_stamp().to_seconds() << endl;
}
        AXI_WAIT_HS(Wr, Wr.Wready); // wait for Wready high
        if (m_Reset) break;
      }
      W.Wvalid = false; MasterP.PutChannel(W);
    }

    if (!m_Reset) {
      // Buffered response channel
      Br.Bready = true; MasterP.PutChannel(Br);
      AXI_WAIT_HS(B, B.Bvalid); // wait for Bvalid
if (Verbose) {
  if (!m_Reset)
    cout << m_Name << " ID=" << m_ID
         << " BID=" << B.BID << " Bresp=" << B.Bresp
         << " at " << sc_time_stamp().to_seconds() << endl;
}
      Br.Bready = false; MasterP.PutChannel(Br);
    } // end of not Reset branch
  } // end of not Reset branch
  }// end of thread loop

  while(true) { // read transactions
  if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
    // Address channel
    ta++;
    A.Awrite = false; A.Avalid = true; 
    A.Alen = Len; A.Asize = Size; A.AID = Aid; A.Addr = Addr;
    A.Aburst = Burst; A.Alock = (ta >= NumWrites + 5) ? ALOCK_NORMAL : Lock;
    MasterP.PutChannel(A);
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
         << " AID=" << A.AID
         << " Awrite=" << A.Awrite << " Alen=" << A.Alen
         << " Asize=" << A.Asize << " Aburst=" << A.Aburst
         << " Alock=" << A.Alock << " Aprot=" << A.Aprot
         << " Acache=" << A.Acache
         << " at " << sc_time_stamp().to_seconds() << endl;
}
    AXI_WAIT_HS(Ar, Ar.Aready); // wait for Aready high
    A.Avalid = false; MasterP.PutChannel(A);

    if (!m_Reset) {
      // read data channel
      for (i = 0; i < A.Alen+1; i++) {
        Rr.Rready = true; MasterP.PutChannel(Rr);
        AXI_WAIT_HS(R, R.Rvalid); // wait for Rvalid
        if (m_Reset) break;
if (Verbose) {
    axi_wstrb_bt m_Wstrb[4];
    axi_get_wstrb(m_Wstrb, A.GetAddressN(i+1) & m_BitMask,
                 (1 << A.Asize), m_WstrbBytes, m_WstrbWords);
   cout << m_Name << " ID=" << m_ID
         << " RID=" << R.RID 
         << " Rdata=" << axi_hexprint(R.Rdata, m_Wstrb, m_DataBusBytes)
         << " Rlast=" << R.Rlast << " Rresp=" << R.Rresp
         << " at " << sc_time_stamp().to_seconds() << endl;
}
      }
      Rr.Rready = false; MasterP.PutChannel(Rr);
    } // end of if (!m_Reset) branch
  } // end of not Reset branch
  }// end of thread loop
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_SimpleMaster<NegEdge, Verbose>::
Reset()
{
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  RreadySig Rr;
  BreadySig Br;
  A.Avalid = false; MasterP.PutChannel(A); // set Avalid low
  W.Wvalid = false; MasterP.PutChannel(W); // set Wvalid low
  Rr.Rready = false; MasterP.PutChannel(Rr); // Set Rready low
  Br.Bready = false; MasterP.PutChannel(Br); // Set Bready low
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_SimpleMaster<NegEdge, Verbose>::
Areset_SimpleMaster()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_Reset = true;
  }
}

// ----------------------------------------------------------------------------
// Parameter and direct access methods
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_SimpleMaster<NegEdge, Verbose>::GetMasterParam(int Idx, int& Priority)
{
  Priority = m_Priority;
  return(true);
}
// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_SimpleMaster<true, true >;
template class DW_AXI_SimpleMaster<false, true >;
template class DW_AXI_SimpleMaster<true, false >;
template class DW_AXI_SimpleMaster<false, false >;
