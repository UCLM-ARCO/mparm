// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Verification Level Master Class
//    - Class handles sending and receiving requests / responses
//    - AXI masters should derive from this class
//    - Implements the VIP AXI master command IF 
// ============================================================================

#ifndef _AXI_CVIP_Master_H
#define _AXI_CVIP_Master_H

#include "axi_slave_if_impl.h"
#include "axi_master_port.h"
#include "axi_cvip_buffer_list.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

// This module should be used as base class for AXI master traffic
// generators. AXI masters should derive from this class. The
// class implements a command interface on top of the master interface
// to generate AXI master traffic. See the model
// dw_amba_axi_aux/DW_AXI_CVIP_Master for an example that uses this class.
// The usage model for the command interface is as follows:
// - create a buffer
// - fill the buffer with control information, address, and data
// - send the buffer (non blocking)
// - fetch the result data (blocking)
// - delete the buffer
//
// Command interface
// -----------------
// 
// void send_xact(int streamID, int buf_handle, int& cmd_handle)
//   Submits a read or write transaction that is sent to the attached slave/IC
//   This command is non blocking. The transaction is placed into a queue and
//   sent to the attached slave as soon as possible.
//   int streamID: Command stream ID, this value is currently not used.
//   int buf_handle: Handle to the transaction buffer. The transaction buffer
//     can be either an XACT buffer or an interleave buffer.
//   int& cmd_handle: Returns the command handle. This handle must be used
//     in the get_result() or get_interleave_result() methods, depending
//     on the buffer type.
//
// void get_result(int streamID, int cmd_handle, int& buf_handle)
//   Gets the result for a specified transaction. This command is blocking
//   until the results are available. Results can be accessed via the
//   get_buffer_attr_int(), get_buffer_attr_bitvec() methods using the returned
//   buf_handle.
//   int streamID: Command stream ID, this value is currently not used.
//   int cmd_handle: Command handle. This handle is obtained from the
//     send_xact() command.
//   int& buf_handle: Returns, the handle to the transaction buffer.
//     This handle must be used in the get_buffer_attr_int(),
//     get_buffer_attr_bitvec() methods to access the result data.
//
// void get_interleave_result(int streamID, int cmd_handle, int index,
//                            int& buf_handle)
//   Gets the result for a specific transaction from a interleave buffer.
//   This command is blocking until the results are available.
//   Results can be accessed via the get_buffer_attr_int(),
//   get_buffer_attr_bitvec() methods using the returned buf_handle.
//   int streamID: Command stream ID, this value is currently not used.
//   int cmd_handle: Command handle. This handle is obtained from the
//     send_xact() command.
//   int index: Specified for which XACT buffer to get the result.
//     Index refers to the order of the first occurrence of each XACT buffer
//     in the interleave buffer.
//     index = 0 returns the buffer handle for the first XACT buffer.
//   int& buf_handle: Returns, the handle to the transaction buffer.
//     This handle must be used in the get_buffer_attr_int(),
//     get_buffer_attr_bitvec() methods to access the result data.
//
// void new_buffer(int buf_type, int& buf_handle)
//   Creates a new buffer, that can be an XACT buffer or an interleave buffer.
//   int buf_type: buf_type can have the following values:
//     DW_AXI_VIP_XACT : XACT buffer for single burst requests
//     DW_VIP_AXI_INTERLEAVE: Interleave buffer for a sequence of possibly
//       interleaved bursts
//   int& buf_handle: Returns, the handle to the transaction buffer.
//     This handle must be used in the get_result(),
//     get_interleave_result() methods to wait for the result data.
//
// void copy_buffer(int orig_buf_handle, int& new_buf_handle)
//   Copies an existing buffer. Can be used to copy any buffer type.
//   int orig_buf_handle: Handle to the buffer to be copied.
//   int& new_buf_handle: Returns the handle to the new buffer that has
//     the same data and control information as the original buffer.
//
// void delete_buffer(int buf_handle)
//   Frees the memory associated with a buffer. Buffers created via the
//   new_buffer() and copy_buffer() commands as well as buffers returned
//   via the get_result() command must be deleted to prevent memory leaks
//   or an overflow on the maximum number of outstanding buffers. These
//   numbers are specified via the constructor parameters MaxXactBuffers
//   and MaxIntlBuffers.
//   int buf_handle: Handle to the buffer to be deleted.
//     This can be a handle to an XACT or interleave buffer.
//
// void set_buffer_pattern(int buf_handle, axi_attr_t attr_id,
//                         axi_pattern_t pattern, int init_val)
//   Fills the specified attribute (data or control information) with
//   a pattern. As the difference to the set_buffer_attr_bitvec(),
//   and set_buffer_attr_int() commands, this method changes also the default
//   values. This has an effect to the reset_buffer_pattern() command.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//   axi_pattern_t pattern: Pattern to apply to the buffer data.
//     See Table 2 for the values of the enum type axi_pattern_t.
//   int init_val: Initial value for the pattern, if needed. Otherwise this
//     value is ignored.
//
// void reset_buffer_pattern(int buf_handle, axi_attr_t attr_id)
//   Fills the specified attribute (data or control information) with
//   the default values. The default value is either the value as specified
//   in Table 1, or the value set by the set_buffer_pattern() command.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//
// void set_buffer_attr_bitvec(int buf_handle, axi_attr_t attr_id,
//                            int position, const sc_bv<1024>& value)
//   Sets an attribute (data or control information) in the buffer as an
//   bit vector. Attributes that support a different value for each beat
//   require the position argument to indicate which beat is being accessed.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//   int position: Specifies which beat in a burst is being accessed.
//     The first beat in a burst has position 0. This value is ignored for
//     attributes that do not have different values for each beat in a burst.
//   const sc_bv<1024>& value: Value to set for the specified attribute
//     position. Attributes with less than 1024 bits are right justified.
//
// void get_buffer_attr_bitvec(int buf_handle, axi_attr_t attr_id,
//                            int position, sc_bv<1024>& value)
//   Gets the value of a buffer attribute (data or control information)
//   as a bit vector.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//   int position: Specifies which beat in a burst is being accessed.
//   sc_bv<1024>& value: Return value of the specified attribute.
//     Attributes with less than 1024 bits are right justified.
//
// void set_buffer_attr_int(int buf_handle, axi_attr_t attr_id,
//                         int position, int value)
//   Sets an attribute (data or control information) in the buffer as an
//   integer. Attributes that support a different value for each beat
//   require the position argument to indicate which beat is being accessed.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//   int position: Specifies which beat in a burst is being accessed.
//   int value: Value to set for the specified attribute position.
//
// void get_buffer_attr_int(int buf_handle, axi_attr_t attr_id,
//                         int position, int& value)
//   Gets the value of a buffer attribute (data or control information)
//   as an integer.
//   int buf_handle: Handle to the affected XACT buffer.
//   axi_attr_t attr_id: Specifies which control information or data is set.
//     See Table 1 for the values of the enum type axi_attr_t.
//   int position: Specifies which beat in a burst is being accessed.
//   int& value: Return value of the specified attribute.
//
// void set_buffer_interleave(int buf_handle, int position, int txn_buf_handle)
//   Sets a specific position in an interleaved transaction sequence to use
//   data from the specified XACT buffer. The 0 position is the first
//   transaction in the sequence. Each time a write XACT buffer is specified
//   the next beat data from that XACT buffer is used. If a read XACT buffer
//   is specified, only the address phase is interleaved with the write address
//   phases. The timing of the data phase of a read is determined by hte slave
//   response.
//   int buf_handle: Handle to the affected interleave buffer.
//   int position: Specifies for which position in the transfer sequence
//     to set the txn_buf_handle.
//     The maximum interleave buffer size is 128. Therefore position must be
//     in the range 0 <= position <= 127.
//   int txn_buf_handle: Handle to the XACT buffer, containing the
//     transfer data.
//
// void get_buffer_interleave(int buf_handle, int position, int& txn_buf_handle)
//   Gets the XACT buffer handle from a specific position in an interleaved
//   transaction sequence. 
//   int buf_handle: Handle to the affected interleave buffer.
//   int position: Specifies for which position in the transfer sequence
//     to get the txn_buf_handle.
//   int& txn_buf_handle: Returns the handle to the XACT buffer, containing the
//     transfer data.
//
// Table 1: axi_attr_t values.
// ---------------------------------------------------------------------
// axi_attr_t value: DW_VIP_AXI_AWRITE
// Data type       : bit [0]
// Default value   : 0
// Description     : AWRITE signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ALEN
// Data type       : bit [3:0]
// Default value   : 0
// Description     : ALEN signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ASIZE
// Data type       : bit [2:0]
// Default value   : 0
// Description     : ASIZE signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ABURST
// Data type       : bit [1:0]
// Default value   : 0
// Description     : ABURST signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ALOCK
// Data type       : bit [1:0]
// Default value   : 0
// Description     : ALOCK signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ACACHE
// Data type       : bit [3:0]
// Default value   : 0
// Description     : ACACHE signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_APROT
// Data type       : bit [2:0]
// Default value   : 0
// Description     : APROT signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_AID
// Data type       : bit [7:0]
// Default value   : 0
// Description     : AID signal
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_ADDR
// Data type       : bit [31:0]
// Default value   : 0
// Description     : ADDR signal. Currently only 32-bit addresses are supported.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_BUFFER_DATA
// Data type       : bit [1023:0]
// Default value   : 0
// Description     : WDATA/RDATA signal depending on the transaction type.
//                   Regardless of the width and position of the data on the
//                   WDATA/RDATA byte lanes, the data for this command are
//                   always right justified to the LSB.
//                   That means the bit vector holds 2**ASIZE bytes of data
//                   in its LSB's. This module aligns the data to the
//                   appropriate byte lanes.
//                   This attributes supports up to 16 values.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_WSTRB_BEAT
// Data type       : bit [127:0]
// Default value   : 128'hFFF...FFF
// Description     : WSTRB signal.
//                   Regardless of the width and position of the data on the
//                   WDATA/RDATA byte lanes, the data for this command must be
//                   always right justified to the LSB.
//                   That means the bit vector holds 2**ASIZE bits of data
//                   in its LSB's. This module aligns the input WSTRB signal
//                   to the appropriate byte lanes, and inserts zeros for the
//                   unused byte lanes.
//                   This attributes supports up to 16 values.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_RESP
// Data type       : bit [1:0]
// Default value   : 0
// Description     : RRESP/BRESP signal, depending on the transaction type.
//                   For a read or read expect buffer the attribute supports up
//                   to 16 values. For a write buffer the position argument is
//                   ignored.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_XACT_TYPE
// Data type       : int
// Default value   : DW_VIP_AXI_XACT_READ
// Description     : Type of a transaction represented by this buffer.
//                   Valid types are:
//                   DW_VIP_AXI_XACT_WRITE
//                   DW_VIP_AXI_XACT_READ
//                   DW_VIP_AXI_XACT_READ_EXPECT
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_AVALID_WVALID_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Timing between the transfer of the first beat of
//                   write data and its address phase on the AXI IC.
//                   If VALUE < 0 the first beat of write data starts
//                   max(-VALUE, DW_VIP_AXI_NEXT_AVALID_DELAY) clock cycles
//                   before it's address phase.
//                   If VALUE == 0 he first beat of write data and it's
//                   address phase start at the same time, after
//                   DW_VIP_AXI_NEXT_AVALID_DELAY has elapsed.
//                   If VALUE > 1 the first beat of write data starts
//                   VALUE clock cycles after it's address phase.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_NEXT_AVALID_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Number of clock cycles delay between the completion
//                   of the current address phase and the start of the next
//                   address phase.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_NEXT_WVALID_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Inter-beat delay in a write burst transaction. Number
//                   of clock cycles the next write beat starts after the current
//                   write beat has been accepted.
//                   This attribute supports up to 15 values.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_BVALID_BREADY_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Number of clock cycles delay between sampling BVALID true
//                   and setting BREADY true. VALUE=0 means BREADY goes true
//                   in the cycle following the BVALID = true. This delay has
//                   no effect if BREADY is driven true by the constructor 
//                   parameter BreadyDefault.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_RVALID_RREADY_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Number of clock cycles delay between sampling RVALID true
//                   and setting RREADY true. VALUE=0 means RREADY goes true
//                   in the cycle following the RVALID = true. This delay has
//                   no effect if RREADY is driven true by the constructor 
//                   parameter RreadyDefault.
//                   This attribute supports up to 16 values, one for each
//                   beat in a read transaction.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_RREADY_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Number of clock cycles delay between RVALID and RREADY
//                   were both true and setting RREADY to the default value.
//                   During this delay RREADY is driven false, unless RREADY
//                   is driven due to an active RDATA transfer.
//                   The default value that is applied to RREADY after the delay
//                   elapses is specified by the constructor parameter
//                   RreadyDefault. 
//                   VALUE=0 means RREADY is set to the default value
//                   in the cycle following the cycle were RVALID and
//                   RREADY = true.
// -----------------------------------
// axi_attr_t value: DW_VIP_AXI_BREADY_DELAY
// Data type       : int
// Default value   : Constructor parameter, with default = 0
// Description     : Number of clock cycles delay between BVALID and BREADY
//                   were both true and setting BREADY to the default value.
//                   During this delay BREADY is driven false, unless BREADY
//                   is driven due to an active BRESP transfer.
//                   The default value that is applied to BREADY after the delay
//                   elapses is specified by the constructor parameter
//                   BreadyDefault. 
//                   VALUE=0 means BREADY is set to the default value
//                   in the cycle following the cycle were BVALID and
//                   BREADY = true.
// -----------------------------------
//
//
// Table 2: axi_pattern_t values.
// ---------------------------------------------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_X
//                      DW_VIP_AXI_PATTERN_ZERO
// Description        : Sets all bits in the region to zero.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_ONE
// Description        : Sets all bits in the region to one.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_A5
// Description        : Sets all bits in the region to 0xA5.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_5A
// Description        : Sets all bits in the region to 0x5A.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_WALK0
// Description        : Sets all words in the region to a walking 0 pattern.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_WALK1
// Description        : Sets all words in the region to a walking 1 pattern.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_INCR
// Description        : Sets all words in the region to an incrementing pattern.
//                      An initial value must be set.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_DECR
// Description        : Sets all words in the region to an decrementing pattern.
//                      An initial value must be set.
// -----------------------------------
// axi_pattern_t value: DW_VIP_AXI_PATTERN_CONST
// Description        : Sets all words in the region to the specified
//                      initial value.
// -----------------------------------
//

template <bool NegEdge, bool Verbose>

class AXI_CVIP_Master // Base Class for AXI Verification Master
  : public AXI_SlaveIFimpl<NegEdge>
  , public AXI_CVIP_BufferList
{
public:
  // Master port that must be connected to a master interface.
  AXI_MasterPort MasterP;

  SC_HAS_PROCESS(AXI_CVIP_Master);

  AXI_CVIP_Master(sc_module_name Name
  , int ID // Master ID. Unique number among all IC's used in the
    // AXI system. Must satisfy: 0 <= ID <= 31
  , int DataBusBytes // Number of byte lanes of the AXI IC.
    // The same number must be used in the entire AXI system.
  , int DataBusWords // DataBusBytes in units of words
    // (1 word = 4 bytes).
    // DataBusWords = (DataBusBytes > 4) ? DataBusBytes/4 : 1
  , double ClockFrequency // Frequency of the AXI system clock (Aclk)
    // in units of cycles per second.
  , int MaxXactBuffers // Maximum number of Xact buffers. A master using
    // this class must not create more Xact buffers than here specified.
  , int MaxIntlBuffers // Maximum number of interleave buffers. A master using
    // this class must not create more interleave buffers than here specified.
  , int AID_Width // Width in units of bits of the AID field.
    // Masters should not generate an AID >= (2**AID_Width) - 1
    // AID_Width must be selected such that the width of the extended
    // AID field at the output of the IC does not exceed 13 bits.
    // Must satisfy: 0 <= AID_Width <= 12
    // Recommended values are AID_Width = 4 or 8
  , int MaxRequests // Maximum number of outstanding requests. All requests
    // coming from a master are queued. The queue size is MaxRequests.
    // MaxRequest should be larger than MaxXactBuffers. MaxRequest depends
    // also on the number of transfers coming form the interleave buffer.
  , int WriteIntlDepth // Maximum number of write bursts that can be
    // interleaved. Note that this number must not exceed the interleaving
    // depth of the attached slave (or IC). This value can be set during
    // run time (preferable after the interleaving depth of the attached
    // slave/IC has been fetched) via the public data member m_WriteIntlDepth.
  , bool RreadyDefault     = false // Default value of the Rready signal.
  , bool BreadyDefault     = false // Default value of the Bready signal.
  , int RreadyDelayP       = 0 // Default value of for Rready delay.
  , int BreadyDelayP       = 0 // Default value of for Bready delay.
  , int AvalidWvalidDelayP = 0 // Default value of for AvalidWvalid delay.
  , int NextAvalidDelayP   = 0 // Default value of for NextAvalid delay.
  , int NextWvalidDelayP   = 0 // Default value of for NextWvalid delay.
  , int RvalidRreadyDelayP = 0 // Default value of for RvalidRready delay.
  , int BvalidBreadyDelayP = 0 // Default value of for BvalidBready delay.
                 );
  ~AXI_CVIP_Master();

  // SystemC processes
  void MasterFSM();

  // VIP API methods (buffer manipulation methods are inherited)
  void send_xact(int streamID, int xact_buf_handle, int& cmd_handle);
  void get_result(int streamID, int cmd_handle, int& buf_handle);
  void get_interleave_result(int streamID, int cmd_handle, int index,
                             int& buf_handle);

  void AXI_CVIP_Ms_Reset();


private:
  void AchannelFSM();
  void WchannelFSM();
  void RchannelFSM();
  void BchannelFSM();
  void Initialize();
  void ErrorMessage(int Id);
  void end_of_elaboration();

  // parameter
  const char* m_Name;
  int m_ID;
  int m_DataBusBytes;
  int m_DataBusWords;
  int m_AID_Width;
  int m_MaxRequests;
public:  int m_WriteIntlDepth;
private:
  bool m_RreadyDefault;
  bool m_BreadyDefault;
  int m_RreadyDelayP;
  int m_BreadyDelayP;
  int m_AvalidWvalidDelayP;
  int m_NextAvalidDelayP;
  int m_NextWvalidDelayP;
  int m_RvalidRreadyDelayP;
  int m_BvalidBreadyDelayP;

  // constants
  int m_Idx;
  int m_NumAIDs;
  double m_ClockPeriod;
  int m_WstrbWords;
  int m_WstrbBytes;
  int m_BitMask;

  // FIFO's
  AXI_FIFO<AXI_CVIP_XactBuffer*>* m_AchannelFIFO;
  AXI_FIFO<sc_time>*  m_AsendTimeFIFO;

  AXI_FIFO<AXI_CVIP_XactBuffer*>* m_WchannelFIFO;
  AXI_FIFO<int>* m_WbeatFIFO;
  AXI_FIFO<sc_time>*  m_WsendTimeFIFO;

  AXI_FIFO<AXI_CVIP_XactBuffer*>** m_RchannelFIFO;
  AXI_FIFO<int>** m_RbeatFIFO;
  char** m_Rstr;

  AXI_FIFO<AXI_CVIP_XactBuffer*>** m_BchannelFIFO;
  char** m_Bstr;

  
  // data
  AXI_CVIP_XactBuffer* m_AxactBuffer;
  AXI_CVIP_XactBuffer* m_WxactBuffer;
  AXI_CVIP_XactBuffer* m_RxactBuffer;
  AXI_CVIP_XactBuffer* m_BxactBuffer;

  struct AIDinfo{
    bool Active;
    int Alen;
    int Beat;
  };
  AIDinfo* m_tIDa[AXI_MAX_ID_SIZE];

  AXI_CVIP_XactBuffer** m_Monitor;

  // States
  // global states
  bool m_Reset;
  sc_time m_CurrTime;

  // A-FSM states
  axi_put_state m_Astate;
  bool m_AaddrValid;
  sc_time m_Adelay;
  sc_time m_AWdelay;

  // W-FSM states
  axi_put_state m_Wstate;
  bool m_WaddrValid;
  sc_time m_Wdelay;
  sc_time m_WAdelay;
  int m_Wbeat;

  // R-FSM states
  axi_getr_state m_Rstate;
  bool m_RaidValid;
  sc_time m_Rdelay;
  sc_time m_RvalidRdelay;
  int m_Rbeat;
  bool m_Rready;

  // B-FSM states
  axi_getr_state m_Bstate;
  bool m_BaidValid;
  sc_time m_Bdelay;
  sc_time m_BvalidBdelay;
  bool m_Bready;

  // get_result data
  AXI_CVIP_XactBuffer* m_ResXactBuffer; 
  AXI_CVIP_IntlBuffer* m_IntlBuffer;

  // Error reporting values
  int m_IntValue;

  // temp variables
  AXI_CVIP_XactBuffer* m_XactBuffer;
  AXI_CVIP_IntlBuffer* m_tIntlBuffer;
  AXI_CVIP_IntlBuffer* m_tNewIntlBuffer;
  AIDinfo* m_tID;
  sc_time m_tt;
  Achannel* m_tA;
  Achannel* m_A;
  AreadySig* m_Ar;
  Wchannel* m_W;
  WreadySig* m_Wr;
  Rchannel* m_R;
  RreadySig* m_Rr;
  Bchannel* m_B;
  BreadySig* m_Br;
  int *tIntlBufferList;
  int m_iError;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CVIP_Master_H
