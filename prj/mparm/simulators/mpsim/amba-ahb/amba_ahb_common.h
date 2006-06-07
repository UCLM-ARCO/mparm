///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_common.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Encapsulates common AMBA AHB (signal) definitions and data types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_COMMON_H__
#define __AMBA_AHB_COMMON_H__

#include <systemc.h>

///////////////////////////////////////////////////////////////////////////////
// Maximum number of devices in an AMBA AHB setup (actually, one master slot
// and one slave slot are taken by default master and slave respectively, so
// the actual amount is one less).
#define AHB_MAX_MASTERS 16
#define AHB_MAX_SLAVES  16
///////////////////////////////////////////////////////////////////////////////
// Maximum number of layers that can be attached to an AMBA ICM crossbar.
#define AHB_MAX_ICM_LAYERS 16
///////////////////////////////////////////////////////////////////////////////
// Number of bits of the HMASTER wires. Must be == log2(AHB_MAX_MASTERS).
#define AHB_HMASTER_BITS 4
///////////////////////////////////////////////////////////////////////////////
// ID of the default master on an AHB layer.
#define AHB_DEFAULT_MASTER 0
///////////////////////////////////////////////////////////////////////////////
// ID of the master with top priority at boot on an AHB layer. NOTICE:
// for proper operation, this ID will be the LOWEST among all instantiated
// masters, and master IDs will be assigned sequentially starting from this one.
// The master ID sequence MUST NOT trespass the AHB_MAX_MASTERS boundary, so if
// e.g. you have 8 masters (plus the default one, ID = 0) on a 16-master layer,
// this ID CAN NOT be set to anything above 8.
// The default master ID can be in the middle of the master ID sequence.
#define AHB_BOOT_MASTER 1

///////////////////////////////////////////////////////////////////////////////
// AMBA AHB HTRANS values.
typedef enum
{
  AHB_IDLE   = 0,
  AHB_BUSY   = 1,
  AHB_NONSEQ = 2,
  AHB_SEQ    = 3
} AHB_HTRANS;
              
inline void sc_trace(sc_trace_file *tf, const AHB_HTRANS &v, const sc_string &NAME)
{
  sc_trace(tf, (unsigned char)v, NAME);
};

///////////////////////////////////////////////////////////////////////////////
// AMBA AHB HBURST values.
typedef enum
{
  AHB_SINGLE = 0,
  AHB_INCR   = 1,
  AHB_WRAP4  = 2,
  AHB_INCR4  = 3,
  AHB_WRAP8  = 4,
  AHB_INCR8  = 5,
  AHB_WRAP16 = 6,
  AHB_INCR16 = 7
} AHB_HBURST;
              
inline void sc_trace(sc_trace_file *tf, const AHB_HBURST &v, const sc_string &NAME)
{
  sc_trace(tf, (unsigned char)v, NAME);
};


///////////////////////////////////////////////////////////////////////////////
// AMBA AHB HSIZE values.
typedef enum
{
  AHB_BYTE     = 0,
  AHB_HALFWORD = 1,
  AHB_WORD     = 2,
  AHB_SQ       = 3,       // This one and below not actually used
  AHB_WORD4    = 4,
  AHB_WORD8    = 5,
  AHB_WORD16   = 6,
  AHB_WORD32   = 7
} AHB_HSIZE;
              
inline void sc_trace(sc_trace_file *tf, const AHB_HSIZE &v, const sc_string &NAME)
{
  sc_trace(tf, (unsigned char)v, NAME);
};

///////////////////////////////////////////////////////////////////////////////
// AMBA AHB HRESP values.
typedef enum
{
  AHB_OKAY  = 0,
  AHB_ERROR = 1,
  AHB_RETRY = 2,
  AHB_SPLIT = 3
} AHB_HRESP;
              
inline void sc_trace(sc_trace_file *tf, const AHB_HRESP &v, const sc_string &NAME)
{
  sc_trace(tf, (unsigned char)v, NAME);
};
              
///////////////////////////////////////////////////////////////////////////////
// AMBA AHB HWRITE values.
typedef enum {AHB_READ  = 0,
              AHB_WRITE = 1} AHB_HWRITE;
              
inline void sc_trace(sc_trace_file *tf, const AHB_HWRITE &v, const sc_string &NAME)
{
  sc_trace(tf, (unsigned char)v, NAME);
};

///////////////////////////////////////////////////////////////////////////////
// AHB_master_port - Wires composing an AMBA AHB master pinout.
struct AHB_master_port
{
  bool          hgrant;
  bool          hready;
  sc_uint<32>   hrdata;
  AHB_HRESP     hresp;
  AHB_HBURST    hburst;
  AHB_HTRANS    htrans;
  AHB_HSIZE     hsize;
  AHB_HWRITE    hwrite;
  sc_uint<32>   haddr;
  sc_uint<32>   hwdata;
  bool          hbusreq;
  bool          hlock;

  inline bool operator == (const AHB_master_port& rhs) const
  {
    return (rhs.hgrant  == hgrant  &&
            rhs.hready  == hready  &&
            rhs.hrdata  == hrdata  &&
            rhs.hresp   == hresp   &&
            rhs.hburst  == hburst  &&
            rhs.htrans  == htrans  &&
            rhs.hsize   == hsize   &&
            rhs.hwrite  == hwrite  &&
            rhs.haddr   == haddr   &&
            rhs.hwdata  == hwdata  &&
            rhs.hbusreq == hbusreq &&
            rhs.hlock   == hlock);
  }
};

///////////////////////////////////////////////////////////////////////////////
// AHB_slave_port - Wires composing an AMBA AHB slave pinout.
struct AHB_slave_port
{
  bool                       hsel;
  sc_uint<32>                haddr;
  AHB_HWRITE                 hwrite;
  AHB_HTRANS                 htrans;
  AHB_HSIZE                  hsize;
  AHB_HBURST                 hburst;
  sc_uint<32>                hwdata;
  sc_uint<AHB_HMASTER_BITS>  hmaster;
  AHB_HRESP                  hresp;
  bool                       hready;
  bool                       hreadyout;
  sc_uint<32>                hrdata;
  bool                       hsplit[AHB_MAX_MASTERS];
  bool                       hmastlock;

  inline bool operator == (const AHB_slave_port& rhs) const
  {
    unsigned int i;
    bool hspliteq = true;
    
    for (i = 0; i < AHB_MAX_MASTERS; i ++)
      if (rhs.hsplit[i] != hsplit[i])
        hspliteq = false;
        
    return (rhs.hsel       == hsel       &&
            rhs.haddr      == haddr      &&
            rhs.hwrite     == hwrite     &&
            rhs.htrans     == htrans     &&
            rhs.hsize      == hsize      &&
            rhs.hburst     == hburst     &&
            rhs.hwdata     == hwdata     &&
            rhs.hmaster    == hmaster    &&
            rhs.hresp      == hresp      &&
            rhs.hready     == hready     &&
            rhs.hreadyout  == hreadyout  &&
            rhs.hrdata     == hrdata     &&
            hspliteq                     &&
            rhs.hmastlock  == hmastlock);
  }
};

///////////////////////////////////////////////////////////////////////////////
// AHB_bus_activity - Information needed to compute power usage of AMBA AHB.
struct AHB_bus_activity
{
  bool htrans;
  bool hmaster;
  bool hready;
  unsigned short int n_masters;
  unsigned short int n_slaves;
};

#endif // __AMBA_AHB_COMMON_H__
