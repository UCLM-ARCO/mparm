// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 08/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Slave Request Class
//    - Request/Response handling of a slave
//    - Samples and stores the master request at all ports
//    - Sends response to master
//    - Accepts new addresses if Wlast was detected
//  Template argument T: Memory implementation
//    - This class must provide the memory allocation and the access methods
// ============================================================================

#ifndef _AXI_SL_REQUEST_H
#define _AXI_SL_REQUEST_H

#include "axi_master_if_impl.h"
#include "axi_slave_port.h"
#include "axi_fifo.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <class T, bool NegEdge, bool Verbose> class AXI_SlRequest
{
public:
AXI_SlRequest(const char* Name
        , int ID // Unique number among all slaves attached to the same IC.
          // Must satisfy: 0 <= ID <= 31
        , int AddrDepth // Number of addresses the slave can accept.
          // Must satisfy: AddrDepth >= 1
        , int IntrlDepth // Number of interleaved write bursts the slave can
          // accept. Must satisfy: AddrDepth >= IntrlDepth >= 1
        , int ReadReorderDepth // ReadReorderDepth > 1 enables re-ordering
          // of data transmission compared to the order of received 
          // read addresses. E.g. The data for the second address may be
          // transmitted before the data for the first address.
          // Must satisfy (Limitation): ReadReorderDepth = 1
        , int TimeOut // Time out value, used for read data reordering
          // if the data for the second address should be
          // transmitted before the data for the first address
          // The slave FSM waits at most Timeout cycles for the
          // second address. Must satisfy: TimeOut >= 0
        , int NumWriteWaits // Number of cycles the slave FSM waits before
          // accepting the next write data.
          // Must satisfy: NumWriteWaits >= 0
        , int NumReadWaits // Number of cycles the slave FSM waits before
          // transmitting the next read data.
          // Must satisfy: NumReadWaits >= 0
        , int PID // Port ID
        , AXI_SlavePort& SlaveP // Slave port
        , int Idx // Interface Idx
        , AXI_MasterIFimpl<NegEdge>* MasterIFimpl // IF implementation class
        , T* MemoryImpl // Memory implementation class
        , int DataBusBytes // Number of byte lanes of the AXI IC.
        , int DataBusWords // Number of words (4-bytes) needed to represent
          // DataBusBytes byte lanes
             );
~AXI_SlRequest();


  void Reset();
  bool GetArequest(Achannel& A);
  bool GetWrequest(Wchannel& W);
  bool PutRresponse(Rchannel& R, int VC_Idx, int FIFO_Idx);
  bool PutBresponse(Bchannel& B);

private:
  inline void ExecRead(Rchannel& R, int , bool& rt);

  // parameter
  const char* m_Name;
  int m_ID;
  int m_AddrDepth;
  int m_IntrlDepth;
  int m_ReadReorderDepth;
  int m_TimeOut;
  int m_NumWriteWaits;
  int m_NumReadWaits;
  int m_PID;
  AXI_SlavePort& m_SlaveP;
  int m_Idx;
  AXI_MasterIFimpl<NegEdge>* m_MasterIFimpl;
  T* m_MemoryImpl;
  int m_DataBusBytes;
  int m_DataBusWords;

  // constants
  int m_WstrbWords;
  int m_WstrbBytes;
  int m_BitMask;

  // data
  int* m_Nw;           // transfer counter for write transactions
  Achannel* m_Aactive; // contains data of active write transfers
  bool* m_WIDvalid;    // flag identifying if a WID is an active WID's
  int m_NumWwaits;
  AXI_FIFO<Achannel>* m_AwFIFO; // FIFO for write addresses
  AXI_FIFO<Achannel>* m_ArFIFO; // FIFO for read addresses
  AXI_FIFO<Bchannel>* m_B_FIFO; // FIFO for Bresponse data
  AreadySig* m_Ar;
  WreadySig* m_Wr;
  RreadySig* m_Rr;
  BreadySig* m_Br;
  Achannel* m_Ra;
  Bchannel* m_B;
  Wchannel* m_Win;

  // state machine variables
  axi_get_state m_Astate;
  axi_getw_state m_Wstate;
  axi_resp_t* m_Eflag;
  int m_NumAddr;
  int m_NumActivWIDs;

  // read channel states
  int m_NumRwaits;
  int* m_Nr;
  axi_putd_state* m_Rstate;
  bool* m_RaddrValid;
  int m_CurrTimeOut;
  bool m_ArActive[AXI_MAX_EID_SIZE];

  // Bresponse channel states
  axi_putc_state m_Bstate;
  bool m_BdataValid;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_SL_REQUEST_H
