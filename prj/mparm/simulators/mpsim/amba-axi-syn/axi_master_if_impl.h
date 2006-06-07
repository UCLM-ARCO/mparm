// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Implementation of the AXI master IF
// ============================================================================

#ifndef _AXI_MASTER_IF_IMPL_H
#define _AXI_MASTER_IF_IMPL_H

#include "axi_channel.h"
#include "axi_master_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge> class AXI_MasterIFimpl // Master Interface Implementation
  : public sc_module
  , public AXI_MasterIF
{
public:
  // Global clock signal. 
  // All AXI channels must be sampled at the rising edge of the global clock.
  sc_in_clk   Aclk;

  // Global reset signal. This signal is active LOW.
  sc_in<bool> Aresetn;

  SC_HAS_PROCESS(AXI_MasterIFimpl);

  // constructor
  AXI_MasterIFimpl(
        sc_module_name name_
       , int SlaveID // Slave ID
       , int NumPorts // NumPorts = 1 for single ported slave
       , axi_idx_mode_t IdxMode // IdxMode = AXI_IM_SP for single ported slave
       , int DataBusBytes // Number of read/write byte lanes of the AXI IC
       , int DataBusWords // (DataBusBytes < 4) ? 1 : DataBusBytes / 4;
       , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
                  );

  // destructor
  ~AXI_MasterIFimpl();

  // Master IF methods
  virtual void PutChannel(int Idx, const Achannel&);
  virtual void PutChannel(int Idx, const Wchannel&);
  virtual void PutChannel(int Idx, const RreadySig&);
  virtual void PutChannel(int Idx, const BreadySig&);
  virtual int GetMasterIdx(int MasterID, int& SlaveID);
  virtual void register_port(sc_port_base& port, const char* if_typename);

  // read access methods
  void GetChannel(int Idx, Achannel&); // use Idx = 0, for single ported slave
  void GetChannel(int Idx, Wchannel&);
  void GetChannel(int Idx, RreadySig&);
  void GetChannel(int Idx, BreadySig&);
  bool GetAvalid(int Idx);             // use Idx = 0, for single ported slave
  bool GetWvalid(int Idx);

  bool m_MasterID[AXI_MAX_PORT_SIZE];

private :
  void end_of_elaboration();
  // Allocate the memory
  void Instantiate(int NumPorts, int DataBusBytes, int DataBusWords);

  void Toggle();
  void Areset();
  void Reset();

  // Parameter
  int m_SlaveID;
  int m_NumPorts; // Number of AXI xhannels, allocated by the interface
                  // Must be greater than or equal to the number of ports
                  // attached to the interface
  axi_idx_mode_t m_IdxMode; // IF index selection mode
  double m_ClockPeriod; // Clock period [seconds per cycle]

  AXI_Channel<Achannel>  *m_Achannel;
  AXI_Channel<Wchannel>  *m_Wchannel;
  AXI_Channel<RreadySig> *m_Rready;
  AXI_Channel<BreadySig> *m_Bready;

  int m_NumBinds; // Number of ports attached to the interface
  bool m_Reset;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_MASTER_IF_IMPL_H
