// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Arbiter Interface
//    - Is part of the AXI API 
//    - Methods:
//      . Initialize(), use for initialization
//      . Reset(), call at the beginning of the simulation and if Aresetn = low
//      . Arbitrate(), call for arbitration
//        Arbitrate() must select from the array of ID's and the
//        corresponding priorities a winner ID
// ============================================================================

#ifndef _AXI_ARBITER_IF_H
#define _AXI_ARBITER_IF_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

//--------------------------------------------------------
//
//  INTERFACE : Arbiter interface
//--------------------------------------------------------

class AXI_ArbiterIF
  : virtual public sc_interface
{
public:
  virtual void Initialize() = 0;
  virtual void Reset() = 0;
  virtual int Arbitrate( // Returns the index from Idx[] array that wins
              axi_channel_t, // Identifier to specify for which of the 4 AXI
                // channels (address, read, write, response) arbitration is
                // performed. This allows to implement different arbitration
                // schemes for different AXI channels 
              int Length, // Number of items in the Idx[] and Priority[] array
              int* Idx,   // Pointer to array containing the ID's
              int* Priority // Pointer to array containing the Priorities
                       ) = 0;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_ARBITER_IF_H
