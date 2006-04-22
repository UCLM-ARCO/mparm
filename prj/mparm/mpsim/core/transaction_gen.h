///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         transaction_gen.h
// author       DEIS - Universita' di Bologna
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         Transaction generator for a bus traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSACTION_GEN_H__
#define __TRANSACTION_GEN_H__

#include "globals.h"

typedef enum
{
  TGEN_WORD     = 0,
  TGEN_BYTE     = 1,
  TGEN_HALFWORD = 2
} TGEN_BENABLE;

typedef enum
{
  TGEN_RANDOM,
  TGEN_TABLE,
  TGEN_RHOSIGMA,
  TGEN_STATISTICAL
} TRAFFIC_TYPE;

///////////////////////////////////////////////////////////////////////////////
// transaction_gen - This class encapsulates an AMBA AHB traffic generator.
//                        nm: traffic generator name
//                        ID: traffic generator ID
class transaction_gen
{
  public:
    
    void get_trans(uint32_t *data, uint32_t *address, uint *burst, bool *wr, TGEN_BENABLE *benable, int *idlewait);
  
    transaction_gen(uint ID, TRAFFIC_TYPE ttype);
    ~transaction_gen(){};
  
  private:
    uint ID;
    TRAFFIC_TYPE ttype;
    uint trans_counter, loop_counter, max_trans;
    int repeat_loop;
    bool trans_done;
    
    uint32_t *data_array;
    uint32_t *address_array;
    uint *burst_array;
    bool *wr_array;
    TGEN_BENABLE *benable_array;
    int *idlewait_array;
};

#endif // __TRANSACTION_GEN_H__
