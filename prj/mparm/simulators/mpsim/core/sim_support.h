///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         sim_support.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Simulation support: memory-mapped user-app I/O, stats interaction
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SIM_SUPPORT_H__
#define __SIM_SUPPORT_H__

#include <systemc.h>
#include "globals.h"
#include "stats.h"
#include "debug.h"
#include <stdlib.h>
#include "sim_support_flags.h"

class Sim_Support
{
  public:
    Sim_Support(int argc, char *argv[], char *envp[]);
    ~Sim_Support();
    
    bool catch_sim_message(uint32_t addr, uint32_t *data, bool write, uint8_t ID);
    
  private:
    bool *stopped_time;
    bool *stopped_cycle;
    uint64_t *current_time;
    uint64_t *current_cycle;
    char **debug_msg_string;
    uint32_t *debug_msg_value;
    uint32_t *debug_msg_mode;
    uint32_t *debug_msg_id;
    int argc;
    char **argv, **envp;
    bool *file_read;
    FILE **current_file;
    uint32_t *file_window_valid_data;
    uint32_t **filebuffer;
    char **filename;
};

extern Sim_Support *simsuppobject;

#endif // __SIM_SUPPORT_H__
