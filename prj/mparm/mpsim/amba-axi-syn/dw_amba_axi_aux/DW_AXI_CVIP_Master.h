// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 master example.
//    - Example master using the generic AXI_CVIP_Master class
//      that implements the VIP AXI master command IF
// ============================================================================

#ifndef _DW_AXI_CVIP_MASTER_H
#define _DW_AXI_CVIP_MASTER_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_cvip_master.h"

// This module is a master example that processes the address, write data,
// read data, and write response channel in parallel. It uses the
// AXI_CVIP_Master base class that implements a command interface on top of the
// master/slave interface. The model generates read, write and interleave
// buffers and sends them to the underlying master FSM. That master FSM queues
// the transfers and submits them to the attached slave/IC as soon as possible.
// See the module dw_amba_axi/AXI_CVIP_Master for a description of the
// command interface.

template <bool NegEdge, bool Verbose>

class DW_AXI_CVIP_Master // AXI Master Example, using the VIP AXI master command IF 
 : public axi_namespace::AXI_CVIP_Master<NegEdge, Verbose>
#ifdef __EDG__
, public sc_interface
#endif
{
public:

// Aclk, Aresetn, and MasterP are inherited from AXI_CVIP_Master

SC_HAS_PROCESS(DW_AXI_CVIP_Master);

DW_AXI_CVIP_Master(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority // Priority of the master.
               // Must satisfy: Priority >= 0
      , axi_namespace::axi_addr_t Addr1
      , axi_namespace::axi_addr_t Addr2
      , axi_namespace::axi_addr_t Addr3
      , axi_namespace::axi_addr_t Addr4
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system.
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      , int MaxXactBuffers
      , int MaxIntlBuffers
      , int AID_Width
      , int MaxRequests // Maximum number of outstanding requests
      , int WriteIntlDepth
      , bool RreadyDefault     = false
      , bool BreadyDefault     = false
      );
~DW_AXI_CVIP_Master();

// SystemC processes
void Master();
void AresetMs(); // Note: Areset() already used in IF impl

// Slave IF methods are inherited from AXI_CVIP_Master
// Direct access method
virtual bool GetMasterParam(int Idx, int& Priority);

private:
// methods
void end_of_elaboration();
void Initialize();
void CreateBuffer();

// parameter
int m_ID;
int m_Priority;
axi_namespace::axi_addr_t m_Addr1;
axi_namespace::axi_addr_t m_Addr2;
axi_namespace::axi_addr_t m_Addr3;
axi_namespace::axi_addr_t m_Addr4;
int m_DataBusBytes;

// constants
int m_IntlDepth;

// data
int* m_Bh;
int* m_Ch;
int* m_Rh;

// states
bool m_Reset;
sc_event m_ResetEvent;
bool m_CreateBuffer;
};
#endif //_DW_AXI_CVIP_MASTER_H
