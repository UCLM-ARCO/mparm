// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Types used by th CVIP Master
// ============================================================================

#ifndef _AXI_CVIP_TYPES_H
#define _AXI_CVIP_TYPES_H

#ifndef __EDG__
namespace axi_namespace {
#endif

typedef enum {
  DW_VIP_AXI_AWRITE = 0,
  DW_VIP_AXI_ALEN   = 1,
  DW_VIP_AXI_ASIZE  = 2,
  DW_VIP_AXI_ABURST = 3,
  DW_VIP_AXI_ALOCK  = 4,
  DW_VIP_AXI_ACACHE = 5,
  DW_VIP_AXI_APROT  = 6,
  DW_VIP_AXI_AID    = 7,
  DW_VIP_AXI_ADDR   = 8,
  DW_VIP_AXI_BUFFER_DATA = 9,
  DW_VIP_AXI_WSTRB_BEAT  = 10,
  DW_VIP_AXI_RESP        = 11,
  DW_VIP_AXI_XACT_TYPE            = 12,
  DW_VIP_AXI_AVALID_WVALID_DELAY = 13,
  DW_VIP_AXI_NEXT_AVALID_DELAY   = 14,
  DW_VIP_AXI_NEXT_WVALID_DELAY   = 15,
  DW_VIP_AXI_BVALID_BREADY_DELAY = 16,
  DW_VIP_AXI_RVALID_RREADY_DELAY = 17,
  DW_VIP_AXI_RREADY_DELAY        = 18,
  DW_VIP_AXI_BREADY_DELAY        = 19
} axi_attr_t;

typedef enum {
  DW_VIP_AXI_PATTERN_X     = 0,
  DW_VIP_AXI_PATTERN_ZERO  = 1,
  DW_VIP_AXI_PATTERN_ONE   = 2,
  DW_VIP_AXI_PATTERN_A5    = 3,
  DW_VIP_AXI_PATTERN_5A    = 4,
  DW_VIP_AXI_PATTERN_WALK0 = 5,
  DW_VIP_AXI_PATTERN_WALK1 = 6,
  DW_VIP_AXI_PATTERN_INCR  = 7,
  DW_VIP_AXI_PATTERN_DECR  = 8,
  DW_VIP_AXI_PATTERN_CONST = 9
} axi_pattern_t;

const int DW_VIP_AXI_XACT_WRITE = 0;
const int DW_VIP_AXI_XACT_READ  = 1;
const int DW_VIP_AXI_XACT_READ_EXPECT = 2;

const int DW_VIP_AXI_XACT       = 0;
const int DW_VIP_AXI_INTERLEAVE = 1;

const int AXI_MAX_INTL_BUFFER_SIZE = 128;

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CVIP_TYPES_H
