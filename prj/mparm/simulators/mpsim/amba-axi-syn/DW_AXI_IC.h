// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Interconnection (IC) Class
//   - Top level class implementing the IC
// ============================================================================

#ifndef _DW_AXI_IC_H
#define _DW_AXI_IC_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_config_line.h"
#include "axi_ic_request.h"
#include "DW_AXI_SimpleSlave.h"


// The AXI interconnection is the resource sharing device in an AXI system.
//
// Features
// --------
// - It is configurable with respect to:
//   . Number of IC lines (Address, Write data, Read data, Response lines)
//   . Access to IC line that can be configured for each master/slave
//     individually
//   . Priorities that can be set for each master on IC line basis
// - Arbiter and decoder are external
//
// Constructor Parameters
// ----------------------
// The constructor parameters are described in the header file.
//
// IC Timing
// ---------
// 1.) For each attached master and slave the IC has an input stage of depth one
// that works as follows:
// If a request/response arrives at the IC that can not be forwarded immediately
// it is stored in the input stage. If the input stage is occupied the ready
// signal is de-asserted and no more data is accepted from that particular
// master/slave. The input stage exists for all AXI channels, that are,
// address data, write data, read data, and write response data.
// 2.) A target that has not sampled its previous request/response does not
// get another request/response of same type, even if an additional IC line
// would be available.
// 3.) If data have been sent to a target that particular IC line is blocked
// until the target has sampled the data (indicated by the ready signal).
// This is normal bus behavior.
//
// Arbitration
// -----------
// For data transfers from master to slave direction (requests),
// the priorities of the initiators (masters) are used for arbitration
// to resolve resource conflicts.
// For data transfers from slave to master direction (responses),
// the priorities of the targets (masters) are used for arbitration
// to resolve resource conflicts.
// If more than one IC line is available (e.g. two write data busses),
// arbitration is performed on each IC line separately.
//
// Write Interleaving
// ------------------
// Currently the IC supports only a write interleaving depth of 1
// (at the master side). That means a single master can only issue one
// write burst at the time. Depending on the interleaving depth of the slaves,
// the IC may interleave write bursts from different masters to the same slave.
//
// Debug Output
// -----------
// If the template parameter Verbose is equal to true, the IC outputs
// to stdout the received requests/responses (denoted by "(In)") and
// the requests/responses sent to the target (denoted by "(Out)")
//
// Configuration File Format
// -------------------------
// The configuration file name can be specified to set master
// priorities for each IC line separately or to specify master and
// slave access to IC lines. If no file name is specified the default
// setup is taken. See the parameter description for more details.
// For a configuration file example see the file
// $DW_AMBA_SC/dw_amba_axi/axi_config_3m.txt
// The structure of the file is: 
//   - AXI line name and number (keywords listed under a),
//   - followed by the masters and slaves that have access to that
//     particular AXI line, and the priorities of these masters
//     (keywords listed under b).
// The file format is for each line as follows:
//         Keyword integer number(s)
//
// For comments, use '#' or '//'. If a comment is used the rest of the
// line is ignored.
// Keyword can be:
// a) Aline, Wline, Rline, Bline
//    These keywords must be followed by a single integer number, namely
//    the IC line number, where each (A-, W-, R-, B-) first IC line starts
//    with number 0. For example, if there are two write data lines in the
//    system, Wline number must be either 0 or 1.
//    An AXI line that is in the system but is not listed in the file,
//    uses the default settings.
// b) Master, Slave, Priority
//    These keywords must be followed by several white space separated
//    integer numbers. Note that these keywords must follow an
//    AXI line specification using a keywords form a).
//    If a keyword is not used for a particular AXI line the default
//    setting is used. For masters and slaves, the integer numbers after
//    the keyword represent master and slave ID. This ID must match the ID
//    that masters and slaves report to the IC.
//    The (master) priorities must be specified as on-negative numbers
//    using position matching (first priority belongs to first master).
//
// Example: 1 Aline, 2 Wlines, 2 Rlines, 1 Bline
//
// # Aline is not specified, because we use the default setting
// #
// # Bline is not specified, because we use the default setting
// #
// Wline 0 # it follows the setup for the first write data line in the IC
// Master 1 2 3 # masters with ID = 1, 2, and 3 have access to the first write data line
// Priority  3 3 1 # For the first write data line, 
//                 # master with ID = 1 has priority 3
//                 # master with ID = 2 has priority 3
//                 # master with ID = 3 has priority 1
// # No "Slave" keyword is used, i.e., all slaves of the system
// # can be reached via this line
// #
// Wline 1 # setup for the second write data line in the IC
// Master 1 # exclusive write data line for master 1
// Slave 2 3 # slaves with ID = 2, and 3 can be reached via the first write data line
// #
// Rline 0 # setup for the first read data line in the IC
// Master 1 2 3 # masters with ID = 1, 2, and 3 can be reached via the
//              # first read data line
// Priority  3 2 1 # For the first read data line, 
//                 # master with ID = 1 has priority 3
//                 # master with ID = 2 has priority 2
//                 # master with ID = 3 has priority 1
// Slave 2 3 # slaves with ID = 2, and 3 have access to the first read data line
// #
// Rline 1 # it follows the setup for the second read data line in the IC
// Master 1 # exclusive read data line for master 1
// Slave 2 3 # slaves with ID = 1, and 2 have access the first read data line
// #
//
// Programming Priorities
// ----------------------
// The IC contains a slave that allows to change master priorities
// during run-time. The address map of that slave is for an IC
// with 2 Address, 2 Wdata, 2 Rdata, and 2 Response lines as follows:
// +----------------------------------------------------------------------+
// | Address |     Type | Width |          Description                    |
// | Offset  |          | [bit] |                                         |
// |----------------------------------------------------------------------|
// |    0x00 |     r/w  |     4 | Priority master 0, 1st Address channel  |
// |----------------------------------------------------------------------|
// |    0x04 |     r/w  |     4 | Priority master 1, 1st Address channel  |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |    0x7C |     r/w  |     4 | Priority master 31, 1st Address channel |
// |----------------------------------------------------------------------|
// |    0x80 |     r/w  |     4 | Priority master 0, 2nd Address channel  |
// |----------------------------------------------------------------------|
// |    0x84 |     r/w  |     4 | Priority master 1, 2nd Address channel  |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |    0xFC |     r/w  |     4 | Priority master 31, 2nd Address channel |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x100 |     r/w  |     4 | Priority master 0, 1st Wdata channel    |
// |----------------------------------------------------------------------|
// |   0x104 |     r/w  |     4 | Priority master 1, 1st Wdata channel    |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x17C |     r/w  |     4 | Priority master 31, 1st Wdata channel   |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x200 |     r/w  |     4 | Priority master 0, 1st Rdata channel    |
// |----------------------------------------------------------------------|
// |   0x204 |     r/w  |     4 | Priority master 1, 1st Rdata channel    |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x27C |     r/w  |     4 | Priority master 31, 1st Rdata channel   |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x300 |     r/w  |     4 | Priority master 0, 1st Response channel |
// |----------------------------------------------------------------------|
// |   0x304 |     r/w  |     4 | Priority master 1, 1st Response channel |
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// |   0x37C |     r/w  |     4 | Priority master 31, 1st Response channel|
// |----------------------------------------------------------------------|
// |    ...  |     ...  |   ... |               ...                       |
// |----------------------------------------------------------------------|
// The general address map can be derived from the table.

template <bool NegEdge, bool Verbose>

class DW_AXI_IC // AXI Interconnection Module
  : public sc_module
  , public axi_namespace::AXI_SlaveIF
  , public axi_namespace::AXI_MasterIF
{
public:

// Global clock signal. 
// All AXI channels must be sampled at the rising edge of the global clock.
sc_in_clk Aclk;

// Global reset signal. This signal is active LOW.
sc_in<bool> Aresetn;

// Slave port that must be connected to a slave interface.
axi_namespace::AXI_SlaveMport<axi_namespace::AXI_MAX_PORT_SIZE>  SlaveP;

// Master port that must be connected to a master interface.
axi_namespace::AXI_MasterMport<axi_namespace::AXI_MAX_PORT_SIZE> MasterP;

// Decoder port that must be connected to a decoder interface.
sc_port<axi_namespace::AXI_DecoderIF, 1> DecoderP;

// Arbiter port that must be connected to an arbiter interface.
sc_port<axi_namespace::AXI_ArbiterIF, 1> ArbiterP;

SC_HAS_PROCESS(DW_AXI_IC);

DW_AXI_IC(sc_module_name name_
      , int IC_ID // Unique number among all IC's used in the AXI system
                  // Must satisfy: 0 <= ID <= 31
      , int IC_Priority // Priority of the IC.
                  // Must satisfy: IC_Priority >= 0
      , int IC_IntrlDepth // Number of interleaved write bursts the IC can
               // accept from a single master.
               // Must satisfy: IntrlDepth = 1
      , int NumAlines // Number of address lines of the IC
               // Must satisfy: NumAlines >= 1
      , int NumWlines // Number of write data lines of the IC
               // Must satisfy: NumWlines >= 1
      , int NumRlines // Number of read data lines of the IC
               // Must satisfy: NumRlines >= 1
      , int NumBlines // Number of write data response lines of the IC
               // Must satisfy: NumBlines >= 1
      , const sc_string& ConfigFileName // IC Configuration file name
           // The following settings can be specified in the configuration file:
               // - Individual master priorities for each IC line.
               // - Master access to each IC line.
               // - Slave access to each IC line.
               // See the description section for the file format.
               // If no file name is specified, the default setup is used.
               // The IC default setup is,
               // - Master priorities are taken from the masters.
               // - Masters have the same priority on all IC lines.
               // - All masters/slaves are attached to all IC lines.
      , int SlaveID // ID of the internal slave, storing the priorities
               // of the masters attached to the IC. SlaveID must be
               // a unique number among all slaves attached to the IC.
               // Must satisfy: 0 <= SlaveID <= 31
      , int PriorityStartAddr // Start address of the internal slave
      , int PriorityEndAddr // End address of the internal slave
               // Start and end address must be 4K aligned
               // if PriorityEndAddr <= PriorityStartAddr no internal slave
               // is used, and the master priorities can not be changed
               // during run time.
      , const sc_string& SlaveFileName // File name for the slave memory
               // initialization file, if no name is specified the memory
               // is initialized with zeros. 
      , bool BigEndian // Flag specifying whether the data in the slave
               // memory initialization file are interpreted as big or
               // little endian data. Note that the slave memory itself is
               // always little endian.
      , bool DefaultSlave // Flag specifying if the slave is the default slave
               // in the AXI system. There should be only one default slave
               // per AXI system. The default slave gets all requests for
               // which the decoder does not find a valid slave
      , int DataBusBytes // Number of read/write byte lanes of the AXI IC
               // DataBusBytes is the width of the data bus signals
               // (Rdata/ Wdata) in units of bytes.
               // The same number must be used in the entire AXI system
               // DataBusBytes must be a power of 2.
               // Must satisfy: 1 <= DataBusBytes <= 128
               // Example: IC with 64 bit read/write data bus
               //          DataBusBytes = 8
      , int AID_Width // Width in units of bits of the AID field at the input.
               // Masters should not generate an AID >= (2**AID_Width) - 1
               // AID_Width must be selected such that the
               // width of the extended AID field at the output of the IC
               // does not exceed 13 bits.
               // Must satisfy: 0 <= AID_Width <= 12
               // Recommended values are AID_Width = 4 or 8
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      , int MaxFIFO_Depth // Size of the internal FIFO's.
               // The IC must use FIFO's in order to internally store
               // addresses of pending requests. This is necessary for
               // AXI protocol compliance with respect to the read/write
               // reordering rules. 
      , bool FIFO_MonitorOn // Flag activating the (textual) FIFO monitor.
               // The FIFO monitor outputs information about the maximum
               // FIFO size used during simulation.
      );
~DW_AXI_IC();

  // Slave IF methods
  virtual void PutChannel(int Idx, const axi_namespace::Rchannel&);
  virtual void PutChannel(int Idx, const axi_namespace::Bchannel&);
  virtual void PutChannel(int Idx, const axi_namespace::AreadySig&);
  virtual void PutChannel(int Idx, const axi_namespace::WreadySig&);
  virtual int GetSlaveIdx(int SlaveID, int& MasterID);
  virtual bool GetMasterParam(int Idx, int &P);

// Direct access methods Master to Slave direction.
virtual bool PutDirect(int ID, bool Write,
                       axi_namespace::axi_addr_t Addr,
                       axi_namespace::axi_data_t Data, int NumBytes);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       const axi_namespace::Wchannel& W,
                       axi_namespace::axi_resp_t& Status);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       axi_namespace::Rchannel &R,
                       axi_namespace::axi_resp_t& Status);
  virtual bool GetSlaveParam(int Idx, int& ItrlDepth,
                             axi_namespace::AXI_AddressMap&);


  // Master IF methods
  virtual void PutChannel(int Idx, const axi_namespace::Achannel&);
  virtual void PutChannel(int Idx, const axi_namespace::Wchannel&);
  virtual void PutChannel(int Idx, const axi_namespace::RreadySig&);
  virtual void PutChannel(int Idx, const axi_namespace::BreadySig&);
  virtual int GetMasterIdx(int MasterID, int& SlaveID);

  // SystemC processes
  void Bus();
  void Areset();

  void Reset();
 
private:
   void end_of_elaboration();
  void Initialize();
  void IC_GetMasterParam();
  void IC_GetSlaveParam();
  void IC_Connections(bool Mconn[][axi_namespace::AXI_MAX_PORT_SIZE],
                      bool Sconn[][axi_namespace::AXI_MAX_PORT_SIZE]);
  void GetPriorityPointer(axi_namespace::axi_data_t* P, int NumLines,
                         int RowLength, int MemoryOffset);
  void CopyPriorities(axi_namespace::axi_data_t* P,
                      int NumLines, int RowLength);
  void GetConfiguration(bool Mconn[][axi_namespace::AXI_MAX_PORT_SIZE],
                        bool Sconn[][axi_namespace::AXI_MAX_PORT_SIZE]);
  void ErrorMessage(int Id);

  // parameter
  const char* m_Name;
  int m_IC_ID;
  int m_IC_Priority;
  int m_IC_IntrlDepth;
  int m_NumAlines;
  int m_NumWlines;
  int m_NumRlines;
  int m_NumBlines;
  sc_string m_ConfigFileName;
  int m_DataBusBytes;
  int m_DataBusWords;
  int m_AID_Width;
  double m_ClockFrequency;
  int m_MaxFIFO_Depth;
  bool m_FIFO_MonitorOn;

// constants
int m_NumSports;
int m_NumMports;

  // states
  bool m_IsInitialized;
  bool m_IC_GetSlaveParam;
  bool m_Reset;
  bool m_HaveSlave;

  // data
  axi_namespace::AXI_SlaveIFimpl<NegEdge>*  m_SlaveIF;
  axi_namespace::AXI_MasterIFimpl<NegEdge>* m_MasterIF;
  axi_namespace::AXI_IC_Request<NegEdge, Verbose>*   m_Step;
  axi_namespace::AXI_ConfigLines*  m_Aline;
  axi_namespace::AXI_ConfigLines*  m_Wline;
  axi_namespace::AXI_ConfigLines*  m_Rline;
  axi_namespace::AXI_ConfigLines*  m_Bline;
  axi_namespace::axi_data_t m_Ap[axi_namespace::AXI_MAX_PORT_SIZE];
  axi_namespace::axi_data_t m_Wp[axi_namespace::AXI_MAX_PORT_SIZE];
  axi_namespace::axi_data_t m_Rp[axi_namespace::AXI_MAX_PORT_SIZE];
  axi_namespace::axi_data_t m_Bp[axi_namespace::AXI_MAX_PORT_SIZE];
  int* m_SPIDtoMID;
  int* m_MPIDtoSID;
  int* m_SIDtoMPID;
  int* m_MIDtoSPID;
  DW_AXI_SimpleSlave<NegEdge, Verbose>* m_Sl;
  axi_namespace::axi_data_t m_Memory;
  int m_IntrlDepth[axi_namespace::AXI_MAX_PORT_SIZE];
  axi_namespace::AXI_AddressMap m_AddressMap;

  // tmp
  int m_Error;
};
#endif //_DW_AXI_IC_H
