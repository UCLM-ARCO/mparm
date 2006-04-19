// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Slave Interface
//    - Is part of the AXI API (mandatory) 
//    - Methods:
//      . PutChannel(), for Read channel data, Response channel data,
//        the Aready signal, and the Wready signal
//      . GetSlaveIdx(), used by the specialized port to get the indentifier
//        that is used in the PutChannel() method calls
// ============================================================================

#ifndef _AXI_SLAVE_IF_H
#define _AXI_SLAVE_IF_H

#include "axi_direct_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_SlaveIF
      : public AXI_SdirectIF
{
public:
  virtual void PutChannel(
               int Idx,        // Identifier
               const Rchannel& // Read channel data
                         )  = 0;
  virtual void PutChannel(
               int Idx,        // Identifier
               const Bchannel& // Response channel data
                         )  = 0;
  virtual void PutChannel(
               int Idx,         // Identifier
               const AreadySig& // Aready signal
                         ) = 0;
  virtual void PutChannel(
               int Idx,         // Identifier
               const WreadySig& // Wready signal
                          ) = 0;
  virtual int GetSlaveIdx(  // Returns the identifier, that is used by
                            // subsequent PutChannel() method calls
              int SlaveID,  // ID of the calling slave
              int& MasterID // Returns the ID of the called master
                         ) = 0;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_SLAVE_IF_H
