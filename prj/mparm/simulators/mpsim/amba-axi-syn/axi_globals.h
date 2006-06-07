// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Global Types
//  - Internal global classes and types
// ============================================================================

#ifndef _AXI_GLOBALS_H
#define _AXI_GLOBALS_H

#include "axi_types.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

// data type dependent constants
const int AXI_WSTRB_BYTES = sizeof(axi_wstrb_bt);
const int AXI_WSTRB_BITS = AXI_WSTRB_BYTES * 8;
const int AXI_WORDSIZE_BYTES = sizeof(axi_data_bt);
const int AXI_WORDSIZE_BITS = AXI_WORDSIZE_BYTES * 8;

const sc_time_unit AXI_TIME_UNIT = SC_SEC;

// Max sizes
const int AXI_MAX_ID_SIZE   = 256;
// Max FIFO length (for all FIFO's that are not controlled by MaxFIFO_Depth)
const int AXI_MAX_FIFO_SIZE = 512;
// derived constant, maximum numbers of extended ID's
const int AXI_MAX_EID_SIZE  = AXI_MAX_ID_SIZE*AXI_MAX_PORT_SIZE;
// maximum number of virtual channels per master
const int AXI_MAX_VC_SIZE   = 16;
// max number of IC lines (sum of A-lines, W-lines, R-lines and B-lines)
const int AXI_MAX_IC_LINE_SIZE = 32;

// ----------------------------------------------------------------------------
//  Type denoting the AXI Channels
// ----------------------------------------------------------------------------
typedef enum {
	AXI_A_CHANNEL = 0,
	AXI_W_CHANNEL = 1,
	AXI_R_CHANNEL = 2,
	AXI_B_CHANNEL = 3
} axi_channel_t;

// -------------------------------------------------------------------------
//  Types for the get FSM's
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
//  Type for the get request state
// -------------------------------------------------------------------------
typedef enum {
	AXI_GET_START = 0,
	AXI_GET_WAIT  = 1
} axi_get_state;
// -------------------------------------------------------------------------
//  Type for the get write request state
// -------------------------------------------------------------------------
typedef enum {
	AXI_GETW_START       = 0,
	AXI_GETW_WAIT_VALID  = 1,
	AXI_GETW_WAIT_ADDR   = 2
} axi_getw_state;
// -------------------------------------------------------------------------
//  Type for the get response FSM's
// -------------------------------------------------------------------------
typedef enum {
	AXI_GETR_START       = 0,
	AXI_GETR_WAIT_VALID  = 1,
	AXI_GETR_WAIT_READY  = 2
} axi_getr_state;
// -------------------------------------------------------------------------
//  Types for the put FSM's
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
//  Type for the put request FSM's
// -------------------------------------------------------------------------
typedef enum {
	AXI_PUT_START        = 0,
	AXI_PUT_WAIT_NEXT    = 1,
	AXI_PUT_WAIT_ADDRESS = 2,
	AXI_PUT_SEND         = 3
} axi_put_state;
typedef enum {
	AXI_PUT_DATA_START0 = 0,
	AXI_PUT_DATA_START1 = 1,
	AXI_PUT_DATA_CONT0  = 2,
	AXI_PUT_DATA_CONT1  = 3
} axi_putd_state;
typedef enum {
	AXI_PUT_CONTROL_START = 0,
	AXI_PUT_CONTROL_SEND  = 1
} axi_putc_state;
// ----------------------------------------------------------------------------
//  Type for the Lock States.
// ----------------------------------------------------------------------------
typedef enum {
	AXI_UNLOCKED     = 0,
	AXI_LOCKED_START = 1,
	AXI_LOCKED       = 2,
	AXI_LOCKED_LAST  = 3,
	AXI_LOCKED_WAIT  = 4
} axi_lockstate_t;
// ----------------------------------------------------------------------------
//  Type for the IF index selection mode
// ----------------------------------------------------------------------------
typedef enum {
	AXI_IM_IC = 0,
	AXI_IM_SP = 1,
	AXI_IM_MP = 2
} axi_idx_mode_t;

// ----------------------------------------------------------------------------
//  Class for storing the config parameter for the IC lines
// ----------------------------------------------------------------------------
class AXI_IC_Line
{
public:
  AXI_IC_Line(){
    IPortID = new int[AXI_MAX_PORT_SIZE];
    TPortID = new bool[AXI_MAX_PORT_SIZE];
    for (int i = 0; i < AXI_MAX_PORT_SIZE; i++)
      TPortID[i] = false;
  };
  ~AXI_IC_Line() {
    delete [] IPortID; delete [] TPortID;
};

int Size;
int* IPortID; // IPortID[0] = first initiator connected to the line
bool* TPortID; // TPortID[i] = true, means target[i] is connected
};

// ----------------------------------------------------------------------------
//  Some useful tables
// ----------------------------------------------------------------------------
const unsigned int
AXI_ByteStrobeMask[16] = {
        0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
        0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF,
        0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
        0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
};

// ----------------------------------------------------------------------------
//  Some useful defines
// ----------------------------------------------------------------------------
// Wait for handshake signal asserted to high
#define AXI_WAIT_HS(x, s) { \
  do { \
    if (m_Reset) s = true; \
    else { \
      wait(); \
      this->GetChannel(0, x); \
    } \
  } while (!s); \
}

#define AXI_EXTEND_ID(ID, PID, ID_Width) { \
  ID |= (PID << ID_Width); \
}
#define AXI_TRUNCATE_ID(ID, PID, ExID, ID_Width, ID_Mask) { \
  ExID = ID; PID = ID >> ID_Width; ID &= ID_Mask; \
}

// global functions
extern sc_string axi_expand(const sc_string &expstr);
extern void axi_bv_lshift(axi_data_t Input, axi_data_t Output, int Shift,
                          int DataBusBytes, int DataBusWords);
extern void axi_bv_rshift(axi_data_t Input, axi_data_t Output, int Shift,
                          int DataBusBytes, int DataBusWords);
extern sc_string axi_hexprint(const unsigned int* data_,
       const unsigned int* strobe_, int num_bytes_);
extern void axi_get_wstrb(axi_wstrb_t Wstrb,
                   int Shift, // address_&(_data_width_bytes-1)
                   int NumBytes,
                   int WstrbBytes, int WstrbWords);

// ----------------------------------------------------------------------------
//  Debug defines
// ----------------------------------------------------------------------------
//#define AXI_MICRO_DEBUG
//#define AXI_VERBOSE_2
//#define AXI_VERBOSE_1

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_GLOBALS_H
