// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Example address decoder module
//   - Single address map
//   - Address map must be specified in the slaves (and not in the decoder)
// ============================================================================

#ifndef _DW_AXI_DECODER_H
#define _DW_AXI_DECODER_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_decoder_if.h"

// This module is an example for an address decoder with single address map.
// The address map and the ID of the default slave is provided by the IC.
// The decoder returns the ID of the addressed slave.
//
// Other decoders are possible. E.g the address map and default slave ID
// may be specified at the decoder, or the decoder may support more than
// one address map. This can be done through a remap port at the decoder.
class DW_AXI_Decoder // Address Decoder, Single Address Map
  : public sc_module
  , public axi_namespace::AXI_DecoderIF
{

public:
  DW_AXI_Decoder(sc_module_name Name);
  ~DW_AXI_Decoder();

  void Initialize(const axi_namespace::AXI_AddressMap& M);
  void Reset();
  int Decode(axi_namespace::axi_addr_t& A);

private:
  const axi_namespace::AXI_AddressMap* m_M;
  int m_DefaultSlaveID;
};

#endif // _DW_AXI_DECODER_H
