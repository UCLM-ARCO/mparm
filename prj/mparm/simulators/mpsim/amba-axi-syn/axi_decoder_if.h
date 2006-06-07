// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Decoder Interface
//    - Is part of the AXI API 
//    - Methods:
//      . Initialize(), use for initialization, pass address map to decoder
//      . Reset(), call at the beginning of the simulation and if Aresetn = low
//      . Decode(), call for address decoding, if address remapping is
//        enabled, return the corresponding normal mode address
// ============================================================================

#ifndef _AXI_DECODER_IF_H
#define _AXI_DECODER_IF_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

//--------------------------------------------------------
//
//  INTERFACE : Decoder interface
//--------------------------------------------------------

class AXI_DecoderIF
  : virtual public sc_interface
{
public:
  virtual void Initialize(const AXI_AddressMap& M) = 0;
  virtual void Reset() = 0;
  virtual int Decode( // Returns the ID of the addressed Slave
                     axi_addr_t& A // Address that should be decoded, if this
                       // is a remapped address, return the corresponding
                       // default mode address 
                     ) = 0;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_DECODER_IF_H
