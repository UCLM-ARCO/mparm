// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Example adress decoder module
//   - Single address map
//   - Address map must be specified in the slaves (and not in the decoder)
// ============================================================================

#include "DW_AXI_Decoder.h"

using namespace axi_namespace;

DW_AXI_Decoder::
DW_AXI_Decoder(sc_module_name Name)
           : sc_module   (Name)
{
  Reset();
}

DW_AXI_Decoder::~DW_AXI_Decoder() {
}


int DW_AXI_Decoder::Decode(axi_addr_t& A) {
  for (int i = 0; i < m_M->GetSize(); i++) {
    const AXI_AddressRegion& Ar = (*m_M)[i];
    if ((Ar.StartAddr <= A) && (Ar.EndAddr >= A))
      return(Ar.SlaveID);
  }
  return(m_DefaultSlaveID);
}

void DW_AXI_Decoder::Initialize(const AXI_AddressMap& M) {
  m_M = &M;
  if ((m_DefaultSlaveID = M.GetDefaultSlave()) < 0) {
    cerr << "AXI Decoder ERROR: Default slave is not specified"
          << endl;
    sc_assert(m_DefaultSlaveID >= 0);
  }
}

void DW_AXI_Decoder::Reset() {
}

