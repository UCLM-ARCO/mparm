///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         swi_calls.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Provides customized SWIs
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SWI_CALLS_H__
#define __SWI_CALLS_H__

#define SWI_EXIT         0x00800000
#define SWI_PRINT        0x00800005
#define SWI_METRIC_START 0x00800007
#define SWI_METRIC_STOP  0x00800008
#define SWI_METRIC_DUMP  0x00800009
#define SWI_METRIC_DUMPL 0x0080000A
#define SWI_METRIC_CLEAR 0x0080000B
#define SWI_CORE_GO_IDLE 0x0080000C

#define SWI_EXITstr         "0x00800000"
#define SWI_PRINTstr        "0x00800005"
#define SWI_METRIC_STARTstr "0x00800007"
#define SWI_METRIC_STOPstr  "0x00800008"
#define SWI_METRIC_DUMPstr  "0x00800009"
#define SWI_METRIC_DUMPLstr "0x0080000A"
#define SWI_METRIC_CLEARstr "0x0080000B"
#define SWI_CORE_GO_IDLEstr "0x0080000C"

#endif // __SWI_CALLS_H__
