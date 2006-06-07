///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         arbiter.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Arbiter for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#include "arbiter.h"

///////////////////////////////////////////////////////////////////////////////
// arbitering - Monitors bus requests coming from masters by invoking granting(),
//              and assigns address/control bus ownership with the hgrant and
//              hmaster signals. Since data timings are delayed wrt address/control
//              timings, the latter get reassigned just before the last trasnfer
//              of a transaction.
void arbiter::arbitering()
{
  int burst = 0, prev, next, k;
  request mastin;
  bool alreadywaitedlasthready = false;

#ifdef PRINTDEBUG
    cout << "Arbiter started" << endl;
#endif

  while ((prev = granting()) != 0)             // Initialization: force processor 0 to be the first to be granted the bus
    wait();
  hgrant[prev].write(true);
  hmaster.write ((sc_uint<5>)prev);
  wait_until(hready.delayed() == false);


  while (true)
  {
    mastin = ctrl_signals.read();                  // control signals for the new transaction are available

    if (mastin.hburst == AMBA_SIG_HBURST_SINGLE)   // wait until the end of the penultimate transfer of the
      burst = 1;                                   // current transaction (do nothing if it is a single transfer)
    if (mastin.hburst == AMBA_SIG_HBURST_INCR4)
      burst = 4;
    if (mastin.hburst == AMBA_SIG_HBURST_INCR8)
      burst = 8;
    if (mastin.hburst == AMBA_SIG_HBURST_INCR16)
      burst = 16;
    for (k = 1; k < burst; k++)
      wait_until (hready.delayed() == true);

    hgrant[prev].write(false);                 // try to rearbiter. Take address/control away from the currently active master
    hmaster.write ((sc_uint<5>)31);

    alreadywaitedlasthready = false;
    next = granting();                         // search for a new master with asserted requests
    if (next < 0 || (burst == 1 && next == prev))
      do                                       // if not found, or if detecting the same request just served,
      {                                        // wait until one is found, remembering if the final
#if 0
        wait();                                // transfer of the current transaction has meanwhile completed
#else
        wait_until (hready.delayed () == true); // AST-OCCN: This is to be compliant with OCCN Arbiter !!!!!
#endif
        if (hready.read() == true)
          alreadywaitedlasthready = true;
      } while ((next = granting()) < 0);

    hgrant[next].write(true);                 // reassign address and control to the new master
    hmaster.write ((sc_uint<5>)next);
    prev = next;

    if (!alreadywaitedlasthready)
      wait_until (hready.delayed () == true);  // last HREADY of the current transaction
    else                                       // new transaction already underway; now needed is its burst size, which will only
      wait();                                  // be stable one cycle after hmaster. This wait() won't break 0 WS operation, as it
  }                                            // overlaps with the address cycle of the first transfer of the new transaction

}



///////////////////////////////////////////////////////////////////////////////
// granting - Cycles through external bus requests and chooses current best
//            candidate basing upon a round robin policy.
int arbiter::granting()
{
  static int previous = -1;
  int i;

  for (i = previous + 1; i < N_MASTERS; i++)
    if (hreq[i].read() == true)
    {
      previous = i;
      if (previous == N_MASTERS - 1)
        previous = -1;
      return(i);
    }
    for (i = 0; i <= previous; i++)
    if (hreq[i].read() == true)
    {
      previous = i;
      if (previous == N_MASTERS - 1)
        previous = -1;
      return(i);
    }
  return(-1);
}
