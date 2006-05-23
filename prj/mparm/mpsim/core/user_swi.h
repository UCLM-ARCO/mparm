///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         user_swi.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Provides customized SWIs
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USER_SWI_H__
#define __USER_SWI_H__

// Forward declaration
class CArmProc;

uint32_t print_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t exit_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t metric_start_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t metric_stop_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t metric_dump_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t metric_dump_light_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t metric_clear_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t core_go_idle_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);

uint32_t read_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
uint32_t write_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);

#endif // __USER_SWI_H__
