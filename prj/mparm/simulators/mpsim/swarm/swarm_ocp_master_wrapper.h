///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         swarm_ocp_master_wrapper.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Wraps a C++ ARM ISS in a SystemC OCP envelope
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SWARM_OCP_MASTER_WRAPPER_H__
#define __SWARM_OCP_MASTER_WRAPPER_H__
 
#include <systemc.h>  //FIXME less #includes here in the .h
#include "globals.h"
#include "armproc.h"
#include "address.h"
#include <fcntl.h>
#include <string.h>
  #ifdef WIN32
  #include <IO.h>
  #else
  #include <unistd.h>
  #endif
#include <iostream>
#include <sys/stat.h>
#include "syscopro.h"
#include "user_swi.h"
#include "swi_calls.h"
#include <stdlib.h>
#include "dmacontrol.h"
#include "dmatransfer.h"
//FIXME
#include "core_signal.h"

//FIXME wrapper.cpp away from Makefile when building xpipes

SC_MODULE(ocpswarm)
{ 
  CArmProc* pArm;
  uint16_t ID;

  sc_in_clk clock;
  // OCP interface
  sc_out<sc_uint<MCMDWD> >          *MCmd;
  sc_out<sc_uint<MATOMICLENGTHWD> > *MAtomicLength;
  sc_out<sc_uint<MBURSTLENGTHWD> >  *MBurstLength;
  sc_out<bool>                      *MBurstPrecise;
  sc_out<sc_uint<MBURSTSEQWD> >     *MBurstSeq;
  sc_out<bool>                      *MBurstSingleReq;
  sc_out<bool>                      *MDataLast;
  sc_out<bool>                      *MReqLast;
  sc_out<sc_uint<MADDRWD> >         *MAddr;
  sc_out<sc_uint<MADDRSPACEWD> >    *MAddrSpace;
  sc_out<sc_uint<MDATAWD> >         *MData;
  sc_out<sc_uint<MBYTEENWD> >       *MByteEn;
  sc_out<bool>                      *MDataValid;
  sc_out<bool>                      *MRespAccept;
  sc_in<bool>                       *SCmdAccept;
  sc_in<bool>                       *SDataAccept;
  sc_in<sc_uint<MDATAWD> >          *SData;
  sc_in<sc_uint<SRESPWD> >          *SResp;
  sc_in<bool>                       *SRespLast;
  sc_out<sc_uint<MFLAGWD> >         *MFlag;
  sc_in<bool>                       *SInterrupt;
  sc_in<sc_uint<SFLAGWD> >          *SFlag;
  
  //FIXME OCP sideband interrupts
  sc_inout<bool> *extint;

  void request();
  void receive();
  void DebugMemoryDump(unsigned int address, const char *target, uint16_t TargetID);
  
  int *intforw; // intforw: external interrupts not yet forwarded to armproc
  uint8_t burst;
  PINOUT pinout;
  sc_signal<bool> go;
  uint8_t cmd_type;
  // #ifndef OLD_INT_STYLE
  sc_uint<SFLAGWD> int_mask;
  bool int_caught;
  // #endif
  
  SC_HAS_PROCESS(ocpswarm);

  ocpswarm(sc_module_name nm, unsigned int ID, Mem_class *builderscratch, Mem_class *builderqueue) :
    sc_module(nm), ID(ID)
  {
    intforw = (int*) calloc(NUMBER_OF_EXT, sizeof(int));
    
    if (DMA)
    {
    /* FIXME. Currently unsupported feature: DMA
      pinout_ft_master = new sc_inout<PINOUT> [2];
      ready_from_master = new sc_in<bool> [2];
      request_to_master = new sc_out<bool> [2];
      */
    }
    else
    {
      MCmd = new sc_out<sc_uint<MCMDWD> > [1];
      MAtomicLength = new sc_out<sc_uint<MATOMICLENGTHWD> > [1];
      MBurstLength = new sc_out<sc_uint<MBURSTLENGTHWD> > [1];
      MBurstPrecise = new sc_out<bool> [1];
      MBurstSeq = new sc_out<sc_uint<MBURSTSEQWD> > [1];
      MBurstSingleReq = new sc_out<bool> [1];
      MDataLast = new sc_out<bool> [1];
      MReqLast = new sc_out<bool> [1];
      MAddr = new sc_out<sc_uint<MADDRWD> > [1];
      MAddrSpace = new sc_out<sc_uint<MADDRSPACEWD> > [1];
      MData = new sc_out<sc_uint<MDATAWD> > [1];
      MByteEn = new sc_out<sc_uint<MBYTEENWD> > [1];
      MDataValid = new sc_out<bool> [1];
      MRespAccept = new sc_out<bool> [1];
      SCmdAccept = new sc_in<bool> [1];
      SDataAccept = new sc_in<bool> [1];
      SData = new sc_in<sc_uint<MDATAWD> > [1];
      SResp = new sc_in<sc_uint<SRESPWD> > [1];
      SRespLast = new sc_in<bool> [1];
      MFlag = new sc_out<sc_uint<MFLAGWD> > [1];
      SInterrupt = new sc_in<bool> [1];
      SFlag = new sc_in<sc_uint<SFLAGWD> > [1];
    }
      
    extint = new sc_inout<bool> [NUMBER_OF_EXT];
      
    pArm = new CArmProc(name(), builderscratch, builderqueue, ID);

    SC_CTHREAD(request, clock.pos());
    SC_CTHREAD(receive, clock.pos());
  }
};

#endif // __SWARM_OCP_MASTER_WRAPPER_H__
