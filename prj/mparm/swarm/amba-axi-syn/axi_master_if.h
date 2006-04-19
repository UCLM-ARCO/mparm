// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Master Interface
//    - Is part of the AXI API (mandatory) 
//    - Methods:
//      . PutChannel(), for Address channel data, Write channel data,
//        the Rready signal, and the Bready signal
//      . GetMasterIdx(), used by the specialized port to get the indentifier
//        that is used in the PutChannel() method calls
// ============================================================================

#ifndef _AXI_MASTER_IF_H
#define _AXI_MASTER_IF_H

#include "axi_direct_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_MasterIF
      : public AXI_MdirectIF
{
public:
  virtual void PutChannel(
               int Idx,        // Identifier
               const Achannel& // Address channel data
                          )  = 0;
  virtual void PutChannel(
               int Idx,        // Identifier
               const Wchannel& // Write channel data
                         )  = 0;
  virtual void PutChannel(
               int Idx,         // Identifier
               const RreadySig& // Rready signal
                          ) = 0;
  virtual void PutChannel(
               int Idx,         // Identifier
               const BreadySig& // Bready signal
                          ) = 0;
  virtual int GetMasterIdx(  // Returns the identifier, that is used by
                             // subsequent PutChannel() method calls
               int MasterID, // ID of the calling master
               int& SlaveID  // Returns the ID of the called slave
                           ) = 0;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_MASTER_IF_H
