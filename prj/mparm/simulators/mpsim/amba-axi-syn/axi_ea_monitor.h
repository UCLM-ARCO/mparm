// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 09/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Exclusive Access Monitor
//    - Monitors the slave memory for exclusive access
// ============================================================================

#ifndef _AXI_EA_MONITOR_H
#define _AXI_EA_MONITOR_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_EA_Monitor
{
public: 
AXI_EA_Monitor(const char* Name
              , int DataBusBytes
              );
~AXI_EA_Monitor();
void ExRead(int AID, axi_addr_t Addr, int Asize);
void Write(axi_addr_t Addr, const Wchannel& W);
bool ExWrite(int AID, axi_addr_t Addr);
void Reset();

private:
  // parameter
  const char* m_Name;

  //constants
  int m_WstrbWords;
  
  // data
  typedef struct {
    bool Monitor;
    int  AID;
    axi_addr_t Addr;
    axi_addr_t BusAlignedAddr;
    axi_wstrb_bt Strb[4];
  } AXI_EA_MonitorT;

  AXI_EA_MonitorT* m_Monitor;
  sc_biguint<128> m_Strb;
  int m_Idx;
  axi_addr_t m_Mask;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_EA_MONITOR_H
