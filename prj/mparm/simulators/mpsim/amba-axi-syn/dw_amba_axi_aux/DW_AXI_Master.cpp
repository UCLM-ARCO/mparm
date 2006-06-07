// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 09/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 master example.
//    - Example master using external master state machine
//    - The 4 AXI channels (address, read, write, response)
//      are served in parallel
// ============================================================================

#include "DW_AXI_Master.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_Master<NegEdge, Verbose>::
DW_AXI_Master(sc_module_name Name
      , int ID
      , int Priority
      , int WriteReorderDepth
      , int TimeOut
      , int NumWriteWaits
      , int NumReadWaits
      , int DataBusBytes
      , double ClockFrequency
      )
      : AXI_SlaveIFimpl<NegEdge>(Name, ID, 1, AXI_IM_SP, DataBusBytes,
                         (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes / AXI_WORDSIZE_BYTES, ClockFrequency)
      , MasterP        (ID)
      , m_ID           (ID)
      , m_Priority     (Priority)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
{
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert(Priority >= 0);
  sc_assert(NumWriteWaits >= 0);
  sc_assert(NumReadWaits >= 0);
  sc_assert(WriteReorderDepth == 1);
  sc_assert(TimeOut >= 0);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  int AddrDepth = 3;
  int ReadDepth = 3;
  int WriteDepth = 3;
  m_VC1 = 0;
  m_VC2 = 1;

  m_A_FIFO = new AXI_FIFO<Achannel>(Name, AddrDepth);
  m_AwFIFO = new AXI_FIFO<Achannel>(Name, 10);
  m_B_FIFO = new AXI_FIFO<Bchannel>(Name, AddrDepth);
  m_R_FIFO = new AXI_FIFO_P<Rchannel>(Name, ReadDepth+1,
                                      DataBusBytes, m_DataBusWords);
  m_W_FIFO = new AXI_FIFO_P<Wchannel>*[AXI_MAX_ID_SIZE];
  for (int i = 0; i < AXI_MAX_ID_SIZE; i++)
    m_W_FIFO[i] = new AXI_FIFO_P<Wchannel>(Name, WriteDepth,
                                           DataBusBytes, m_DataBusWords);

  m_MasterFSM = new AXI_MaRequest<NegEdge, Verbose>(Name,
                   ID, WriteReorderDepth, TimeOut,
                   NumWriteWaits, NumReadWaits,
                   0 /* PID */, MasterP,
                   0 /* Idx */, static_cast<AXI_SlaveIFimpl<NegEdge>*>(this),
                   m_A_FIFO, m_AwFIFO, m_W_FIFO, m_R_FIFO, m_B_FIFO,
                   DataBusBytes, m_DataBusWords);

  m_Ain = new Achannel;
  m_Aout = new Achannel;
  m_Bold = new Bchannel;
  m_Bcurr = new Bchannel;
  m_Win  = new Wchannel(DataBusBytes, m_DataBusWords);
  m_Wout = new Wchannel(DataBusBytes, m_DataBusWords);
  m_Rold = new Rchannel(DataBusBytes, m_DataBusWords);
  m_Rcurr = new Rchannel(DataBusBytes, m_DataBusWords);

  SC_METHOD(Master);
  this->sensitive << this->Aclk.pos();
  this->dont_initialize();
  SC_METHOD(AresetMa);
  this->sensitive << this->Aresetn;
  this->dont_initialize();

  // define a few masters
  // Virtual Channel 1
  m_LockBurstLen[m_VC1] = 3;
  switch(m_ID) {
    case 1: m_NumWrites[m_VC1] = 5;
            m_Adelay[m_VC1]    = 0;
            m_Wdelay[m_VC1]    = 0;
            m_Rdelay[m_VC1]    = 0;
            m_Len[m_VC1]       = 4;
            m_Size[m_VC1]      = ASIZE_32;
            m_Wstrb[m_VC1]     = 0xF;
            m_Aid[m_VC1]       = 1;
            m_Addr[m_VC1]      = 0x4;
            m_Burst[m_VC1]     = ABURST_INCR;
            m_Lock[m_VC1]      = ALOCK_NORMAL;
            break;
    case 2: m_NumWrites[m_VC1] = 6;
            m_Adelay[m_VC1]    = 2;
            m_Wdelay[m_VC1]    = 5;
            m_Rdelay[m_VC1]    = 0;
            m_Len[m_VC1]       = 10;
            m_Size[m_VC1]      = ASIZE_8;
            m_Wstrb[m_VC1]     = 0x1;
            m_Aid[m_VC1]       = 4;
            m_Addr[m_VC1]      = 0x4;
            m_Burst[m_VC1]     = ABURST_INCR;
            m_Lock[m_VC1]      = ALOCK_LOCKED;
            break;
    case 3: m_NumWrites[m_VC1] = 5;
            m_Adelay[m_VC1]    = 0;
            m_Wdelay[m_VC1]    = 0;
            m_Rdelay[m_VC1]    = 0;
            m_Len[m_VC1]       = 15;
            m_Size[m_VC1]      = ASIZE_16;
            m_Wstrb[m_VC1]     = 0x3;
            m_Aid[m_VC1]       = 2;
            m_Addr[m_VC1]      = 0x1004;
            m_Burst[m_VC1]     = ABURST_WRAP;
            m_Lock[m_VC1]      = ALOCK_NORMAL;
            break;
    case 4: m_NumWrites[m_VC1] = 1;
            m_Adelay[m_VC1]    = 0;
            m_Wdelay[m_VC1]    = 0;
            m_Rdelay[m_VC1]    = 0;
            m_Len[m_VC1]       = 10;
            m_Size[m_VC1]      = ASIZE_8;
            m_Wstrb[m_VC1]     = 0x1;
            m_Aid[m_VC1]       = 3;
            m_Addr[m_VC1]      = 0x4;
            m_Burst[m_VC1]     = ABURST_INCR;
            m_Lock[m_VC1]      = ALOCK_NORMAL;
            break;
    default: m_NumWrites[m_VC1] = 10;
            m_Adelay[m_VC1]     = 0;
            m_Wdelay[m_VC1]     = 0;
            m_Rdelay[m_VC1]     = 0;
            m_Len[m_VC1]        = 0;
            m_Size[m_VC1]       = ASIZE_32;
            m_Wstrb[m_VC1]      = 0xF;
            m_Aid[m_VC1]        = m_ID;
            m_Addr[m_VC1]       = 0x4;
            m_Burst[m_VC1]      = ABURST_INCR;
            m_Lock[m_VC1]       = ALOCK_NORMAL;
            break;
  }

  for (int i = 0; i < AXI_MAX_VC_SIZE; i++) {
    m_i[i]           = 0;
    m_WaddrValid[i]  = false;
    m_AddrCount[i]   = 0;
    m_WburstCount[i] = 0;
    m_AdelayC[i]    = 0;
    m_WdelayC[i]    = 0;
    m_RdelayC[i]    = 0;
  }
}
template <bool NegEdge, bool Verbose> DW_AXI_Master<NegEdge, Verbose>::
~DW_AXI_Master()
{
  delete m_Ain; delete m_Aout; delete m_Bold; delete m_Bcurr;
  delete m_Win; delete m_Wout; delete m_Rold; delete m_Rcurr; 
  delete m_MasterFSM;
  delete m_A_FIFO;  delete m_AwFIFO;  delete m_B_FIFO;  delete m_R_FIFO;
  for (int i = 0; i < AXI_MAX_ID_SIZE; i++)
    delete m_W_FIFO[i];
  delete [] m_W_FIFO;
}

template <bool NegEdge, bool Verbose> void DW_AXI_Master<NegEdge, Verbose>::
Master()
{ 
  // to be save, we call each state machine at every clock cycle
  // exactly onces per master

if (!m_Reset) {
  // generate write address
  if ((m_AdelayC[m_VC1]++ >= m_Adelay[m_VC1]) && m_A_FIFO->IsFree() &&
      (m_AddrCount[m_VC1] < m_NumWrites[m_VC1])) {
    m_Ain->Awrite = true;
    m_Ain->Alen = m_Len[m_VC1]; m_Ain->Asize = m_Size[m_VC1];
    m_Ain->AID = m_Aid[m_VC1]; m_Ain->Addr = m_Addr[m_VC1];
    m_Ain->Aburst = m_Burst[m_VC1]; 
    m_Ain->Alock = (m_AddrCount[m_VC1] >= m_LockBurstLen[m_VC1]) ?
                    ALOCK_NORMAL : m_Lock[m_VC1];
    m_A_FIFO->PutValue(*m_Ain);
    m_AwFIFO->PutValue(*m_Ain);
    m_AddrCount[m_VC1]++;
  }
  // generate read address
  if ((m_RdelayC[m_VC1]++ >= m_Rdelay[m_VC1]) && m_A_FIFO->IsFree() &&
      (m_AddrCount[m_VC1] >= m_NumWrites[m_VC1])) {
    m_Ain->Awrite = false;
    m_Ain->Alen = m_Len[m_VC1]; m_Ain->Asize = m_Size[m_VC1];
    m_Ain->AID = m_Aid[m_VC1]; m_Ain->Addr = m_Addr[m_VC1];
    m_Ain->Aburst = m_Burst[m_VC1]; 
    m_Ain->Alock = (m_AddrCount[m_VC1] >= m_LockBurstLen[m_VC1]) ?
      ALOCK_NORMAL : m_Lock[m_VC1];
    m_A_FIFO->PutValue(*m_Ain);
//    m_AddrCount[m_VC1]++;
  }
  // generate write data
  if ((m_WdelayC[m_VC1]++ >= m_Wdelay[m_VC1])
       && m_WburstCount[m_VC1] < m_NumWrites[m_VC1]) {
    if ((m_i[m_VC1] == 0) && (!m_WaddrValid[m_VC1])) { 
      if ((m_WaddrValid[m_VC1] = m_AwFIFO->ShowValue(*m_Ain))) {
        m_WID[m_VC1]  = m_Ain->AID;
        m_Alen[m_VC1] = m_Ain->Alen;
      }
    }
    if (m_WaddrValid[m_VC1] && m_W_FIFO[m_WID[m_VC1]]->IsFree()) {
      // except for Wdata and Wsrtb, m_MasterFSM sets the signals
      m_Win->Wdata[0] = m_i[m_VC1]; m_Win->Wstrb[0] = m_Wstrb[m_VC1];
      m_W_FIFO[m_WID[m_VC1]]->PutValue(*m_Win);
      if (m_i[m_VC1]++ == m_Alen[m_VC1]) {
        // last beat in a burst
        m_i[m_VC1] = 0; m_WburstCount[m_VC1]++;
        m_WaddrValid[m_VC1] = false;
      }
    }
  }

  // get B-response data otherwise m_MasterFSM->GetBresponse
  // will not accept new data
  m_B_FIFO->GetValue(*m_Bold);
  
  // get R-response data otherwise m_MasterFSM->GetRresponse
  // will not accept new data
  m_R_FIFO->GetValue(*m_Rold);

  // execute send address state machine
  m_MasterFSM->PutArequest(*m_Aout);
  // execute send write data state machine
  m_MasterFSM->PutWrequest(*m_Wout, m_VC1, 0);
  // execute get Bresponse state machine
  m_MasterFSM->GetBresponse(*m_Bcurr);
  // execute get read data state machine
  m_MasterFSM->GetRresponse(*m_Rcurr);
} // end of (!m_Reset) branch
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_Master<NegEdge, Verbose>::
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

  for (int i = 0; i < AXI_MAX_VC_SIZE; i++) {
    m_i[i]           = 0;
    m_WaddrValid[i]  = false;
    m_AddrCount[i]   = 0;
    m_WburstCount[i] = 0;
    m_AdelayC[i]    = 0;
    m_WdelayC[i]    = 0;
    m_RdelayC[i]    = 0;
  }
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_Master<NegEdge, Verbose>::
AresetMa()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_A_FIFO->Reset();
    m_AwFIFO->Reset();
    m_B_FIFO->Reset();
    m_R_FIFO->Reset();
    m_MasterFSM->Reset();
    for (int i = 0; i < AXI_MAX_ID_SIZE; i++)
      m_W_FIFO[i]->Reset();
    m_Reset = true;
  }
}

// ----------------------------------------------------------------------------
// Parameter and direct access methods
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_Master<NegEdge, Verbose>::
GetMasterParam(int Idx, int& Priority)
{
  Priority = m_Priority;
  return(true);
}
// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_Master<true, true >;
template class DW_AXI_Master<false, true >;
template class DW_AXI_Master<true, false >;
template class DW_AXI_Master<false, false >;
