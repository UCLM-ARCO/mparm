// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Arbiter Class
//                - used in IC
//                - configures the Arbiter parameter, calls Arbiter module
//                - Selects the Master who gets the grant
// ============================================================================

#ifndef _AXI_IC_ARBITER_H
#define _AXI_IC_ARBITER_H

#include "axi_get_request.h"
#include "axi_arbiter_if.h"
#include "axi_aid_fifo.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge, bool Verbose> class AXI_IC_Arbiter
{
public:

  // constructor
  AXI_IC_Arbiter(const char* Name
            , sc_port<AXI_ArbiterIF, 1>& ArbiterP
            , AXI_GetRequest<NegEdge, Verbose>** Request
            , bool* AoutputStageFree
            , bool* WoutputStageFree
            , AXI_GetRequest<NegEdge, Verbose>** Response
            , bool* RoutputStageFree
            , bool* BoutputStageFree
            , int* SPIDtoMID
            , int MaxFIFO_Depth
            , int* ItrlDepth
            , bool MonitorOn
              );

  // destructor
  ~AXI_IC_Arbiter();

  // methods
  int ArbitrateA(AXI_IC_Line& line, axi_data_t priority);
  int ArbitrateW(const AXI_IC_Line& line, axi_data_t priority);
  int ArbitrateR(const AXI_IC_Line& line, axi_data_t priority);
  int ArbitrateB(const AXI_IC_Line& line, axi_data_t priority);
  void Reset();

  // public data
  AXI_WID_FIFO* m_WID_Order;

private:

  // parameter
  const char* m_Name;
  sc_port<AXI_ArbiterIF, 1>& m_ArbiterP;
  AXI_GetRequest<NegEdge, Verbose>** m_Request;
  bool* m_AoutputStageFree;
  bool* m_WoutputStageFree;
  AXI_GetRequest<NegEdge, Verbose>** m_Response;
  bool* m_RoutputStageFree;
  bool* m_BoutputStageFree;
  int* m_SPIDtoMID;

  // constants
  // int m_OpenReqLength;

  // tmp variables
  int* Idx;
  int* P;

  // data
  AXI_ID_FIFO* m_RID_Order;
  AXI_ID_FIFO* m_BID_Order;
  AXI_Lock* m_Lock;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_IC_ARBITER_H
