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

#define SWI_READ         0x00800100
#define SWI_WRITE        0x00800101

#define __swi_str(x) #x
#define __swi(x) __asm("swi " __swi_str(x))

#endif // __SWI_CALLS_H__
