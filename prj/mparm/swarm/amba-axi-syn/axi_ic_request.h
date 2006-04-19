// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI IC Request Class
//    - Request handling of an interconnect
//    - Samples and stores the master request at all ports
//    - Arbitartion 
//    - Sends request to slave
// ============================================================================

#ifndef _AXI_IC_REQUEST_H
#define _AXI_IC_REQUEST_H

#include "axi_master_port.h"
#include "axi_slave_if_impl.h"
#include "axi_get_request.h"
#include "axi_ic_arbiter.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge, bool Verbose> class AXI_IC_Request
{
public:

  // constructor
  AXI_IC_Request(const char* Name
              , AXI_SlaveMport<AXI_MAX_PORT_SIZE>& SlaveP
              , AXI_MasterMport<AXI_MAX_PORT_SIZE>& MasterP
              , AXI_SlaveIFimpl<NegEdge>*  SlaveIFimpl
              , AXI_MasterIFimpl<NegEdge>* MasterIFimpl
              , int* SPIDtoMID
              , int* MPIDtoSID
              , int* SIDtoMPID
              , int* MIDtoSPID
              , AXI_IC_Line* Aline
              , AXI_IC_Line* Wline
              , AXI_IC_Line* Rline
              , AXI_IC_Line* Bline
              , axi_data_t* Apriority
              , axi_data_t* Wpriority
              , axi_data_t* Rpriority
              , axi_data_t* Bpriority
              , int NumSports
              , int NumMports
              , int NumAlines
              , int NumWlines
              , int NumRlines
              , int NumBlines
              , sc_port<AXI_DecoderIF, 1>& DecoderP
              , sc_port<AXI_ArbiterIF, 1>& ArbiterP
              , int DataBusBytes
              , int DataBusWords
              , int AID_Width
              , int MaxFIFO_Depth
              , int* ItrlDepth
              , bool MonitorOn
              );

  // destructor
  ~AXI_IC_Request();

  void Request();
  void Response();
  void Reset();

private:
  // parameter
  const char* m_Name;
  AXI_SlaveMport<AXI_MAX_PORT_SIZE>& m_SlaveP;
  AXI_MasterMport<AXI_MAX_PORT_SIZE>& m_MasterP;
  AXI_SlaveIFimpl<NegEdge>*  m_SlaveIFimpl;
  AXI_MasterIFimpl<NegEdge>* m_MasterIFimpl;
  int* m_SPIDtoMID;
  int* m_MPIDtoSID;
  AXI_IC_Line* m_Aline;
  AXI_IC_Line* m_Wline;
  AXI_IC_Line* m_Rline;
  AXI_IC_Line* m_Bline;
  axi_data_t* m_Apriority;
  axi_data_t* m_Wpriority;
  axi_data_t* m_Rpriority;
  axi_data_t* m_Bpriority;
  int m_NumSports;
  int m_NumMports;
  int m_NumAlines;
  int m_NumWlines;
  int m_NumRlines;
  int m_NumBlines;
  int m_DataBusBytes;
  int m_DataBusWords;
  
  // data
  bool* m_AinputStageFree;
  bool* m_WinputStageFree;
  AXI_GetRequest<NegEdge, Verbose>** m_Request; // Request[j] is Request at IF[j]
  bool* m_AoutputStageFree;
  bool* m_WoutputStageFree;
  bool* m_RinputStageFree;
  bool* m_BinputStageFree;
  AXI_GetRequest<NegEdge, Verbose>** m_Response;
  bool* m_RoutputStageFree;
  bool* m_BoutputStageFree;
  AXI_IC_Arbiter<NegEdge, Verbose>* m_AXI_Arbiter;

  // line busy arrays
  bool m_AlineFree[AXI_MAX_IC_LINE_SIZE];
  int m_AlineIdx[AXI_MAX_PORT_SIZE];
  bool m_WlineFree[AXI_MAX_IC_LINE_SIZE];
  int m_WlineIdx[AXI_MAX_PORT_SIZE];
  bool m_RlineFree[AXI_MAX_IC_LINE_SIZE];
  int m_RlineIdx[AXI_MAX_PORT_SIZE];
  bool m_BlineFree[AXI_MAX_IC_LINE_SIZE];
  int m_BlineIdx[AXI_MAX_PORT_SIZE];

  // Statistics
  double m_Abusy[AXI_MAX_IC_LINE_SIZE];
  double m_Wbusy[AXI_MAX_IC_LINE_SIZE];
  double m_Rbusy[AXI_MAX_IC_LINE_SIZE];
  double m_Bbusy[AXI_MAX_IC_LINE_SIZE];
  double m_ClockTicks;

  // constants
  Achannel m_A;
  Wchannel* m_W;
  Rchannel* m_R;
  Bchannel m_B;

  // temp;
  Achannel *m_tA;
  Wchannel *m_tW;
  Rchannel *m_tR;
  Bchannel *m_tB;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_IC_REQUEST_H
