///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         core_signal.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of the platform
//
///////////////////////////////////////////////////////////////////////////////

#include "core_signal.h"

// System clock
sc_clock ClockGen_1("ClockGen_1", CLOCKPERIOD, SC_NS);
// Reset signal - true high
sc_signal<bool> ResetGen_1;
// Reset signal - true low
sc_signal<bool> ResetGen_low_1;

// DMA signals
sc_signal<PINOUT>        *pinoutwrappertodma;
sc_signal<bool>          *readywrappertodma;
sc_signal<bool>          *requestwrappertodma;
sc_signal<DMA_CONT_REG1> *datadmacontroltotransfer;
sc_signal<bool>          *requestcontroltotransfer;
sc_signal<bool>          *finishedtransfer;
sc_signal<PINOUT>        *spinoutscratchdma;
sc_signal<bool>          *sreadyscratchdma;
sc_signal<bool>          *srequestscratchdma;

//arm_system or dma_transfer inout_signals (core frequency)
sc_signal<bool>          *readymast;
sc_signal<PINOUT>        *pinoutmast;
sc_signal<bool>          *requestmast;
//arm_system or dma_transfer inout_signals (interconnect frequency)
sc_signal<bool>          *readymast_interconnect;
sc_signal<PINOUT>        *pinoutmast_interconnect;
sc_signal<bool>          *requestmast_interconnect;

//interrupts
sc_signal<bool>          *extint;

// FFT signals
sc_signal<PINOUT>       *pinoutmasterfft;
sc_signal<bool>         *readymastertofft;
sc_signal<bool>         *requestffttomaster;
