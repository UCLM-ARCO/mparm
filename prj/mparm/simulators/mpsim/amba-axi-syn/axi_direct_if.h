// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Direct Interface
//    - Is part of the AXI API (conditional mandatory) 
//    - Slave direct IF methods:
//      . GetMasterParam(), use to report the masters priority to the IC
//    - Master direct IF methods:
//      . 3 MputDirect() methods
//         general purpose, for write channel data, and for read channel data
//      . GetSlaveParam(), use to report the slaves write interleaving depth
//        and address map to the IC/master
// ============================================================================

#ifndef _AXI_DIRECT_IF_H
#define _AXI_DIRECT_IF_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

//--------------------------------------------------------
//
//  INTERFACE : Direct read/write interface
//--------------------------------------------------------

class AXI_SdirectIF
  : virtual public sc_interface
{
public:
  // Direct access method Slave to Master direction.
//  virtual bool PutDirect(      // Returns the status
//               int ID,          // Identifier
//               bool Write,      // true = write access,
//                                // false = read access
//               int AID,          // AID
 //              axi_addr_t Addr, // Address
//               axi_data_t Data, // Pointer to the data
//               int NumBytes     // Number of bytes
//                           )
//               {return (false);}
  virtual bool GetMasterParam( // Returns the status
               int Idx,        // Identifier
               int& Priority   // Priority of the slave
                             )
  {
    cout << " WARNING: Master has not implemented GetMasterParam() " << endl;
    cout << "          Default implementation is used " << endl;
    return(false);
  }
};

class AXI_MdirectIF
  : virtual public sc_interface
{
public:
  // Direct access method Master to Slave direction.
  virtual bool PutDirect(      // Returns the status
               int ID,          // Identifier
               bool Write,      // true = write access,
                                // false = read access
               axi_addr_t Addr, // Address
               axi_data_t Data, // Pointer to the data
               int NumBytes     // Number of bytes
                           )
  {
    cout << " WARNING: Slave has not implemented PutDirect() " << endl;
    cout << "          Default implementation is used " << endl;
    return(false);
  }
  virtual bool PutDirect(         // Returns the status
               int ID,            // Identifier
               int BeatNumber,    // Beat number within the burst
               Achannel &A,       // Address channel data
               const Wchannel& W, // Write channel data
               axi_resp_t& Status  // AXI status
                           )
  {
    cout << " WARNING: Slave has not implemented PutDirect() " << endl;
    cout << "          Default implementation is used " << endl;
    return(false);
  }
  virtual bool PutDirect(        // Returns the status
               int ID,           // Identifier
               int BeatNumber,   // Beat number within the burst
               Achannel &A,      // Address channel data
               Rchannel &R,      // Read channel data
               axi_resp_t& Status // AXI status
                           )
  {
    cout << " WARNING: Slave has not implemented PutDirect() " << endl;
    cout << "          Default implementation is used " << endl;
    return(false);
  }
  virtual bool GetSlaveParam(   // Returns the status
               int Idx,         // Identifier
               int& ItrlDepth,  // Write interleaving depth
               AXI_AddressMap&  // Address map
                             )
  {
    cout << " WARNING: Slave has not implemented GetSlaveParam() " << endl;
    cout << "          Default implementation is used " << endl;
    return(false);
  }
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_DIRECT_IF_H
