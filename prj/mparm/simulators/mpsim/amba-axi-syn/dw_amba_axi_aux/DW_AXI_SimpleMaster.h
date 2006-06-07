// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 Simple Master Example
//    - Example for usage of Master IF method calls (PutChannel()) and
//      Slave IF implementation class methods (GetChannel()) 
//    - Sequential master
// ============================================================================

#ifndef _DW_AXI_SIMPLEMASTER_H
#define _DW_AXI_SIMPLEMASTER_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_master_port.h"
#include "axi_slave_if_impl.h"

// This module is a master example that shows the usage of Master IF method
// calls (PutChannel()) and Slave IF implementation class methods
// (GetChannel()).
// It is a pure sequential master in the sense that the data busses are only
// served after the address has been accepted.
// Different transactions are generated depending on the ID of the master.
template <bool NegEdge, bool Verbose>

class DW_AXI_SimpleMaster // Master Example for the Usage of Master IF Methods 
 : public axi_namespace::AXI_SlaveIFimpl<NegEdge>
#ifdef __EDG__
, public sc_interface
#endif
{
public:

axi_namespace::AXI_MasterPort MasterP;

SC_HAS_PROCESS(DW_AXI_SimpleMaster);

DW_AXI_SimpleMaster(sc_module_name name_
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority// Priority of the master.
               // Must satisfy: Priority >= 0
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system. 
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      );
~DW_AXI_SimpleMaster();

void Send();
void Areset_SimpleMaster();

void Reset();
virtual bool GetMasterParam(int Idx, int& Priority);

private:

// parameter
const char* m_Name;
int m_ID;
int m_Priority;
int m_DataBusBytes;

// constants
int m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int
int m_WstrbWords;
int m_WstrbBytes;
int m_BitMask;

// states
bool m_Reset;
};
#endif // _DW_AXI_SIMPLEMASTER_H
