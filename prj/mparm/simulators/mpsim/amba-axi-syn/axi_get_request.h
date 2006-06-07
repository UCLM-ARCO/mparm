// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Get Request Class
//    - Input stage for a single request
//    - Samples and stores the master request
//    - Performs address decoding 
// ============================================================================

#ifndef _AXI_GET_REQUEST_H
#define _AXI_GET_REQUEST_H

#include "axi_master_if_impl.h"
#include "axi_slave_port.h"
#include "axi_slave_if_impl.h"
#include "axi_master_port.h"
#include "axi_decoder_if.h"
#include "axi_fifo.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge, bool Verbose> class AXI_GetRequest
{
public:

  // constructor
  AXI_GetRequest(const char* Name
              , int PID  // Port ID (index of the multiport component)
              , AXI_SlaveMport<AXI_MAX_PORT_SIZE>& SlaveP
              , AXI_MasterMport<AXI_MAX_PORT_SIZE>& MasterP
              , int Idx  // index of the IF struct
              , AXI_MasterIFimpl<NegEdge>* MasterIFimpl
              , sc_port<AXI_DecoderIF, 1>& DecoderP
              , int* SIDtoMPID
              , int* MPIDtoSID
              , int* SPIDtoMID
              , int MaxFIFO_Depth
              , int MonitorOn
              , int DataBusBytes
              , int DataBusWords
              , int AID_Width
              );
  AXI_GetRequest(const char* Name
              , int PID  // Port ID (index of the multiport component)
              , AXI_SlaveMport<AXI_MAX_PORT_SIZE>& SlaveP
              , AXI_MasterMport<AXI_MAX_PORT_SIZE>& MasterP
              , int Idx  // index of the IF struct
              , AXI_SlaveIFimpl<NegEdge>* SlaveIFimpl
              , sc_port<AXI_DecoderIF, 1>& DecoderP
              , int* MPIDtoSID
              , int* SPIDtoMID
              , int DataBusBytes
              , int DataBusWords
              , int AID_Width
             );

  // destructor
  ~AXI_GetRequest();

  void GetArequest0(bool& Afree);
  void GetWrequest0(bool& Wfree);
  void GetRresponse0(bool& Rfree);
  void GetBresponse0(bool& Bfree);
  void GetArequest1(bool& Afree);
  void GetWrequest1(bool& Wfree);
  void GetRresponse1(bool& Rfree);
  void GetBresponse1(bool& Bfree);
  void ReqReset();
  void ResReset();

  // data
  Achannel* m_Achannel;
  Wchannel* m_Wchannel;
  Rchannel* m_Rchannel;
  Bchannel* m_Bchannel;
  int m_MasterPID_A;
  int m_MasterPID_W;
  int m_SlavePID_R;
  int m_ERID;
  int m_SlavePID_B;
  int m_EBID;
  bool m_Arequest;
  bool m_Wrequest;
  bool m_Rrequest;
  bool m_Brequest;

private:
  // parameter
  const char* m_Name;
  int m_PID;
  AXI_SlaveMport<AXI_MAX_PORT_SIZE>& m_SlaveP;
  AXI_MasterMport<AXI_MAX_PORT_SIZE>& m_MasterP;
  int m_Idx;
  AXI_SlaveIFimpl<NegEdge>* m_SlaveIFimpl;
  AXI_MasterIFimpl<NegEdge>* m_MasterIFimpl;
  sc_port<AXI_DecoderIF, 1>& m_DecoderP;
  int* m_SIDtoMPID;
  int* m_MPIDtoSID;
  int* m_SPIDtoMID;
  int m_DataBusBytes;
  int m_DataBusWords;
  int m_AID_Width;

  // constants
  int m_ID_Mask;

  // data
  AXI_FIFO_M<int>** m_MPID_FIFO;
  int m_NumFIFOs;
  char** m_Str;

  // states
  axi_get_state m_Astate;
  axi_getw_state m_Wstate;
  axi_get_state m_Rstate;
  axi_get_state m_Bstate;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_GET_REQUEST_H
