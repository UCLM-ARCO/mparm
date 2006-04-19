///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         slaving.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Logically ORs ready signals from slaves
//
///////////////////////////////////////////////////////////////////////////////

#include "slaving.h"

///////////////////////////////////////////////////////////////////////////////
// logicalor - Performs a logical OR of the ready signals of every slave in the
//             system. The output is the system-wide hready signal.
void slaving::logicalor()
{
    hready.write(false);   
    for (int i=0; i<N_SLAVES;i++)
      if (ready[i].read()==true)
        hready.write(true);
}
