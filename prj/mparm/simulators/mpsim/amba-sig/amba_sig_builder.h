///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_builder.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Manages AMBA AHB (signal) platform instantiation
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_SIG_BUILDER_H__
#define __AMBA_SIG_BUILDER_H__

void buildAMBAAHBSigPlatform(int new_argc, char *new_argv[], char *new_envp[]);

#endif // __AMBA_SIG_BUILDER_H__
