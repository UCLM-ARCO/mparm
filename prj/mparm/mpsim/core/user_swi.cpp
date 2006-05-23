///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         user_swi.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Provides customized SWIs
//
///////////////////////////////////////////////////////////////////////////////

#include <systemc.h>
#include "globals.h"
#include "address.h"
#include "stats.h"
#include "armproc.h"

typedef struct {
  char *string;
  bool num_arg_needed;
} MSG_;

// User-defined printable messages
MSG_  messages[]={
  /* 0 */  {"Waiting on semaphore", true},
  /* 1 */  {"Semaphore acquired", true},
  /* 2 */  {"Semaphore released", true},
  /* 3 */  {"Number of times I've been interrupted:", true},
  /* 4 */  {"Begin", false},
  /* 5 */  {"Element:", true},
  /* 6 */  {"Matrix computed!", false},
  /* 7 */  {"", false},
  /* 8 */  {"Inside interrupt routine", false},
  /* 9 */  {"Shared input address:", true},
  /* 10*/  {"Shared output address:", true},
  /* 11*/  {"Op matrix address:", true},
  /* 12*/  {"Input matrix address:", true},
  /* 13*/  {"Output matrix address:", true},
  /* 14*/  {"End", false},
  /* 15 .. 19 */  {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
  /* 20 .. 24 */  {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
  /* 25 */  {"dummy0", true},
  /* 26 */  {"dummy1", true},
  /* 27 */  {"dummy2", true},
  /* 28 */  {"dummy3", true},
  /* 29 */  {"dummytot", true}
};


///////////////////////////////////////////////////////////////////////////////
// print_swi_call - Allows printing debug messages even without OS support.
//                  The simulator can identify the processor which made the SWI call.
//                  r2: selects a string to be printed.
//                  r3: numeric argument to be printed.
//
//    r2 value         behavior
//    --------------------------------------------
//    0x10000     Processor %d  STRING  and \n
//    0x10001     Processor %d  STRING  without \n
//    0x10002     NO proc %d    STRING  and \n
//    0x10003     NO proc %d    STRING  and NO \n
//
//    0x10010     Processor %d  HEXVAL  and \n
//    0x10011     Processor %d  HEXVAL  without \n
//    0x10012     NO proc %d    HEXVAL  and \n
//    0x10013     NO proc %d    HEXVAL  and NO \n
//
//    0x10020     Processor %d  HEXVAL && TIME_start  and \n
//    0x10021     Processor %d  HEXVAL && TIME_start without \n
//    0x10022     NO proc %d    HEXVAL && TIME_start and \n
//    0x10023     NO proc %d    HEXVAL && TIME_start and NO \n
//
//    0x10030     Processor %d  HEXVAL && TIME_stop  and \n
//    0x10031     Processor %d  HEXVAL && TIME_stop without \n
//    0x10032     NO proc %d    HEXVAL && TIME_stop and \n
//    0x10033     NO proc %d    HEXVAL && TIME_stop and NO \n
//
uint32_t print_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  int id;
  uint32_t command;
  uint32_t without_proc_id, without_newline;

  if (r1<N_CORES) {
      /* we can use another cpu's memory */
    id = r1;
  } else
    id = arm->ID;

  command = r2>>4;
  without_proc_id  = r2&2;
  without_newline = r2&1;
  switch (command) {
    case 0x1000:
        /* print a string */
      if (!without_proc_id) printf("Processor %d  -  ", arm->ID);
      printf("%s", addresser->pMemoryDebug[id]+r3);
      if (!without_newline) printf("\n");
      return r0;
      break;

    case 0x1001:
        /* print a hex value */
      if (!without_proc_id) printf("Processor %d  -  ", arm->ID);
      printf("0x%x", r3);
      if (!without_newline) printf("\n");
      return r0;
      break;
    
    case 0x1002:
        /* print an hex value and also the time_start*/
      if (!without_proc_id) printf("Processor %d  -  ", arm->ID);
      printf("0x%x start_time: %10.1f", r3, sc_simulation_time());
      if (!without_newline) printf("\n");
      return r0;    
      
    case 0x1003:
        /* print an hex value and also the time_start*/
      if (!without_proc_id) printf("Processor %d  -  ", arm->ID);
      printf("0x%x stop_time: %10.1f", r3, sc_simulation_time());
      if (!without_newline) printf("\n");
      return r0;
      break;
    
    default:
      break;
  }

  /* Predefined strings */
  if (r2<sizeof(messages)/sizeof(messages[0])) {
    unsigned int k;
    for (k=0 ; k<arm->ID ; k++) printf("\t");
    printf("Processor %d  -  ", arm->ID);
    printf(messages[r2].string);
    if (messages[r2].num_arg_needed) printf(" %d", r3);
    printf("\n");
  }
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// exit_swi_call - Marks the processor as ready to shutdown.
uint32_t exit_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  statobject->quit(arm->ID);
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// metric_start_swi_call - Starts collecting statistics for this processor.
uint32_t metric_start_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  statobject->startMeasuring(arm->ID);
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// metric_stop_swi_call - Stops collecting statistics for this processor.
uint32_t metric_stop_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  statobject->stopMeasuring(arm->ID);
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// metric_dump_swi_call - Requests a dump of the statistics for this processor.
uint32_t metric_dump_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  statobject->dump(arm->ID);
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// metric_dump2_swi_call - Requests a restricted dump of the statistics for
// this processor and for the global objects.
uint32_t metric_dump_light_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t
 r2, uint32_t r3)
{
  statobject->dump_light(arm->ID);
  return r0;
}


///////////////////////////////////////////////////////////////////////////////
// metric_clear_swi_call - Reset collected values.
uint32_t metric_clear_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  statobject->clear();
  return r0;
}

///////////////////////////////////////////////////////////////////////////////
// core_go_idle_swi_call - Turn the core in IDLE (low-power, no-op state).
uint32_t core_go_idle_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  arm->SwitchToIdleState();
  return r0;
}

///////////////////////////////////////////////////////////////////////////////
// read_swi_call - read data from native file descriptor
uint32_t read_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  int fd = (int)r1;
  void* data = (void*)r2;
  size_t len = (size_t)r3;

  return (uint32_t) read(fd, data, len);
}

///////////////////////////////////////////////////////////////////////////////
// write_swi_call - write data to native file descriptor
uint32_t write_swi_call(CArmProc *arm, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
  int fd = (int)r1;
  void* data = (void*)r2;
  size_t len = (size_t)r3;

  return (uint32_t) write(fd, data, len);
}
