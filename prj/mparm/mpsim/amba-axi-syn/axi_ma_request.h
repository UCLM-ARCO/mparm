// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 08/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Master Request Class
//    - Request/Response handling of a master
//    - Issues master requests
//    - Samples and stores the slave responses
// ============================================================================

#ifndef _AXI_MA_REQUEST_H
#define _AXI_MA_REQUEST_H

#include "axi_slave_if_impl.h"
#include "axi_master_port.h"
#include "axi_fifo.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge, bool Verbose> class AXI_MaRequest
{
public:
AXI_MaRequest(const char* Name
      , int ID // Unique number among all masters attached to the same IC.
        // Must satisfy: 0 <= ID <= 31
      , int WriteReorderDepth // WriteReorderDepth > 1 enables re-ordering
        // of data transmission compared to the order of submitted 
        // addresses. E.g. The data for the second address may be
        // transmitted before the data for the first address.
        // Must satisfy (Limitation): WriteReorderDepth == 1
      , int TimeOut // Time out value, used for write data reordering
        // if the data for the second address should be
        // transmitted before the data for the first address
        // The master FSM waits at most Timeout cycles for the
        // second address. Must satisfy: TimeOut >= 0
      , int NumWriteWaits // Number of cycles the master FSM waits before
        // transmitting the next write data.
        // Must satisfy: NumWriteWaits >= 0
      , int NumReadWaits // Number of cycles the master FSM waits before
        // accepting the next read data.
        // Must satisfy: NumReadWaits >= 0
      , int PID // Port ID
      , AXI_MasterPort& MasterP // Master port
      , int Idx // Interface index
      , AXI_SlaveIFimpl<NegEdge>* SlaveIFimpl // IF impl. class
      , AXI_FIFO<Achannel>* A_FIFO // address FIFO
      , AXI_FIFO<Achannel>* AwFIFO  // write address FIFO
      , AXI_FIFO_P<Wchannel>** W_FIFO // write data FIFO
      , AXI_FIFO_P<Rchannel>* R_FIFO // read data FIFO
      , AXI_FIFO<Bchannel>* B_FIFO // write response data FIFO
      , int DataBusBytes // Number of byte lanes of the AXI IC.
      , int DataBusWords // Number of words (4-bytes) needed to represent
        // DataBusBytes byte lanes
             );
~AXI_MaRequest();


  void Reset();
  bool PutArequest(Achannel& A);
  bool PutWrequest(Wchannel& W, int VC_Idx, int FIFO_Idx);
  bool GetRresponse(Rchannel& R);
  bool GetBresponse(Bchannel& B);

private:
  inline void ExecWrite(Wchannel& W, int , bool& rt);

  // parameter
  const char* m_Name;
  int m_ID;
  int m_WriteReorderDepth;
  int m_TimeOut;
  int m_NumWriteWaits;
  int m_NumReadWaits;
  int m_PID;
  AXI_MasterPort& m_MasterP;
  int m_Idx;
  AXI_SlaveIFimpl<NegEdge>* m_SlaveIFimpl;
  AXI_FIFO<Achannel>* m_A_FIFO; // FIFO for addresses
  AXI_FIFO<Achannel>* m_AwFIFO; // FIFO for write addresses
  AXI_FIFO_P<Wchannel>** m_W_FIFO; // FIFO array for write data
  AXI_FIFO_P<Rchannel>* m_R_FIFO; // FIFO array for read data
  AXI_FIFO<Bchannel>* m_B_FIFO; // FIFO for B-responses
  int m_DataBusBytes;
  int m_DataBusWords;

  // constants
  int m_NumVC;

  // temp data
  AreadySig* m_Ar;
  WreadySig* m_Wr;
  RreadySig* m_Rr;
  BreadySig* m_Br;
  Bchannel* m_B;

  // state machine variables

  // address channel states
  axi_putc_state m_Astate;
  bool m_AaddrValid;

  // write channel states
  int m_NumWwaits;
  int m_CurrTimeOut;
  int* m_Nw; // transfer counter for write transactions
  axi_putd_state* m_Wstate;
  bool* m_WaddrValid;
  bool* m_AwActive;
  Achannel* m_Wa;

  // read channel states
  int m_NumRwaits;
  axi_get_state m_Rstate;

  // Bresponse channel states
  axi_get_state m_Bstate;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_MA_REQUEST_H
