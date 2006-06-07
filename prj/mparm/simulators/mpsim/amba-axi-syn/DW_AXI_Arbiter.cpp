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

#include "DW_AXI_Arbiter.h"

using namespace axi_namespace;

DW_AXI_Arbiter::
DW_AXI_Arbiter(sc_module_name Name)
           : sc_module   (Name)
{
  m_NumGrants = new int*[4];
  for (int i = 0; i < 4; i++)
    m_NumGrants[i] = new int[AXI_MAX_PORT_SIZE];
  Reset();
}

DW_AXI_Arbiter::~DW_AXI_Arbiter()
{
  for (int i = 0; i < 4; i++)
    delete [] m_NumGrants[i];
  delete [] m_NumGrants;
}


int DW_AXI_Arbiter::Arbitrate(axi_channel_t T,
                             int Length, int* Idx, int* Priority)
{
  int CurrPriority = 0;
  int GrantIdx    = -1;
  int *NumGrants = m_NumGrants[T];

  for (int i = 0; i < Length; i++) {
    if (Priority[i] > CurrPriority) { // first tier arbitration
      GrantIdx = Idx[i];
      CurrPriority = Priority[i];
    }
    else if (Priority[i] == CurrPriority) { // second tier arbitration
      if (NumGrants[Idx[i]] <= NumGrants[GrantIdx])
        GrantIdx = Idx[i];
    }
  }
  if (GrantIdx >= 0) NumGrants[GrantIdx] += 1;
  return (GrantIdx);
}

void DW_AXI_Arbiter::Initialize(){
}

void DW_AXI_Arbiter::Reset()
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < AXI_MAX_PORT_SIZE; j++)
      m_NumGrants[i][j] = 0;
}

