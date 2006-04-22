///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_builder.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Manages STBus platform instantiation
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ST_BUILDER_H__
#define __ST_BUILDER_H__

void buildSTBusPlatform(int new_argc, char *new_argv[], char *new_envp[]);

//Needed for the statitcs dumps of lx cores
void destroySTBusPlatform();
#endif // __ST_BUILDER_H__
