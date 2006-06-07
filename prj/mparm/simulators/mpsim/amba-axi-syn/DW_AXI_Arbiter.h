// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Example arbitration module
//    - Simple 2 tier scheme:
//      First tier: Priority based
//      Second tier: Fair among equals,
//                   based on number of accesses to the AXI channel
// ============================================================================

#ifndef _DW_AXI_ARBITER_H
#define _DW_AXI_ARBITER_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_arbiter_if.h"

// This module is a simple arbiter using a 2 tier arbitration scheme.
// The first tier arbitration scheme selects the winner based on
// highest priorities, where higher numbers mean higher priorities.
// If there is no unique winner, then the second tier arbitration scheme
// is used. The winner is selected using a fair among equals scheme based
// on the number of previous transfers.
//
// The API between IC and arbiter is as follows:
// The IC provides for each arbitration an array of indices and an array
// of corresponding priorities. The arbiter returns the winning index.
class DW_AXI_Arbiter // Arbiter with 2 Tier Arbitration Scheme
  : public sc_module
  , public axi_namespace::AXI_ArbiterIF
{

public:
  DW_AXI_Arbiter(sc_module_name Name);
  ~DW_AXI_Arbiter();

  void Initialize();
  void Reset();
  int Arbitrate(axi_namespace::axi_channel_t T,
                int Length, int* Idx, int* Priority);

private:
  int** m_NumGrants;
};
#endif // _DW_AXI_ARBITER_H
