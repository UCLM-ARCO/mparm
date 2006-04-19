// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : DW_VIP_AXI_XACT and DW_VIP_AXI_INTERLEAVE Buffer
//    - Class is used in the verification master
// ============================================================================

#ifndef _AXI_CVIP_BUFFER_H
#define _AXI_CVIP_BUFFER_H

#include "axi_globals.h"
#include "axi_cvip_types.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

//----------------------------
// XACT Buffer
//----------------------------
class AXI_CVIP_XactBuffer
{
public:
  AXI_CVIP_XactBuffer(const char* Name
                , int DataBusBytes
                , int DataBusWords
                , double ClockFrequency
                , int RreadyDelayP
                , int BreadyDelayP
                , int AvalidWvalidDelayP
                , int NextAvalidDelayP
                , int NextWvalidDelayP
                , int RvalidRreadyDelayP
                , int BvalidBreadyDelayP
                 );
  ~AXI_CVIP_XactBuffer();

  // VIP API methods
  void set_buffer_pattern(axi_attr_t attr_id, axi_pattern_t pattern,
                          const sc_bv<1024>& init_val);
  void reset_buffer_pattern(axi_attr_t attr_id);

  void set_buffer_attr_bitvec(axi_attr_t attr_id,
                              int position, const sc_bv<1024>& value);
  void get_buffer_attr_bitvec(axi_attr_t attr_id,
                              int position, sc_bv<1024>& value);
  void set_buffer_attr_int(axi_attr_t attr_id,
                           int position, int value);
  void get_buffer_attr_int(axi_attr_t attr_id,
                           int position, int& value);

  // fast access methods if one does not care for VIP compliance
  void set_buffer_attr(axi_attr_t attr_id, int position,
                       int value){};
  void get_buffer_attr(axi_attr_t attr_id, int position,
                       int& value){};
  void set_buffer_attr(axi_attr_t attr_id, int position,
                       axi_wstrb_t value){};
  void get_buffer_attr(axi_attr_t attr_id, int position,
                       axi_wstrb_t value){};

  void Reset();

  // public non-API methods
  void PutData(const Rchannel& R, int position);
  void PutData(const Bchannel& B);
  void AlignWdata(int position);
  void AlignRdata(int position);

// Overload the copy operator.
AXI_CVIP_XactBuffer& operator = (const AXI_CVIP_XactBuffer& B)
{
  m_Done = B.m_Done;
  *m_A   = *(B.m_A);
  *m_B   = *(B.m_B);
  *m_ADf = *(B.m_ADf);
  *m_BDf = *(B.m_BDf);
  for (int i = 0; i < 16; i++) {
    *m_W[i]   = *(B.m_W[i]);
    *m_R[i]   = *(B.m_R[i]);
    *m_WDf[i] = *(B.m_WDf[i]);
    *m_RDf[i] = *(B.m_RDf[i]);
  }
  m_XactType           = B.m_XactType;
  m_aAvalidWvalidDelay = B.m_aAvalidWvalidDelay;
  m_wAvalidWvalidDelay = B.m_wAvalidWvalidDelay;
  m_NextAvalidDelay   = B.m_NextAvalidDelay;
  m_BvalidBreadyDelay = B.m_BvalidBreadyDelay;
  m_RreadyDelay       = B.m_RreadyDelay;
  m_BreadyDelay       = B.m_BreadyDelay;
  for (int i = 0; i < 15; i++)
    m_NextWvalidDelay[i] = B.m_NextWvalidDelay[i];
  for (int i = 0; i < 16; i++)
    m_RvalidRreadyDelay[i] = B.m_RvalidRreadyDelay[i];

  m_XactTypeDf           = B.m_XactTypeDf;
  m_aAvalidWvalidDelayDf = B.m_aAvalidWvalidDelayDf;
  m_wAvalidWvalidDelayDf = B.m_wAvalidWvalidDelayDf;
  m_NextAvalidDelayDf   = B.m_NextAvalidDelayDf;
  m_BvalidBreadyDelayDf = B.m_BvalidBreadyDelayDf;
  m_RreadyDelayDf       = B.m_RreadyDelayDf;
  m_BreadyDelayDf       = B.m_BreadyDelayDf;
  for (int i = 0; i < 15; i++)
    m_NextWvalidDelayDf[i] = B.m_NextWvalidDelayDf[i];
  for (int i = 0; i < 16; i++)
    m_RvalidRreadyDelayDf[i] = B.m_RvalidRreadyDelayDf[i];
  return(*this);
}
  // states/events for get_result()
  bool m_Done;
  sc_event m_DoneEvent;
  int m_CmdHandle;

  // AXI channel data
  Achannel* m_A;
  Wchannel* m_W[16];
  Rchannel* m_R[16];
  Bchannel* m_B;

  // CVIP buffer parameter (buffer type and timing values)
  int m_XactType;
  sc_time m_aAvalidWvalidDelay;
  sc_time m_wAvalidWvalidDelay;
  sc_time m_NextAvalidDelay;
  sc_time m_NextWvalidDelay[15];
  sc_time m_BvalidBreadyDelay;
  sc_time m_RvalidRreadyDelay[16];
  sc_time m_RreadyDelay;
  sc_time m_BreadyDelay;

private:
  void Initialize();
  unsigned int GetPattern(int beat, int word,
                          axi_pattern_t pattern, const sc_bv<1024>& init_val);
  void ErrorMessage(int Id);

  // parameter
  const char* m_Name;
  int m_DataBusBytes;
  int m_DataBusWords;
  double m_ClockFrequency;
  int m_RreadyDelayP;
  int m_BreadyDelayP;
  int m_AvalidWvalidDelayP;
  int m_NextAvalidDelayP;
  int m_NextWvalidDelayP;
  int m_RvalidRreadyDelayP;
  int m_BvalidBreadyDelayP;

  // constants
  int m_WstrbBytes;
  int m_WstrbWords;
  int m_Amask;
  double m_ClockPeriod;

  // CVIF buffer default values
  Achannel* m_ADf;
  Wchannel* m_WDf[16];
  Rchannel* m_RDf[16];
  Bchannel* m_BDf;
  int m_XactTypeDf;
  sc_time m_aAvalidWvalidDelayDf;
  sc_time m_wAvalidWvalidDelayDf;
  sc_time m_NextAvalidDelayDf;
  sc_time m_NextWvalidDelayDf[15];
  sc_time m_BvalidBreadyDelayDf;
  sc_time m_RvalidRreadyDelayDf[16];
  sc_time m_RreadyDelayDf;
  sc_time m_BreadyDelayDf;

  // temp
  sc_bigint<1024> m_t0Signed;
  sc_bigint<1024> m_t1Signed;
  sc_bv<1024> m_tValue;
};

//----------------------------
// Interleave Buffer
//----------------------------
class AXI_CVIP_IntlBuffer
{
public:
  AXI_CVIP_IntlBuffer(const char* Name);
  ~AXI_CVIP_IntlBuffer();

  // handle array access commands
  void PutValueH(int position, int buf_handle);
  int GetValueH(int Index);
  int GetNumXactBufferHandles();

  // list access commands
  void PutValue(int buf_handle);
  int GetValue(int Index);
  int GetNumXactBuffers();

  void Reset();

// Overload the copy operator.
AXI_CVIP_IntlBuffer& operator = (const AXI_CVIP_IntlBuffer& B)
{
  for (int i = 0; i < m_NumXactBufferHandles; i++)
    m_XactBufferHandles[i] = B.m_XactBufferHandles[i];
  for (int i = 0; i < m_NumXactBuffers; i++)
    m_XactBufferList[i] = B.m_XactBufferList[i];
  return(*this);
}
  // public data
  int m_CmdHandle;

private:
  void ErrorMessage(int Id);
  // parameter
  const char *m_Name;

  // data
  int m_NumXactBufferHandles;
  int m_XactBufferHandles[AXI_MAX_INTL_BUFFER_SIZE];
  int m_NumXactBuffers;
  int m_XactBufferList[AXI_MAX_INTL_BUFFER_SIZE];
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CVIP_BUFFER_H
