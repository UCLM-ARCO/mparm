// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Memory Class
//    - (Slave) memory allocation and access methods
//    - The memory access methods can be re-used for the
//      slave direct IF implementation
// ============================================================================

#ifndef _AXI_MEMORY_IMPL_H
#define _AXI_MEMORY_IMPL_H

#include "axi_globals.h"
#include "axi_ea_monitor.h"
#include "axi_memory_monitor_impl.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_MemoryImpl
{
public:

AXI_MemoryImpl(const char* Name
      , axi_addr_t StartAddress // Start address of the slaves memory.
      , axi_addr_t EndAddress   // End address of the slaves memory.
      , const sc_string& FileName // File name for the slave memory
               // initialization file, if no name is specified the memory
               // is initialized with zeros.
      , bool BigEndian // Flag specifying whether the data in the slave
               // memory initialization file are interpreted as big or
               // little endian data. Note that the slave memory itself is
               // always little endian.
      , bool ExclusiveAccess // Flag specifying if the slave supports
               // exclusive access (Alock = 1).
      , bool DefaultSlave // Flag specifying if the slave is the default slave
               // in the AXI system. There should be only one default slave
               // per AXI system. The default slave gets all requests for
               // which the decoder does not find a valid slave.
      , bool MonitorOn // Flag specifying if a monitor, displaying
               // the memory content, should be created.
      , int DataBusBytes// Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system
      , int DataBusWords // (DataBusBytes < 4) ? 1 : DataBusBytes / 4;
              );
~AXI_MemoryImpl();

// Memory access methods
bool PutDirect(int ID, bool Write, axi_addr_t Addr,
               axi_data_t Data, int NumBytes);
bool PutDirect(int ID, int BeatNumber, Achannel &A, const Wchannel& W,
               axi_resp_t& Status);
bool PutDirect(int ID, int BeatNumber, Achannel &A, Rchannel &R,
               axi_resp_t& Status);

void Reset();
void ReadFile(const sc_string& fn);

axi_data_t m_Memory;

private:
// parameter
const char* m_Name;
axi_addr_t m_StartAddress;
axi_addr_t m_EndAddress;
sc_string m_FileName;
bool m_BigEndian;
bool m_EA;
bool m_DefaultSlave;
bool m_MonitorOn;
int m_DataBusBytes;
int m_DataBusWords;

// constants
axi_addr_t m_BAMask;
int m_MemoryLength;
int m_WstrbWords;
int m_WstrbBytes;
int m_BitMask;

// data
AXI_EA_Monitor* m_EA_Monitor;
// Monitor
AXI_MemoryMonitorImpl* m_Monitor;
axi_wstrb_bt m_Wstrb[4];

};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif //_AXI_MEMORY_IMPL_H
