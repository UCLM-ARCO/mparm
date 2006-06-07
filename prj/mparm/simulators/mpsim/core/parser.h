///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         parser.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Performs command line option parsing
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARSER_H__
#define __PARSER_H__

void parseArgs(int argc, char *argv[], char *envp[], int *new_argc, char **new_argv[], char **new_envp[]);

#endif // __PARSER_H__
