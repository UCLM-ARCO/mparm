// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Implementation of the AXI slave IF
// ============================================================================

#ifndef _AXI_SLAVE_IF_IMPL_H
#define _AXI_SLAVE_IF_IMPL_H

#include "axi_channel.h"
#include "axi_slave_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template <bool NegEdge> class AXI_SlaveIFimpl // Slave Interface Implementation
  : public sc_module
  , public AXI_SlaveIF
{
public:
  // Global clock signal. 
  // All AXI channels must be sampled at the rising edge of the global clock.
  sc_in_clk Aclk;

  // Global reset signal. This signal is active LOW.
  sc_in<bool> Aresetn;

  SC_HAS_PROCESS(AXI_SlaveIFimpl);

  // constructor
  AXI_SlaveIFimpl(
        sc_module_name name_
       , int MasterID // Master ID
       , int NumPorts // NumPorts = 1 for single ported master
       , axi_idx_mode_t IdxMode // IdxMode = AXI_IM_SP for single ported master
       , int DataBusBytes // Number of read/write byte lanes of the AXI IC
       , int DataBusWords // (DataBusBytes < 4) ? 1 : DataBusBytes / 4;
       , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
                  );

  // destructor
  ~AXI_SlaveIFimpl();

  // Slave IF methods
  virtual void PutChannel(int Idx, const Rchannel&);
  virtual void PutChannel(int Idx, const Bchannel&);
  virtual void PutChannel(int Idx, const AreadySig&);
  virtual void PutChannel(int Idx, const WreadySig&);
  virtual int GetSlaveIdx(int SlaveID, int& MasterID);
  virtual void register_port(sc_port_base& port, const char* if_typename);

  // read access methods
  void GetChannel(int Idx, Rchannel&); // use Idx = 0, for single ported masters
  void GetChannel(int Idx, Bchannel&);
  void GetChannel(int Idx, AreadySig&);
  void GetChannel(int Idx, WreadySig&);
  bool GetRvalid(int Idx);
  bool GetBvalid(int Idx);             // use Idx = 0, for single ported masters

  bool m_SlaveID[AXI_MAX_PORT_SIZE];

private :
  void end_of_elaboration();
  // Allocate the memory
  void Instantiate(int NumPorts, int DataBusBytes, int DataBusWords);

  void Toggle();
  void Areset();
  void Reset();

  // Parameter
  int m_MasterID;
  int m_NumPorts; // Number of AXI xhannels, allocated by the interface
                  // Must be greater than or equal to the number of ports
                  // attached to the interface
  axi_idx_mode_t m_IdxMode; // IF index selection mode
  double m_ClockPeriod; // Clock period [seconds per cycle]

  AXI_Channel<Rchannel>  *m_Rchannel;
  AXI_Channel<Bchannel>  *m_Bchannel;
  AXI_Channel<AreadySig> *m_Aready;
  AXI_Channel<WreadySig> *m_Wready;

  int m_NumBinds; // Number of ports attached to the interface
  bool m_Reset;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_SLAVE_IF_IMPL_H
