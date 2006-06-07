///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         debug.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Basic debug routines
//
///////////////////////////////////////////////////////////////////////////////

#ifdef OUTDEBUG

#include <stdlib.h>
#include "debug.h"
#include "address.h"

//if (level>TRACEX_LEVEL || !statobject->IsMeasuring(0) )

void TRACE(int level, char *format, ...)
{
  va_list ap;
  if (level>TRACELEVEL)
    return;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
}


unsigned int TRACEX_FLAGS = TRACEX_FLAGS_NONE;
unsigned int TRACEX_LEVEL = 5;

void TRACEX(int flags, int level, char *format, ...)
{
    va_list ap;
    if ( flags & TRACEX_FLAGS ) {
        if (level>(int)TRACEX_LEVEL)
            return;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
    }
}

/* to initialize the TRACEX_FLAGS and TRACEX_LEVEL variables */
class dummyClass {
  public:
  dummyClass() {
      static int initialized=0;
      char *env;
      ASSERT (!initialized); /* Only one dummyClass object is needed */
      initialized = 1;
      env = getenv("TRACEX");
      if (env)
          TRACEX_FLAGS = strtoul(env, NULL, 0);
      env = getenv("TRACEX_LEVEL");
      if (env)
          TRACEX_LEVEL = strtoul(env, NULL, 0);
  };
} dummy;


#endif

