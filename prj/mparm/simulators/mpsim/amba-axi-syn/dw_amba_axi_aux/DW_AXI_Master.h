// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 09/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 master example.
//    - Example master using external master state machine
//    - The 4 AXI channels (address, read, write, response)
//      are served in parallel
// ============================================================================

#ifndef _DW_AXI_MASTER_H
#define _DW_AXI_MASTER_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_ma_request.h"

// This module is a master example that processes the address, write data,
// read data, and write response channel in parallel.
template <bool NegEdge, bool Verbose>

class DW_AXI_Master // AXI Master Example, Parallel Handling of the 4 AXI Channels
 : public axi_namespace::AXI_SlaveIFimpl<NegEdge>
#ifdef __EDG__
, public sc_interface
#endif
{
public:

// Master port that must be connected to a master interface.
axi_namespace::AXI_MasterPort MasterP;

SC_HAS_PROCESS(DW_AXI_Master);

DW_AXI_Master(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority // Priority of the master.
               // Must satisfy: Priority >= 0
      , int WriteReorderDepth // WriteReorderDepth > 1 enables re-ordering
               // of data transmission compared to the order of submitted 
               // addresses. E.g. The data for the second address may be
               // transmitted before the data for the first address.
               // Must satisfy (Limitation): WriteReorderDepth == 1
      , int TimeOut // Time out value, used for write data reordering
               // if the data for the second address should be
               // transmitted before the data for the first address
               // The master FSM waits at most Timeout cycles for the
               // second address. Must satisfy: TimeOut >= 0
      , int NumWriteWaits // Number of cycles the master FSM waits before
               // transmitting the next write data.
               // Must satisfy: NumWriteWaits >= 0
      , int NumReadWaits // Number of cycles the master FSM waits before
               // accepting the next read data.
               // Must satisfy: NumReadWaits >= 0
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system.
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      );
~DW_AXI_Master();

// processes
void Master();
void AresetMa(); // Note: Areset() already used in IF impl

// methods
void Reset();

// Direct access method
virtual bool GetMasterParam(int Idx, int& Priority);

private:

// parameter
int m_ID;
int m_Priority;
int m_DataBusBytes;

// constants
int m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int
// data
// FIFO for addresses
axi_namespace::AXI_FIFO<axi_namespace::Achannel>* m_A_FIFO;
// FIFO for write addresses
axi_namespace::AXI_FIFO<axi_namespace::Achannel>* m_AwFIFO;
// FIFO for write data
axi_namespace::AXI_FIFO_P<axi_namespace::Wchannel>** m_W_FIFO;
// FIFO for read data
axi_namespace::AXI_FIFO_P<axi_namespace::Rchannel>* m_R_FIFO;
// FIFO for Bresponse data
axi_namespace::AXI_FIFO<axi_namespace::Bchannel>* m_B_FIFO;

axi_namespace::AXI_MaRequest<NegEdge, Verbose>*  m_MasterFSM;

axi_namespace::Achannel* m_Ain;
axi_namespace::Achannel* m_Aout;
axi_namespace::Bchannel* m_Bold;
axi_namespace::Bchannel* m_Bcurr;
axi_namespace::Wchannel* m_Win;
axi_namespace::Wchannel* m_Wout;
axi_namespace::Rchannel* m_Rold;
axi_namespace::Rchannel* m_Rcurr;

// states
bool m_Reset;
int m_AddrCount[axi_namespace::AXI_MAX_VC_SIZE];
int m_AdelayC[axi_namespace::AXI_MAX_VC_SIZE];
int m_WdelayC[axi_namespace::AXI_MAX_VC_SIZE];
int m_RdelayC[axi_namespace::AXI_MAX_VC_SIZE];
int m_WburstCount[axi_namespace::AXI_MAX_VC_SIZE];
bool m_WaddrValid[axi_namespace::AXI_MAX_VC_SIZE];
int m_WID[axi_namespace::AXI_MAX_VC_SIZE];
int m_Alen[axi_namespace::AXI_MAX_VC_SIZE];
int m_i[axi_namespace::AXI_MAX_VC_SIZE];
int m_VC1;
int m_VC2;

int m_NumWrites[axi_namespace::AXI_MAX_VC_SIZE];
int m_Adelay[axi_namespace::AXI_MAX_VC_SIZE];
int m_Wdelay[axi_namespace::AXI_MAX_VC_SIZE];
int m_Rdelay[axi_namespace::AXI_MAX_VC_SIZE];
int m_Len[axi_namespace::AXI_MAX_VC_SIZE];
axi_namespace::axi_asize_t m_Size[axi_namespace::AXI_MAX_VC_SIZE];
axi_namespace::axi_wstrb_bt m_Wstrb[axi_namespace::AXI_MAX_VC_SIZE];
int m_Aid[axi_namespace::AXI_MAX_VC_SIZE];
axi_namespace::axi_addr_t m_Addr[axi_namespace::AXI_MAX_VC_SIZE];
axi_namespace::axi_aburst_t m_Burst[axi_namespace::AXI_MAX_VC_SIZE];
axi_namespace::axi_alock_t m_Lock[axi_namespace::AXI_MAX_VC_SIZE];
int m_LockBurstLen[axi_namespace::AXI_MAX_VC_SIZE];
};
#endif //_DW_AXI_MASTER_H
