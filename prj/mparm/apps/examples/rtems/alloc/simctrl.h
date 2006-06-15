#ifndef SIMCTRL_H
#define SIMCTRL_H

#define _USr(x) ((volatile unsigned*)(MPSIM_SIMCTRL_BASE + x))
#define _PSr(x) ((volatile void* volatile *)(MPSIM_SIMCTRL_BASE + x))

#define MPSIM_SIMCTRL_BASE     0x7f000000
#define MPSIM_START_METRIC     _USr(0x00000000)
#define MPSIM_STOP_METRIC      _USr(0x00000004)
#define MPSIM_ENDBOOT          _USr(0x00000008)
#define MPSIM_SHUTDOWN         _USr(0x0000000c)
#define MPSIM_DUMP             _USr(0x00000010)
#define MPSIM_DUMP_LIGHT       _USr(0x00000014)
#define MPSIM_CLEAR            _USr(0x00000018)
#define MPSIM_GET_CPU_ID       _USr(0x00000020)
#define MPSIM_GET_CPU_CNT      _USr(0x00000024)
#define MPSIM_SET_REQ_IO       _USr(0x00000030)
#define MPSIM_GET_TIME_LO      _USr(0x00000040)
#define MPSIM_GET_TIME_HI      _USr(0x00000044)
#define MPSIM_GET_CYCLE_LO     _USr(0x00000048)
#define MPSIM_GET_CYCLE_HI     _USr(0x0000004c)
#define MPSIM_STOP_TIME        _USr(0x00000050)
#define MPSIM_RELEASE_TIME     _USr(0x00000054)
#define MPSIM_STOP_CYCLE       _USr(0x00000058)
#define MPSIM_RELEASE_CYCLE    _USr(0x0000005c)
#define MPSIM_DEBUG_MSG_STRING _PSr(0x00000060)
#define MPSIM_DEBUG_MSG_VALUE  _USr(0x00000064)
#define MPSIM_DEBUG_MSG_MODE   _USr(0x00000068)
#define MPSIM_DEBUG_MSG_ID     _USr(0x0000006c)
#define MPSIM_DUMP_TIME_START  _USr(0x00000070)
#define MPSIM_DUMP_TIME_STOP   _USr(0x00000074)
#define MPSIM_GET_ARGC         _USr(0x00010000)
#define MPSIM_GET_ARGV         _USr(0x00010004)
#define MPSIM_GET_ENVP         _USr(0x00010008)
#define MPSIM_ARGV             _USr(0x00020000)
#define MPSIM_ENVP             _USr(0x00030000)
#define MPSIM_FILE_NAME        _USr(0x00040000)
#define MPSIM_FILE_MODE        _USr(0x00040200)
#define MPSIM_FILE_SHIFT       _USr(0x00040204)
#define MPSIM_FILE_OPEN        _USr(0x00040208)
#define MPSIM_FILE_CLOSE       _USr(0x0004020c)
#define MPSIM_FILE_WINDOW_DATA _USr(0x00040210)
#define MPSIM_FILE             _USr(0x00050000)

/* Print mode flags
 * ----------------------------------------------
 *    bit     mnemonic         output
 * ----------------------------------------------
 * 0x00000001 PR_CPU_ID   "Processor PROCID - "
 * 0x00000002 PR_STRING   "STRING "
 * 0x00000004 PR_HEX      "0xHEXVAL "
 * 0x00000008 PR_DEC      "DECVAL "
 * 0x00000010 PR_CHAR     "CHARVAL "
 * 0x00000020 PR_TSTAMP   "@ TIMESTAMP "
 * 0x00000040 PR_NEWL     "\n"
 * 
 * e.g.: 0x00000067 PR_CPU_ID | PR_STRING | PR_HEX | PR_TSTAMP | PR_NEWL  "Processor PROCID - STRING 0xHEXVAL @ TIMESTAMP \n"
 * e.g.: 0x00000049 PR_CPU_ID | PR_DEC | PR_NEWL                          "Processor PROCID - DECVAL \n"
 */ 
#define PR_CPU_ID 0x00000001
#define PR_STRING 0x00000002
#define PR_HEX    0x00000004
#define PR_DEC    0x00000008
#define PR_CHAR   0x00000010
#define PR_TSTAMP 0x00000020
#define PR_NEWL   0x00000040

#define FILE_READ    0x00000001
#define FILE_WRITE   0x00000002

#define MPSIM_FILE_WINDOW_SIZE 0x00004000

static inline void start_metric()      { *MPSIM_START_METRIC = 1; }
static inline void stop_metric()       { *MPSIM_STOP_METRIC = 1;  }
static inline void time_start(int r)   { *MPSIM_DUMP_TIME_START = r; }
static inline void time_stop(int r)    { *MPSIM_DUMP_TIME_STOP = r; }
static inline void end_boot()          { *MPSIM_ENDBOOT = 1; }
static inline void stop_simulation()   { *MPSIM_SHUTDOWN = 1; }
static inline void dump_metric()       { *MPSIM_DUMP = 1; }
static inline void dump_light_metric() { *MPSIM_DUMP_LIGHT = 1; }
static inline void clear_metric()      { *MPSIM_CLEAR = 1; }
static inline char** get_argv()        { return (char**)*MPSIM_GET_ARGV; }
static inline char** get_envp()        { return (char**)*MPSIM_GET_ENVP; }
static inline unsigned get_proc_id()   { return *MPSIM_GET_CPU_ID; }
static inline unsigned get_proc_num()  { return *MPSIM_GET_CPU_CNT; }
static inline unsigned get_argc()      { return *MPSIM_GET_ARGC; }

static inline void pr(char *msg, unsigned value, unsigned mode) {
  *MPSIM_DEBUG_MSG_STRING = msg;
  *MPSIM_DEBUG_MSG_VALUE = value;
  *MPSIM_DEBUG_MSG_MODE = mode;
}

static inline unsigned long long get_time() {
  long long ret;
  *MPSIM_STOP_TIME = 1;
  ret = *MPSIM_GET_TIME_HI;
  ret =  ret << 32 | *MPSIM_GET_TIME_LO;
  *MPSIM_RELEASE_TIME = 1;  
  return ret;
}

static inline unsigned long long get_cycle() {
  long long ret;
  *MPSIM_STOP_CYCLE = 1;
  ret = *MPSIM_GET_CYCLE_HI;
  ret = ret << 32 | *MPSIM_GET_CYCLE_LO;
  *MPSIM_RELEASE_CYCLE = 1;  
  return ret;
}

#endif
