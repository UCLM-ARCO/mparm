// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : Manages a List of Buffers
// ============================================================================

#ifndef _AXI_CVIP_BUFFER_LIST_H
#define _AXI_CVIP_BUFFER_LIST_H

#include "axi_fifo.h"
#include "axi_cvip_buffer.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_CVIP_BufferList
{
public:
  AXI_CVIP_BufferList(const char* Name
                , int DataBusBytes
                , int DataBusWords
                , double ClockFrequency
                , int MaxXactBuffers
                , int MaxIntlBuffers
                , int RreadyDelayP
                , int BreadyDelayP
                , int AvalidWvalidDelayP
                , int NextAvalidDelayP
                , int NextWvalidDelayP
                , int RvalidRreadyDelayP
                , int BvalidBreadyDelayP
                 );
  ~AXI_CVIP_BufferList();

  // VIP API methods
  void new_buffer(int buf_type, int& buf_handle);
  void copy_buffer(int orig_buf_handle, int& new_buf_handle);
  void delete_buffer(int buf_handle);
  void set_buffer_pattern(int buf_handle, axi_attr_t attr_id,
                          axi_pattern_t pattern, int init_val);
  void reset_buffer_pattern(int buf_handle, axi_attr_t attr_id);
  void set_buffer_attr_bitvec(int buf_handle, axi_attr_t attr_id,
                              int position, const sc_bv<1024>& value);
  void get_buffer_attr_bitvec(int buf_handle, axi_attr_t attr_id,
                              int position, sc_bv<1024>& value);
  void set_buffer_attr_int(int buf_handle, axi_attr_t attr_id,
                           int position, int value);
  void get_buffer_attr_int(int buf_handle, axi_attr_t attr_id,
                           int position, int& value);
  void set_buffer_interleave(int buf_handle, int position, int txn_buf_handle);
  void get_buffer_interleave(int buf_handle, int position, int& txn_buf_handle);

  void ResetStart();
  void ResetEnd();

  // constants
  int m_MaxHandles;

  // data
  int* m_BufType;
  AXI_CVIP_XactBuffer** m_XactBufferList;
  AXI_CVIP_IntlBuffer** m_IntlBufferList;

  AXI_FIFO<int>* m_CmdHandleFIFO;
  int*           m_CmdType;

private:
  void Initialize();
  void ErrorMessage(int Id);

  // parameter
  const char* m_Name;
  int m_DataBusBytes;
  int m_DataBusWords;
  double m_ClockFrequency;
  int m_MaxXactBuffers;
  int m_MaxIntlBuffers;
  int m_RreadyDelayP;
  int m_BreadyDelayP;
  int m_AvalidWvalidDelayP;
  int m_NextAvalidDelayP;
  int m_NextWvalidDelayP;
  int m_RvalidRreadyDelayP;
  int m_BvalidBreadyDelayP;

  // states
  int m_XactBuffers;
  int m_IntlBuffers;
  bool m_Reset;

  // data
  AXI_FIFO<int>* m_BufHandleFIFO;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CVIP_BUFFER_LIST_H
