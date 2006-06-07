///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_builder.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Manages AMBA AHB (OCCN) platform instantiation
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_OCCN_BUILDER_H__
#define __AMBA_OCCN_BUILDER_H__

#include <systemc.h>
#include "globals.h"
#include "amba_occn_signal.h"
#include "core_signal.h"
#include "amba_occn_master.h"
#include "amba_occn_slavemem.h"
#include "amba_occn_slaveint.h"
#include "armproc.h"
#include "wrapper.h"
#include "mem_class.h"
#include "scratch_mem.h"
#include "occn.h"
#include "arm_bus.h"
#include "ahb_default_master.h"

void buildAMBAAHBOCCNPlatform(int new_argc, char *new_argv[], char *new_envp[]);

#endif // __AMBA_OCCN_BUILDER_H__
