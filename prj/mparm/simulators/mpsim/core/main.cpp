///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         main.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Platform loader. Initiates and starts SystemC simulation
//
///////////////////////////////////////////////////////////////////////////////

#include <systemc.h>
#include <unistd.h>
#include "stats.h"
#include "sim_support.h"
#include "address.h"
#include "parser.h"
#include "globals.h"
#include "power.h"
#include "config.h"

// Platform-specific includes. Don't include files for platforms we're
// not building (such files may be not redistributable!)
#ifdef AMBAOCCNBUILD
  #include "amba_occn_trace.h"
  #include "amba_occn_builder.h"
#endif
#ifdef AMBAAHBBUILD
  #include "amba_ahb_platform.h"
#endif
#ifdef AMBASIGBUILD
  #include "amba_sig_trace.h"
  #include "amba_sig_builder.h"
#endif
#ifdef AMBAAHBSYNBUILD
  #include "ahb_shared.h"
  //#include "ahb_multi_layering.h"
#endif
#ifdef AMBAAXISYNBUILD
  #include "axi_syn.h"
#endif
#ifdef STBUSBUILD
  #include "st_trace.h"
  #include "st_builder.h"
#endif
#ifdef XPIPESBUILD
  #include "xpipes_platform.h"
#endif
#ifdef WITH_POWER_NODE
  #include "power_stbus_sysc_lib.h"
  #include <SC_POWER_OBJECT.h>
#endif

int sc_main(int argc, char *argv[])
{
  sc_trace_file *tf = NULL;
  int new_argc;
  char **new_argv;
  char **new_envp;

#ifdef AMBAAHBBUILD
  amba_ahb_platform *amba_ic = NULL;
  HAVE_AMBA_AHB = true;
#else
  HAVE_AMBA_AHB = false;
#endif
#ifdef AMBASIGBUILD
  HAVE_AMBA_SIG = true;
#else
  HAVE_AMBA_SIG = false;
#endif
#ifdef AMBAAHBSYNBUILD
  SharedAHB *ahbshared;
  //MultiLayerAHB *ahbml;
  HAVE_AMBA_AHB_SYN = true;
#else
  HAVE_AMBA_AHB_SYN = false;
#endif
#ifdef AMBAAXISYNBUILD
  axi_syn<(const bool)true, (const bool)false> *axisyn = NULL;
  HAVE_AMBA_AXI_SYN = true;
#else
  HAVE_AMBA_AXI_SYN = false;
#endif
#ifdef AMBAOCCNBUILD
  HAVE_AMBA_OCCN = true;
#else
  HAVE_AMBA_OCCN = false;
#endif
#ifdef STBUSBUILD
  HAVE_STBUS = true;
#else
  HAVE_STBUS = false;
#endif
#ifdef XPIPESBUILD
  xpipes_platform *xpipes;
  HAVE_XPIPES = true;
#else
  HAVE_XPIPES = false;
#endif
#ifdef SWARMBUILD
  HAVE_SWARM = true;
#else
  HAVE_SWARM = false;
#endif
#ifdef SIMITARMBUILD
  HAVE_SIMITARM = true;
#else
  HAVE_SIMITARM = false;
#endif
#ifdef POWERPCBUILD
  HAVE_POWERPC = true;
#else
  HAVE_POWERPC = false;
#endif
#ifdef LXBUILD
  HAVE_LX = true;
#else
  HAVE_LX = false;
#endif
#ifdef TGBUILD
  HAVE_TG = true;
#else
  HAVE_TG = false;
#endif

  // Command line option parsing
  parseArgs(argc, argv, environ, &new_argc, &new_argv, &new_envp);
  
#ifdef REDUCED_RELEASE
  printf("\n\n\n--------WARNING--------\nThis is a PREVIEW version of the simulator, with many features disabled!\n");
  printf("Please contact the project lead for a full version release:\n");
  printf("Prof. Luca Benini\nDEIS-Universita' di Bologna\nViale Risorgimento, 2\n40136 Bologna (ITALY)\n");
  printf("lbenini@deis.unibo.it\nTel +39 051 2093782\nFax +39 051 2093785\n\n\n\n");
#endif
  
  // System management of addresses and translations. Must
  // be created before the cores are, or the platform will fail
  addresser = new Addresser();
  
  // Signal tracing
  if (VCD)
  {
    tf = sc_create_vcd_trace_file("waves");
    ((vcd_trace_file*)tf)->sc_set_vcd_time_unit(-10);  // 10^-10 s = 100 ps
                                                       // notice: has to be smaller than the default time unit
                                                       // (clock has two edges per time unit!)
  }

  // Hardware platform instantiation
  switch (CURRENT_INTERC)
  {
#ifdef STBUSBUILD
    case STBUS:     buildSTBusPlatform(new_argc, new_argv, new_envp);
                    break;
#endif
#ifdef AMBASIGBUILD
    case AMBASIG:   buildAMBAAHBSigPlatform(new_argc, new_argv, new_envp);
                    break;
#endif
#ifdef AMBAAHBBUILD
    case AMBAAHB:   amba_ic = new amba_ahb_platform("amba_ahb", tf, VCD, new_argc, new_argv, new_envp);
                    break;
#endif
#ifdef AMBAAHBSYNBUILD
    case AMBAAHBSYN: ahbshared = new SharedAHB("AHB_SYN");
                     //ahbml = new MultiLayerAHB("AHB_SYN");
                     break;
#endif
#ifdef AMBAAXISYNBUILD
    case AMBAAXISYN: axisyn = new axi_syn<(const bool)true, (const bool)false>("AXI_SYN", new_envp);
                     break;
#endif
#ifdef AMBAOCCNBUILD
    case AMBAOCCN:  buildAMBAAHBOCCNPlatform(new_argc, new_argv, new_envp);
                    break;
#endif
#ifdef XPIPESBUILD
    case XPIPES:    xpipes = new xpipes_platform("xpipes", tf, VCD, new_argc, new_argv, new_envp);
                    break;
#endif
    default:        printf("Fatal Error: Error in interconnection instantiation parameters, an unavailable interconnect was selected!\n");
                    exit(1);
  }

  // Statistics collection. Please leave the instantiation below
  // that of the cores
  statobject = new Statistics();

  // Statistics synchronizer with clock
  synchronizer sync("sync");
               sync.clock(ClockGen_1);
               
  simsuppobject = new Sim_Support(new_argc, new_argv, new_envp);

  if (POWERSTATS)
  {
    power_object  = new power(N_CORES, N_SLAVES, N_BRIDGES, N_BUSES);
#ifdef RAM_COUNTER_WORKAROUND
    ram_access_object = new ram_access(N_SLAVES);
    cache_access_counter_object = new cache_access_counter(N_CORES);
#endif
  }
  
  // Signal tracing
  if (VCD)
  {
    switch (CURRENT_INTERC)
    {
#ifdef STBUSBUILD
      case STBUS:     traceSTBusSignals(tf);
                      break;
#endif
#ifdef AMBASIGBUILD
      case AMBASIG:   traceAMBAAHBSigSignals(tf);
                      break;
#endif
#ifdef AMBAAHBBUILD
      case AMBAAHB:   break;
#endif
#ifdef AMBAAHBSYNBUILD
      case AMBAAHBSYN: break;
#endif
#ifdef AMBAAXISYNBUILD
      case AMBAAXISYN: break;
#endif
#ifdef AMBAOCCNBUILD
      case AMBAOCCN:  traceAMBAAHBOCCNSignals(tf);
                      break;
#endif
#ifdef XPIPESBUILD
      case XPIPES:    break;
#endif
      default:        printf("Fatal Error: Error in interconnection tracing parameters, an unavailable interconnect was selected!\n");
                      exit(1);
    }
  }

  sc_start(NSIMCYCLES, SC_NS);

  // Needed to dump statistics of LX cores
  switch (CURRENT_INTERC)
  {
#ifdef STBUSBUILD
       case STBUS:         destroySTBusPlatform();
                           break;
#endif
#ifdef AMBAAHBBUILD
       case AMBAAHB:       amba_ic->destroy_amba_ahb_platform();
         		   amba_ic->~amba_ahb_platform();
                           break;
#endif
#ifdef AMBAAXISYNBUILD
       case AMBAAXISYN:    delete axisyn;
                           break;
#endif
       default:            break;
  }

  if (VCD)
    sc_close_vcd_trace_file(tf);

  if (POWERSTATS)
  {
    finalize_power();
#ifdef WITH_POWER_NODE
    if (CURRENT_INTERC == STBUS)
      pow_e_print_power_report();
#endif
  }

  return(0);
}
