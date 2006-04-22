///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         stats.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Statistics collection
//
///////////////////////////////////////////////////////////////////////////////

#include "stats.h"
#include "power.h"
#include "config.h"
#include "globals.h"
#include "address.h"
#include <time.h>
#include <math.h>

Statistics *statobject;

///////////////////////////////////////////////////////////////////////////////
// Constructor - Initializes statistics collection.
//               ACCTRACE: Enables disk logging of memory accesses made by
//                         processors and DMA controllers.
//               SPCHECK: Enables extrapolation of the performance of a core
//                        with an associated scratchpad memory.
Statistics::Statistics()
{
  start_time = time(NULL);                                       // Simulation execution starting time

  ext_scratchr = new unsigned long int [N_CORES];
  ext_scratchw = new unsigned long int [N_CORES];
  ext_smartmemr = new unsigned long int [N_SMARTMEM];
  ext_smartmemw = new unsigned long int [N_SMARTMEM];

  prev_addr = new uint32_t [N_CORES];
  prev_mode = new PPROC [N_CORES];
  prev_di = new bool [N_CORES];
  prev_time = new float [N_CORES];
  w_cycles = new uint32_t [N_CORES];
  
  master_c = new ACCESS_COUNTERS[N_MASTERS];
  master_ocp_c = new OCP_COUNTERS[N_MASTERS];
  master_s = new MASTER_STATUS[N_MASTERS];
  master_ocp_s = new OCP_STATUS[N_MASTERS];
  core_ct = new CORE_CYCLE_TYPE[N_CORES];
  core_c = new CORE_COUNTERS[N_CORES];
  
  start_time_crit = new double [N_CORES];
  start_sim_time_crit = new time_t [N_CORES];

  status = new CORE_STATUS [N_CORES];
  
  // HACK. See inspectSTbusAccess()
  if (CURRENT_INTERC == STBUS)
    STBUS_DIVIDER_BOOT = I_DIVIDER[0];
  else
    STBUS_DIVIDER_BOOT = 1;
  
#ifdef XPIPESBUILD
  max_xpipes_registered_nis = N_MASTERS + N_SLAVES;
  max_xpipes_registered_buffers = MAX_XPIPES_BUFFERS;
  max_xpipes_registered_link_receivers = MAX_XPIPES_RECEIVERS;
  xpipes_ni_status = new XPIPES_NI_STATUS [max_xpipes_registered_nis];
  xpipes_buffer_status = new XPIPES_BUFFER_STATUS [max_xpipes_registered_buffers];
  xpipes_link_receiver_status = new XPIPES_LINK_REC_STATUS [max_xpipes_registered_link_receivers];
  
  for (uint i = 0; i < max_xpipes_registered_nis; i ++)
  {
    // One-way latencies per target...
    xpipes_ni_status[i].min_ow_latency = new unsigned long int * [max_xpipes_registered_nis];
    xpipes_ni_status[i].max_ow_latency = new unsigned long int * [max_xpipes_registered_nis];
    xpipes_ni_status[i].tot_ow_latency = new unsigned long long int * [max_xpipes_registered_nis];
    // Round-trip latencies per target...
    xpipes_ni_status[i].min_rt_latency = new unsigned long int * [max_xpipes_registered_nis];
    xpipes_ni_status[i].max_rt_latency = new unsigned long int * [max_xpipes_registered_nis];
    xpipes_ni_status[i].tot_rt_latency = new unsigned long long int * [max_xpipes_registered_nis];
    // Packet counts per target...
    xpipes_ni_status[i].sent_packets   = new unsigned long int * [max_xpipes_registered_nis];
  
    // ... and per packet type: SR (single read),
    //                          SWP (single write - posted),
    //                          SWNP (single write - non posted),
    //                          BR (burst read),
    //                          BWP (burst write - posted),
    //                          BWNP (burst write - non posted)
    for (uint j = 0; j < max_xpipes_registered_nis; j ++)
    {
      xpipes_ni_status[i].min_ow_latency[j] = new unsigned long int [6];
      xpipes_ni_status[i].max_ow_latency[j] = new unsigned long int [6];
      xpipes_ni_status[i].tot_ow_latency[j] = new unsigned long long int [6];
      xpipes_ni_status[i].min_rt_latency[j] = new unsigned long int [6];
      xpipes_ni_status[i].max_rt_latency[j] = new unsigned long int [6];
      xpipes_ni_status[i].tot_rt_latency[j] = new unsigned long long int [6];
      xpipes_ni_status[i].sent_packets[j]   = new unsigned long int [6];
    }
    
    xpipes_ni_status[i].in_flight_packets = new XPIPES_PACKET_INFO [MAX_IN_FLIGHT_PACKETS + 1];
  }
#endif
  
  resetValues();
  readytoterm = 0;

  for (uint i = 0; i < N_CORES; i ++)
  {
  
    status[i] = READY_TO_MEASURE;
    status_global = READY_TO_MEASURE;
    
    if (AUTOSTARTMEASURING)
      startMeasuring(i);

    if (SPCHECK)
    {
      core_c[i].scratchrangenumber = addresser->ReturnRangeNumber(i);
      if (core_c[i].scratchrangenumber > 0)
      {
        core_c[i].scratchrrange = new uint64_t [core_c[i].scratchrangenumber];
        core_c[i].scratchwrange = new uint64_t [core_c[i].scratchrangenumber];
      }
    }
  }

  if (ACCTRACE)    // Opens trace output files
  {
    ftrace = new FILE* [N_MASTERS];
    for (uint i=0; i < N_MASTERS; i++)
    {
      sprintf(outname, "trace%d.tra", i);
      ftrace[i] = fopen(outname, "w");
      if (!ftrace[i])
      {
        fprintf(stderr, "Error opening trace output file %s for writing\n", outname);
        exit(1);
      }
    }
  }

  fstat = fopen(STATSFILENAME.c_str(), "w");
  res_file = fopen("stats_light.txt", "w");
  if (!fstat || !res_file)
  {
    fprintf(stderr, "Error opening statistics output files (%s or ", STATSFILENAME.c_str());
    fprintf(stderr, "stats_light.txt) for write\n");
    exit(1);
  }
}


///////////////////////////////////////////////////////////////////////////////
// Destructor - Closes files and dumps information before quitting.
Statistics::~Statistics()
{
  if (ACCTRACE)
    for (uint i=0; i<N_MASTERS; i++)
      fclose(ftrace[i]);
  fclose(fstat);
}


///////////////////////////////////////////////////////////////////////////////
// dumpEverything - Dumps all collected statistics.
void Statistics::dumpEverything()
{
  printSimParameters();
  fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  printTimeResults();
  fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
#ifdef XPIPESBUILD
  if (CURRENT_INTERC == XPIPES)
    printXpipesResults();
  else
#endif
    printInterconnectionResults();
  fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  if (USING_OCP)
  {
    printOCPLatencyResults();
    fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  }
 
  for (uint i=0; i<N_CORES; i++)
  {
   switch (CURRENT_ISS)
   {
    case SWARM: printSWARMCoreResults(i);
     break;
    case LX: printLXCoreResults(i);
     break;
    default:
     break; 
   }
   
   fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  }
 
  if (SMARTMEM)
  {
    for (uint i=0 ; i<N_SMARTMEM; i++)
      printSmartmem(i+N_CORES+(DMA*N_CORES));
    fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  }
  
  if (SCRATCH || CORESLAVE)
  {
    for (uint i=0 ; i<N_CORES; i++)
      printExtscratch(i);
    fprintf(fstat, "\n\n---------------------------------------------------------------------------------\n\n\n");
  }
  
  if (POWERSTATS)
  {
    double tc[N_CORES];
    for (uint i = 0; i < N_CORES; i++)
      tc[i] = core_c[i].total_time_crit;

    fprintf(fstat, "==============================================================================\n");
    fprintf(fstat, "----------------\nPower estimation\n----------------\n");
    fprintf(fstat, "\n  Energy spent:\n");
    power_object->dump(fstat);
    fprintf(fstat, "------------------------------------------------------------------------------\n");
    fprintf(fstat, "\n  Power spent:\n");
    power_object->dump(fstat, tc, global_total_time_crit);
    fprintf(fstat, "==============================================================================\n");
#ifdef RAM_COUNTER_WORKAROUND
    ram_access_object->dump(fstat);
    fprintf(fstat, "==============================================================================\n");
    cache_access_counter_object->dump(fstat);
    fprintf(fstat, "==============================================================================\n");
#endif
  }
  
  #if defined N_TGEN && defined LXBUILD
    printTgenResults();
  #endif
}


///////////////////////////////////////////////////////////////////////////////
// dump - Dumps statistics collected by one processor.
void Statistics::dump(uint ID)
{
 switch (CURRENT_ISS)
   {
    case SWARM: printSWARMCoreResults(ID);
     break;
    case LX: printLXCoreResults(ID);
     break;
    default:
     break; 
   }
}


///////////////////////////////////////////////////////////////////////////////
// dump_light - Dumps restricted statistics collected by one processor and by 
//              the global objects.
void Statistics::dump_light(uint ID)
{
  printf("%u\t%10.1f\t%10.1f\t%lu\t%lu\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\n",
         ID,
         start_time_crit[ID],
         start_time_crit[ID] + core_c[ID].total_time_crit,
         master_c[ID].accesses,
         master_c[ID].tot_waittime,
         power_object->cores[ID],
         power_object->icaches[ID],
         power_object->dcaches[ID],
         power_object->scratches[ID],
         power_object->iscratches[ID],
         power_object->rams[ID],
         power_object->rams[addresser->SharedStartID()],
         power_object->buses_typ[0]);
  fprintf(res_file,
          "%u\t%10.1f\t%10.1f\t%lu\t%lu\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\n",
          ID,
          start_time_crit[ID],
          start_time_crit[ID] + core_c[ID].total_time_crit,
          master_c[ID].accesses,
          master_c[ID].tot_waittime,
          power_object->cores[ID],
          power_object->icaches[ID],
          power_object->dcaches[ID],
          power_object->scratches[ID],
          power_object->iscratches[ID],
          power_object->rams[ID],
          power_object->rams[addresser->SharedStartID()],
          power_object->buses_typ[0]);
}


///////////////////////////////////////////////////////////////////////////////
// clear - Reset collected values
void Statistics::clear()
{
  resetValues();
  power_object->resetValues();
}


////////////////////////////////////////////////////////////////////////////////
// printSimParameters - Dumps simulation platform configuration.
void Statistics::printSimParameters()
{
  fprintf(fstat, "Statistics file: %s\n\n\n", STATSFILENAME.c_str());
  
  if (strcmp("", CFGFILENAME.c_str()))
    fprintf(fstat, "Platform configuration read from file: %s\n\n\n", CFGFILENAME.c_str());
  
  switch (CURRENT_ISS)
  {
    case SWARM:     fprintf(fstat, "Simulation executed with SWARM cores");
                    break;
    case SIMITARM:  fprintf(fstat, "Simulation executed with SimIt-ARM cores");
                    break;
    case POWERPC:   fprintf(fstat, "Simulation executed with PowerPC cores");
                    break;
    case LX:        fprintf(fstat, "Simulation executed with LX cores");
                    break;
    case TG:        fprintf(fstat, "Simulation executed with Traffic Generators");
                    break;
    default:        printf("Fatal Error: Error in ISS parameters!\n");
                    exit(1);
  }
  
  switch (CURRENT_INTERC)
  {
    case STBUS:       fprintf(fstat, " on STBus interconnection\n");
                      break;
    case AMBASIG:     fprintf(fstat, " on AMBA AHB (signal model) interconnection\n");
                      break;
    case AMBAAHB:     fprintf(fstat, " on AMBA AHB (interface model) interconnection\n");
                      break;
    case AMBAOCCN:    fprintf(fstat, " on AMBA AHB (OCCN model) interconnection\n");
                      break;
    case AMBAAHBSYN:  fprintf(fstat, " on AMBA AHB (Synopsys model) interconnection\n");
                      break;
    case AMBAAXISYN:  fprintf(fstat, " on AMBA AXI (Synopsys model) interconnection\n");
                      break;
    case XPIPES:      fprintf(fstat, " on xpipes interconnection\n");
                      break;
    default:          printf("Fatal Error: Error in interconnection parameters!\n");
                      exit(1);
  }
  
  fprintf(fstat, "Simulation executed with %hu buses connected by %hu bridges\n", N_BUSES, N_BRIDGES);
  if (N_BUSES > 1)
  {
    fprintf(fstat, "Masters mapped onto bus IDs:");
    for (uint i = 0; i < N_MASTERS; i ++)
      fprintf(fstat, " %hu", MASTER_CONFIG[i].binding);
    fprintf(fstat, "\nSlaves mapped onto bus IDs:");
    for (uint i = 0; i < N_SLAVES; i ++)
      fprintf(fstat, " %hu", SLAVE_CONFIG[i].binding);
    fprintf(fstat, "\n");
  }
  if (N_BRIDGES > 0)
  {
    for (uint i = 0; i < N_BRIDGES; i ++)
    {
      fprintf(fstat, "Bridge %hu connected as master on bus %hu, slave on bus %hu, and mapping %hu address ranges:\n",
        i, BRIDGE_CONFIG[i].master_binding, BRIDGE_CONFIG[i].slave_binding, BRIDGE_CONFIG[i].n_ranges);
      for (uint j = 0; j < BRIDGE_CONFIG[i].n_ranges; j ++)
        fprintf(fstat, "  0x%08x - 0x%08x\n", BRIDGE_CONFIG[i].start_address[j], BRIDGE_CONFIG[i].end_address[j]);
    }
  }
  
  fprintf(fstat, "Simulation executed with %hu cores (%hu masters including DMAs and smart memories)\n", N_CORES, N_MASTERS);
  if (N_IP_TG > 0)
    fprintf(fstat, "%hu ST IP Traffic Generators were instantiated\n",N_IP_TG);
  fprintf(fstat, "%hu slaves: %hu private, %hu shared, %hu semaphores, %hu interrupt,\n",
            N_SLAVES, N_PRIVATE, N_SHARED, N_SEMAPHORE, N_INTERRUPT);
  fprintf(fstat, "          %hu core-associated, %hu storage, %hu frequency scaling,\n",
            N_CORESLAVE, N_STORAGE, N_FREQ);
  fprintf(fstat, "          %hu smart memories, %hu FFT devices\n", N_SMARTMEM, N_FFT);
  fprintf(fstat, "          (core-associated %s, frequency scaling %s, smart memories %s, DRAM controller %s)\n",
            CORESLAVE ? "on" : "off", FREQSCALINGDEVICE ? "on" : "off", SMARTMEM ? "on" : "off", DRAM ? "on" : "off");
  fprintf(fstat, "DMA controllers %s\n", DMA ? "enabled" : "disabled");
  
  fprintf(fstat, "Scratchpad memories %s", SCRATCH ? "enabled" : "disabled\n");
  if (SCRATCH)
    fprintf(fstat, " (having %lu bytes size and %hu wait states)\n", SCRATCH_SIZE, SCRATCH_WS);
  fprintf(fstat, "Instruction scratchpad memories %s", ISCRATCH ? "enabled" : "disabled\n");
  if (ISCRATCH)
    fprintf(fstat, " (size %lu)\n", ISCRATCHSIZE);
    
  fprintf(fstat, "Queue memories %s", CORESLAVE ? "enabled" : "disabled");
  if (CORESLAVE)
    fprintf(fstat, " (%lu bytes size for the scratchpad, %lu bytes size for the semaphores)\n", SCRATCH_SIZE, QUEUE_SIZE);
  else
    fprintf(fstat, "\n");
  fprintf(fstat, "Advanced statistics %s, ", STATS ? "on" : "off");
  fprintf(fstat, "Access traces %s, ", ACCTRACE ? "on" : "off");
  fprintf(fstat, "TG traces %s\n", TG_TRACE_COLLECTION ? "on" : "off");
  fprintf(fstat, "Simulation executed %s OCP interfacing (where applicable)\n", USING_OCP ? "with" : "without");
#ifndef NOSNOOP
  fprintf(fstat, "Snooping for cache coherence %s, ", SNOOPING ? "on" : "off");
  if (SNOOPING)
    fprintf(fstat, "Snoop policy is %s, ",
            (SNOOP_POLICY == SNOOP_INVALIDATE) ? "invalidate" :
            (SNOOP_POLICY = SNOOP_UPDATE)      ? "update"     : "unknown" );
#endif

  fprintf(fstat, "Master system clock period set to %.2f ns\n", (float)CLOCKPERIOD);
  fprintf(fstat, "VCD waveforms %s\n", VCD ? "on" : "off");
  fprintf(fstat, "Partitioned scratchpad analysis %s, ", SPCHECK ? "on" : "off");
  fprintf(fstat, "/dev/pts prompt %s\n", SHOWPROMPT ? "presented" : "skipped");
  if (SHARED_CACHE)
  {
    fprintf(fstat, "Unified cache of %lu bytes, ", UCACHESIZE);
    switch (UCACHETYPE)
    {
      case SETASSOC: fprintf(fstat, "%u-way set associative", UCACHEWAYS);
                     break;
      case DIRECT:   fprintf(fstat, "direct mapped");
                     break;
      case FASSOC:   fprintf(fstat, "fully associative");
                     break;
      default:       printf("Fatal Error: Error in Unified Cache parameters!\n");
                     exit(1);
    }
    fprintf(fstat, ", having %hu wait states\n", UCACHE_WS);
  }
  else
  {
    fprintf(fstat, "Data cache of %lu bytes, ", DCACHESIZE);
    switch (DCACHETYPE)
    {
      case SETASSOC: fprintf(fstat, "%u-way set associative", DCACHEWAYS);
                     break;
      case DIRECT:   fprintf(fstat, "direct mapped");
                     break;
      case FASSOC:   fprintf(fstat, "fully associative");
                     break;
      default:       printf("Fatal Error: Error in Data Cache parameters!\n");
                     exit(1);
    }
    fprintf(fstat, ", having %hu wait states\n", DCACHE_WS);

    fprintf(fstat, "Cache write policy: ");
    switch (CACHE_WRITE_POLICY)
    {
      case WT:
        fprintf(fstat, "write through\n");
        break;
      case WB:
        fprintf(fstat, "write back with normal round robin\n");
        break;
    }
    
    fprintf(fstat, "Instruction cache of %lu bytes, ", ICACHESIZE);
    switch (ICACHETYPE)
    {
      case SETASSOC: fprintf(fstat, "%u-way set associative", ICACHEWAYS);
                     break;
      case DIRECT:   fprintf(fstat, "direct mapped");
                     break;
      case FASSOC:   fprintf(fstat, "fully associative");
                     break;
      default:       printf("Fatal Error: Error in Instruction Cache parameters!\n");
                     exit(1);
    }
    fprintf(fstat, ", having %hu wait states\n", ICACHE_WS);
  }

  fprintf(fstat, "Simulation executed with %s\n", 
            FREQSCALINGDEVICE ? "dynamic frequency scaling" :
            (FREQSCALING ? "static frequency scaling" : "isofrequential system"));
  if (FREQSCALING || FREQSCALINGDEVICE)
  {
    fprintf(fstat, "Master clock dividers set to:");
    for (int i = 0; i < N_FREQ_DEVICE; i ++)
      fprintf(fstat, " %hu", M_DIVIDER[i]);
    fprintf(fstat, "\nInterconnect clock dividers set to:");
    for (uint i = 0; i < N_BUSES; i ++)
      fprintf(fstat, " %hu", I_DIVIDER[i]);
    fprintf(fstat, "\nPLL delays (in master system clock cycles) set to:");
    for (uint i = 0; i < (uint)(N_FREQ_DEVICE + N_BUSES); i ++)
      fprintf(fstat, " %hu", PLL_DELAY[i]);
    fprintf(fstat, "\n");
  }
  
  fprintf(fstat, "Latencies: interrupts %hu, memories %hu (initial) %hu (back-to-back)\n", INT_WS, MEM_IN_WS, MEM_BB_WS); 
  fprintf(fstat, "Statistics collected since %s\n", AUTOSTARTMEASURING ? "system boot" : "benchmark request");
  if (NSIMCYCLES > 0)
    fprintf(fstat, "Simulation stopped by force after %ld cycles\n", NSIMCYCLES);
}

#if defined N_TGEN && defined LXBUILD
void Statistics::printTgenResults(){
  fprintf(fstat, "\n-------------------------\n");
  fprintf(fstat, "Traffic Generator statistics\n");
  fprintf(fstat, "-------------------------\n");
  fprintf(fstat, "Read TGEN max queue size  = %d\n", tgen_read_max_fifo );
  fprintf(fstat, "Write TGEN max queue size = %d\n", tgen_write_max_fifo );
}
#endif

///////////////////////////////////////////////////////////////////////////////
// resetValues - Initializes all counters.
void Statistics::resetValues()
{
  for (uint i = 0; i < N_MASTERS; i ++)
  {
    memset(&master_c[i], 0, sizeof(ACCESS_COUNTERS));
    // Non-zero initializations
    master_c[i].min_waittime    = 100000;
    master_c[i].min_waittime_sr = 100000;
    master_c[i].min_waittime_sw = 100000;
    master_c[i].min_waittime_br = 100000;
    master_c[i].min_waittime_bw = 100000;
    master_c[i].min_waittime_r  = 100000;
    master_c[i].min_waittime_w  = 100000;
    master_c[i].min_comptime    = 100000;
    master_c[i].min_comptime_sr = 100000;
    master_c[i].min_comptime_sw = 100000;
    master_c[i].min_comptime_br = 100000;
    master_c[i].min_comptime_bw = 100000;
    master_c[i].min_comptime_r  = 100000;
    master_c[i].min_comptime_w  = 100000;
    memset(&master_s[i], 0, sizeof(MASTER_STATUS));
    
    memset(&master_ocp_c[i], 0, sizeof(OCP_COUNTERS));
    // Non-zero initializations
    master_ocp_c[i].min_cmdacctime      = 100000;
    master_ocp_c[i].min_cmdacctime_sr   = 100000;
    master_ocp_c[i].min_cmdacctime_swp  = 100000;
    master_ocp_c[i].min_cmdacctime_swnp = 100000;
    master_ocp_c[i].min_cmdacctime_br   = 100000;
    master_ocp_c[i].min_cmdacctime_bwp  = 100000;
    master_ocp_c[i].min_cmdacctime_bwnp = 100000;
    master_ocp_c[i].min_cmdacctime_r    = 100000;
    master_ocp_c[i].min_cmdacctime_wp   = 100000;
    master_ocp_c[i].min_cmdacctime_wnp  = 100000;
    master_ocp_c[i].min_comptime        = 100000;
    master_ocp_c[i].min_comptime_sr     = 100000;
    master_ocp_c[i].min_comptime_swnp   = 100000;
    master_ocp_c[i].min_comptime_br     = 100000;
    master_ocp_c[i].min_comptime_bwnp   = 100000;
    master_ocp_c[i].min_comptime_r      = 100000;
    master_ocp_c[i].min_comptime_wnp    = 100000;
    memset(&master_ocp_s[i], 0, sizeof(OCP_STATUS));
  }

  global_total_time_crit = 0.0;
  
  for (uint i = 0; i < N_CORES; i ++)
  {
    ext_scratchr[i] = 0;
    ext_scratchr[i] = 0;
  }
      
  for (uint i = 0; i < N_SMARTMEM; i ++)
  {
    ext_smartmemr[i] = 0;
    ext_smartmemw[i] = 0;
  }

  for (uint i = 0; i < N_CORES; i ++)
  {
      memset(&core_c[i], 0, sizeof(CORE_COUNTERS));
        
      if (SPCHECK)
        for (unsigned short int j = 0; j < core_c[i].scratchrangenumber; j++)
        {
          core_c[i].scratchrrange[j] = 0;
          core_c[i].scratchwrange[j] = 0;
        }
      
      core_ct[i] = NONE;

      prev_addr[i] = 0xFFFFFFFF;
      prev_mode[i] = P_NORMAL;
      prev_di[i] = false;   // instruction (see core.h)
      prev_time[i] = 0.0;
      
      w_cycles[i] = 0;
  }

  transferring = 0;
  bus_busy = 0;
  all_cores_exec = 0;
  one_core_exec = 0;
  core_measuring = 0;
  
#ifdef XPIPESBUILD
  xpipes_registered_nis = 0;
  xpipes_registered_buffers = 0;
  xpipes_registered_link_receivers = 0;
  
  for (uint i = 0; i < max_xpipes_registered_nis; i ++)
    xpipes_ni_status[i].registered = false;
    
  for (uint i = 0; i < max_xpipes_registered_buffers; i ++)
    xpipes_buffer_status[i].registered = false;
    
  for (uint i = 0; i < max_xpipes_registered_link_receivers; i ++)
    xpipes_link_receiver_status[i].registered = false;
    
  // Initialize th the platform overall packet statistics
  for (uint i = 0; i < 6; i ++)
  {
    xpipes_overall_min_ow_latency[i] = 5000;
    xpipes_overall_max_ow_latency[i] = 0;
    xpipes_overall_tot_ow_latency[i] = 0;
    xpipes_overall_min_rt_latency[i] = 5000;
    xpipes_overall_max_rt_latency[i] = 0;
    xpipes_overall_tot_rt_latency[i] = 0;
    xpipes_overall_sent_packets[i]   = 0;
  }
#endif
}

#ifdef AMBAAHBBUILD
///////////////////////////////////////////////////////////////////////////////
// inspectAMBAAHBbusAccess - Analyzes a bus access and the power spent.
void Statistics::inspectAMBAAHBAccess(uint bus_ID, AHB_bus_activity monitor)
{   
  if (status_global == IS_MEASURING)
  {
    power_object->buses_typ[bus_ID] += AMBAAHBEnergy(bus_ID, monitor, BUS_TYP);
    power_object->buses_min[bus_ID] += AMBAAHBEnergy(bus_ID, monitor, BUS_MIN);
    // The BUS_MAX call must be the last! See AMBAAHBEnergy()
    power_object->buses_max[bus_ID] += AMBAAHBEnergy(bus_ID, monitor, BUS_MAX);
  }
}
#endif

///////////////////////////////////////////////////////////////////////////////
// inspectAMBAAHBBridge - Analyzes a bridge access and the power spent.
void Statistics::inspectAMBAAHBBridge(uint bridge_ID, bool side, bool interact_fifo)
{
  if (status_global == IS_MEASURING)
    power_object->bridges[bridge_ID] += AMBAAHBBridgeEnergy(bridge_ID, side, interact_fifo);
}

///////////////////////////////////////////////////////////////////////////////
// inspectSTbusAccess - Analyzes a bus access and the power spent.
void Statistics::inspectSTbusAccess(double powertyp, double powermax, double powermin)
{
  // the STbus power model supplies a power, we have to accumulate the energy
  if (status_global == IS_MEASURING) 
  {
    // FIXME this is a hack: the ST power library (unnecessarily) asks for the period at boot,
    // while we could change it at runtime. So, use a fake divider even at runtime, matching
    // the one given during boot. (This code does not support multiple STBuses, or mixed buses!)
    power_object->buses_typ[0] += powertyp * CLOCKPERIOD * STBUS_DIVIDER_BOOT;
    power_object->buses_min[0] += powermin * CLOCKPERIOD * STBUS_DIVIDER_BOOT;
    power_object->buses_max[0] += powermax * CLOCKPERIOD * STBUS_DIVIDER_BOOT;
  }
}

///////////////////////////////////////////////////////////////////////////////
// inspectMemoryAccess - Analyzes a memory access and the power spent.
void Statistics::inspectMemoryAccess(uint32_t addr, bool reading, double energy, uint ID)
{
  /* Only take note of power consumption when accessing actual memory slaves */
  if(!addresser->IsPrivate(ID) && !addresser->IsLXPrivate(ID) && !addresser->IsShared(ID))
//  if (!addresser->IsPrivate(ID) && !addresser->IsShared(ID))
    return;

  if (status_global == IS_MEASURING)
    power_object->rams[ID] += energy;
}

///////////////////////////////////////////////////////////////////////////////
// inspectScratchAccess - Analyzes a scratch access done by the DMA or the CORESLAVE.
void Statistics::inspectextSCRATCHAccess(uint32_t addr, bool reading, uint ID)
{
  if (ID<N_CORES)
  {
   //DMA ACCESS
   if(reading)
    ext_scratchr[ID]++;
   else
    ext_scratchw[ID]++;
  }
  else if (addresser->IsCoreSlave(ID))
  {
   ID = ID - addresser->CoreSlaveStartID();
   ASSERT(ID<N_CORES && ID>=0);
   if(reading)
    ext_scratchr[ID]++;
   else
    ext_scratchw[ID]++;
  }
  else
   {
    printf("Fatal Error: received unexpected access to scratchpad %u neither by a DMA/core, nor by a core-associated slave!\n", ID);
    exit(1);
   }
}

///////////////////////////////////////////////////////////////////////////////
// inspectScratchAccess - Analyzes the power spent in a scratch access.
void Statistics::inspectSCRATCHAccess(uint32_t addr, bool reading, double power, uint ID)
{

  if (ID < N_CORES)
  {
    //CORE DMA ACCESS
    if (status_global == IS_MEASURING)
      power_object->scratches[ID] += power;
  }
  else if (addresser->IsCoreSlave(ID))
  {
    if (status_global == IS_MEASURING)
      power_object->scratches[ID-(N_CORES + N_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT)] += power; 
  }
  else
  {
    printf("Fatal Error: received unexpected access to scratchpad %u neither by a DMA/core, nor by a core-associated slave!\n", ID);
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// inspectIScratchAccess - Analyzes the power spent in an instruction scratch access.
void Statistics::inspectISCRATCHAccess(uint32_t addr, bool reading, double energy, uint ID)
{
  ASSERT(ID < N_CORES);

  if (status_global == IS_MEASURING)
    power_object->iscratches[ID] += energy;
}

///////////////////////////////////////////////////////////////////////////////
// inspectDMAAccess - Analyzes an access made by a DMA controller.
void Statistics::inspectDMAAccess(uint32_t addr, bool reading, uint ID)
{
  if (ACCTRACE)
    if (status[ID] == IS_MEASURING)  // Only when associated core is measuring
      fprintf(ftrace[ID+N_CORES], "%08x\t%d\t%d\n", addr & 0xFFFFFFFC, reading, !reading);
}

///////////////////////////////////////////////////////////////////////////////
// inspectDMAprogramAccess - Analyzes an access made to a DMA controller.
void Statistics::inspectDMAprogramAccess(uint32_t addr, bool reading, double energy, uint ID)
{
  if (ID > (uint)((DMA)*N_CORES + N_SMARTMEM))
  {
    printf("Error in the inspectDMA: %d\n",ID);
    exit(1);
  }
   
  if (status_global == IS_MEASURING)
    power_object->dmas[ID] += energy;
}

///////////////////////////////////////////////////////////////////////////////
// inspectSMARTMEMAccess - Analyzes an access made to a SMARTMEM controller.
void Statistics::inspectSMARTMEMAccess(uint32_t addr, bool reading, double energy, uint ID)
{
  uint which = ID - addresser->Smartmem_slave_StartID();
  ASSERT(which<N_SMARTMEM && which >=0);
 
  if (reading) 
  {
     ext_smartmemr[which]++;
     //printf("\n\n Smartread[%d]:%ld\n",which,ext_smartmemr[which]);
  }
  else 
  {
    ext_smartmemw[which]++;
    //printf("\n\n Smartwrite[%d]:%ld\n",which,ext_smartmemw[which]);
  }
  
  if (POWERSTATS && (status_global == IS_MEASURING))
    power_object->rams[ID] += energy;
}

///////////////////////////////////////////////////////////////////////////////
// inspectCacheAccess - Analyzes an access made into a SWARM's cache.
void Statistics::InspectCacheAccess(uint32_t addr, bool reading, CACHETYPE type, double energy, uint ID, uint ID2)
{
    if (SHARED_CACHE)
    {
      if (UCACHETYPE==SETASSOC)
        if (type==DIRECT)
        {
            /* This call is just for a line of the cache */
          return; /* discard: we are waiting for the full cache power */
        }
        else
        {
          ASSERT(ID2==0);
        }
      ASSERT(UCACHETYPE==type); /* Paranoid check? */
    }
    else
    {
      if (type==DIRECT) 
      {
        /* direct cache: is this just a line of a set associative cache? */
	/*   note that in this case ID and ID2 are nonsense, they aren't set */
	if ( (ICACHETYPE==DIRECT) && (ID2==0) )
	{
	  /* no: this is the instruction cache and it is direct */
	}
	else if ( (DCACHETYPE==DIRECT) && (ID2==1) )
	{
	  /* no: this is the data cache and it is direct */
	}
	else
	{
	    /* yes, this call is just for a line of the cache */
	  return; /* discard: we are waiting for the full cache power */
	}
      }
    }

    /* We are assuming that for a cycle there can be an access to the data */
    /* cache OR to the instr. cache, but not to both                       */
    if (status[ID] == IS_MEASURING) 
    {
      if (ID2 == 0)
        power_object->icaches[ID] += energy;
      else
        power_object->dcaches[ID] += energy;
    }
}


///////////////////////////////////////////////////////////////////////////////
// inspectSWARMAccess - Analyzes an access made by a SWARM processor.
void Statistics::inspectSWARMAccess(uint32_t addr, PPROC mode, bool hit, bool di, uint ID)
{
  short int slave;
  uint8_t rangeid;
  bool stall = false, is_ext_read = false, is_ext_write = false;
  
  //if (sc_simulation_time() - prev_time[ID] != 5.0 && sc_simulation_time() > 20.0)
  //{
  //  printf("Fatal error: stats call by ID %u at %10.1f, previous at %10.1f (expected period %3.1f)\n", ID, sc_simulation_time(), prev_time[ID], 5.0);
  //  exit(1);
  //}
  
  prev_time[ID] = sc_simulation_time();
 
  // Check to see if the ARM pipeline is stalled. Info is irrelevant for cores
  // blocked waiting for the bus to respond
  if (mode != P_BLOCKED && addr == prev_addr[ID] && mode == prev_mode[ID] && di == prev_di[ID] && mode == P_NORMAL)
    stall = true;                 // At least two consecutive accesses to the same address.
  prev_addr[ID] = addr;           // This gets interpreted as the result of a pipeline stall,
  prev_mode[ID] = mode;           // provided we're not doing a cache refill (which actually
  prev_di[ID] = di;               // is a stall too, but will be managed separately).
                                  // This assumption may not be fully accurate, but should
                                  // be good more than enough. It is very important to detect
                                  // stalls to properly count cache accesses (otherwise the
                                  // same access will be counted multiple times).
                                  // The checks on di, P_NORMAL are currently hacks to circumvent
                                  // cache splitting problems (same addresses going to both the D-
                                  // and I-Caches)!

  if (POWERSTATS && status[ID] == IS_MEASURING)
  {
    if (mode == P_BLOCKED)
      power_object->cores[ID] += ARMEnergy(ID, STALLED, M_DIVIDER[ID]);
    else if (mode == P_IDLE)
      power_object->cores[ID] += ARMEnergy(ID, POWER_IDLE, M_DIVIDER[ID]);
    else if (stall)
      power_object->cores[ID] += ARMEnergy(ID, STALLED, M_DIVIDER[ID]);
    else
      power_object->cores[ID] += ARMEnergy(ID, RUNNING, M_DIVIDER[ID]);
  }
  
  // for a core blocked waiting for the bus there is nothing else to do
  if (mode == P_BLOCKED)
  {
    w_cycles[ID] ++;
    return;
  }
  
  // for a core in IDLE state there is nothing else to do
  if (mode == P_IDLE)
  {
    core_c[ID].core_idle++;
    return;
  }

  switch (core_ct[ID])
  {
    case PREPARING_LINE_REFILL:
    case LINE_REFILL:
           core_c[ID].privaterwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case DCACHE_W:
    case ICACHE_W:
    case DCACHE_WT_MISS:
    case ICACHE_WT_MISS:
           core_c[ID].privatewwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_SHARED_R:
           core_c[ID].sharedrwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case SHARED_W:
           core_c[ID].sharedwwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_SEMAPHORE_R:
           core_c[ID].semaphorerwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case SEMAPHORE_W:
           core_c[ID].semaphorewwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case INTERRUPT_W:
           core_c[ID].interruptwwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_DMA_R:
           core_c[ID].dmarwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case DMA_W:
           core_c[ID].dmawwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_FFT_R:
           core_c[ID].fftrwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case FFT_W:
           core_c[ID].fftwwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_FREQ_R:
           core_c[ID].freqrwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case FREQ_W:
           core_c[ID].freqwwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_CORESLAVE_R:
           core_c[ID].coreslaverwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case CORESLAVE_W:
           core_c[ID].coreslavewwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case PREPARING_SMARTMEM_R:
           core_c[ID].smartmemrwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    case SMARTMEM_W:
           core_c[ID].smartmemwwait += w_cycles[ID];
           core_c[ID].waiting += w_cycles[ID];
           break;
    default:
           // It is ok to fall inside of here, just don't do anything
           break;
  }

  core_ct[ID] = NONE;
  w_cycles[ID] = 0;

  if (status[ID] == IS_MEASURING && !stall)
  {
    core_c[ID].counter++;
    
    switch (mode)
    {
      case P_NORMAL:
      {
        if (addresser->PhysicalInInternalSpace(addr) || addresser->PhysicalInSimSupportSpace(addr))
          core_ct[ID] = INTERNAL_R;
        else if (addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr, ID), &rangeid))
          core_ct[ID] = SCRATCH_R;
        else if (addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(addr, ID)))
          core_ct[ID] = ISCRATCH_R;
        else if (addresser->PhysicalInQueueSpace(ID, addresser->Logical2Physical(addr, ID)))
          core_ct[ID] = QUEUE_R;  
        else if (!addresser->LogicalIsCacheable(addr))
        {
          // MapPhysicalToSlave may return -1 to specify DMA or scratchpad accesses.
          // The scratchpad case was managed above, now only check for DMA
          slave = addresser->MapPhysicalToSlave(addresser->Logical2Physical(addr, ID));
          if (addresser->IsShared(slave))
            core_ct[ID] = SHARED_R;
          else if (addresser->IsSemaphore(slave))
            core_ct[ID] = SEMAPHORE_R;
          else if (addresser->PhysicalInDMASpace(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = DMA_R;
          else if (addresser->PhysicalInFFTSpace(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = FFT_R;
          else if (addresser->PhysicalInFreqSpace(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = FREQ_R;
          else if (addresser->PhysicalInCoreSlaveSpace(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = CORESLAVE_R;
          else if (addresser->PhysicalInSmartmem(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = SMARTMEM_R;
          else
          {
            printf("Fatal error: read access at address 0x%08x cannot be inspected!\n", addr);
            exit(1);
          }
        }
        else if (hit)
        {
          if (di)
            core_ct[ID] = DCACHE_HIT;
          else
            core_ct[ID] = ICACHE_HIT;
        }
        else
          if (di)
            core_ct[ID] = DCACHE_MISS;
          else
            core_ct[ID] = ICACHE_MISS;
      }
      break;

      case P_READING1:
      {
        if (addresser->LogicalIsCacheable(addr))
          core_ct[ID] = PREPARING_LINE_REFILL;
        else
        if (addresser->PhysicalInSimSupportSpace(addr))
          core_ct[ID] = INTERNAL_R;
        else
        {
          // MapPhysicalToSlave may return -1 to specify DMA or scratchpad accesses.
          // Managing here only the DMA case, as a scratchpad access would be illegal
          // in P_READING1
          slave = addresser->MapPhysicalToSlave(addresser->Logical2Physical(addr, ID));
          if (addresser->IsShared(slave))
            core_ct[ID] = PREPARING_SHARED_R;
          else if (addresser->IsSemaphore(slave))
            core_ct[ID] = PREPARING_SEMAPHORE_R;
          else if (addresser->IsCoreSlave(slave))
            core_ct[ID] = PREPARING_CORESLAVE_R;
          else if (addresser->IsSmartmem(slave))
            core_ct[ID] = PREPARING_SMARTMEM_R;
          else if (addresser->IsFFT(slave))
            core_ct[ID] = PREPARING_FFT_R; 
          else if (addresser->IsFreq(slave))
            core_ct[ID] = PREPARING_FREQ_R; 
          else if (addresser->PhysicalInDMASpace(addresser->Logical2Physical(addr, ID)))
            core_ct[ID] = PREPARING_DMA_R;
          else
          {
            printf("Fatal error: read access at address 0x%08x cannot be inspected!\n", addr);
            exit(1);
          }
        }
      }
      break;

      case P_READING:
      {
        core_ct[ID] = LINE_REFILL;
      }
      break;

      case P_WRITING1:
      {
        if (addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr, ID), &rangeid))
          core_ct[ID] = SCRATCH_W;
        else if (addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(addr, ID)))
          core_ct[ID] = ISCRATCH_W;
        else if (addresser->PhysicalInQueueSpace(ID, addresser->Logical2Physical(addr, ID)))
          core_ct[ID] = QUEUE_W;
        else if (addresser->PhysicalInSimSupportSpace(addr))
          core_ct[ID] = INTERNAL_W;
        else if (addresser->LogicalIsCacheable(addr))
	{
            if (hit)
            {
             if (di)
               core_ct[ID] = DCACHE_W;
             else
               core_ct[ID] = ICACHE_W;        // FIXME this is an error: assert() here instead of checking stats later?
            }
            else
              if (di)
                core_ct[ID] = DCACHE_WT_MISS;
              else
                core_ct[ID] = ICACHE_WT_MISS;  // FIXME this is an error: assert() here instead of checking stats later?
        }
        else
        {
            // MapPhysicalToSlave may return -1 to specify DMA or scratchpad accesses.
            // The scratchpad case was managed above, now only check for DMA
            slave = addresser->MapPhysicalToSlave(addresser->Logical2Physical(addr, ID));
            if (addresser->IsShared(slave))
              core_ct[ID] = SHARED_W;
            else if (addresser->IsSemaphore(slave))
              core_ct[ID] = SEMAPHORE_W;
            else if (addresser->IsInterrupt(slave))
              core_ct[ID] = INTERRUPT_W;
            else if (addresser->IsCoreSlave(slave))
              core_ct[ID] = CORESLAVE_W;
            else if (addresser->IsSmartmem(slave))
              core_ct[ID] = SMARTMEM_W;
            else if (addresser->IsFFT(slave))
              core_ct[ID] = FFT_W;
            else if (addresser->IsFreq(slave))
              core_ct[ID] = FREQ_W;
            else if (addresser->PhysicalInDMASpace(addresser->Logical2Physical(addr, ID)))
              core_ct[ID] = DMA_W;
            else
            {
              printf("Fatal error: write access at address 0x%08x cannot be inspected!\n", addr);
              exit(1);
            }
        }
      }
      break;

      case P_WRITING_DIRTY_LINE:
      {
        ASSERT (addresser->LogicalIsCacheable(addr));
        core_ct[ID] = DCACHE_WT_MISS;
      }
      break;

      case P_INTWRITE:
      {
        core_ct[ID] = INTERNAL_W;
      }
      break;

      default:
      ASSERT(0); // It must not happen
      break;
    } //switch (mode)
    

    switch(core_ct[ID])
    {
      case SHARED_R:
             core_c[ID].sharedr++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case SHARED_W:
             core_c[ID].sharedw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case SEMAPHORE_R:
             core_c[ID].semaphorer++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case SEMAPHORE_W:
             core_c[ID].semaphorew++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case INTERRUPT_W:
             core_c[ID].intw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case SCRATCH_R:
             core_c[ID].scratchr++;
             is_ext_read = true;
             break;
      case SCRATCH_W:
             core_c[ID].scratchw++;
             is_ext_write = true;
             break;
      case ISCRATCH_R:
             core_c[ID].iscratchr++;
             is_ext_read = true;
             break;
      case ISCRATCH_W:
             core_c[ID].iscratchw++;
             is_ext_write = true;
             break;
      case QUEUE_R:
             core_c[ID].queuer++;
             is_ext_read = true;
             break;
      case QUEUE_W:
             core_c[ID].queuew++;
             is_ext_write = true;
             break;
      case DMA_R:
             core_c[ID].dmar++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case DMA_W:
             core_c[ID].dmaw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case FFT_R:
             core_c[ID].fftr++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case FFT_W:
             core_c[ID].fftw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case FREQ_R:
             core_c[ID].freqr++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case FREQ_W:
             core_c[ID].freqw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case INTERNAL_R:
             core_c[ID].internal++;
             core_c[ID].internalr++;
             break;
      case INTERNAL_W:
             core_c[ID].internal++;
             core_c[ID].internalw++;
             break;
      case CORESLAVE_R:
             core_c[ID].coreslaver++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case CORESLAVE_W:
             core_c[ID].coreslavew++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case SMARTMEM_R:
             core_c[ID].smartmemr++;
             core_c[ID].non_cache_cyc++;
             is_ext_read = true;
             break;
      case SMARTMEM_W:
             core_c[ID].smartmemw++;
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             is_ext_write = true;
             break;
      case PREPARING_LINE_REFILL:
      case LINE_REFILL:
             core_c[ID].refill++;
             break;
      case DCACHE_MISS:
             core_c[ID].dc_miss++;
             break;
      case DCACHE_HIT:
             core_c[ID].privater++;
             is_ext_read = true;
             core_c[ID].dc_hit++;
             break;
      case ICACHE_MISS:
             core_c[ID].ic_miss++;
             break;
      case ICACHE_HIT:
             core_c[ID].privater++;
             is_ext_read = true;
             core_c[ID].ic_hit++;
             break;
      case DCACHE_W:
             core_c[ID].privatew++;
             is_ext_write = true;
             core_c[ID].dc_w++;
             break;
      case ICACHE_W:
             core_c[ID].privatew++;
             is_ext_write = true;
             core_c[ID].ic_w++;
             break;
      case DCACHE_WT_MISS:
             core_c[ID].privatew++;
             is_ext_write = true;
             core_c[ID].dc_wm++;
             break;
      case ICACHE_WT_MISS:
             core_c[ID].privatew++;
             is_ext_write = true;
             core_c[ID].ic_wm++;
             break;
      case PREPARING_SHARED_R:
      case PREPARING_SEMAPHORE_R:
      case PREPARING_DMA_R:
      case PREPARING_FFT_R:
      case PREPARING_FREQ_R:
      case PREPARING_CORESLAVE_R:
      case PREPARING_SMARTMEM_R:
             core_c[ID].non_cache_cyc++;
             core_c[ID].non_cache_acc++;
             break;
      default:
      ASSERT(0); // It must not happen
    } //switch (core_ct[ID])

    // Access to a partitioned scratchpad memory. In this case, trace specific ranges inside of
    // the scratchpad too
    if (SPCHECK && (core_c[ID].scratchrangenumber > 0) && (core_ct[ID] == SCRATCH_R))
      core_c[ID].scratchrrange[rangeid]++;
    if (SPCHECK && (core_c[ID].scratchrangenumber > 0) && (core_ct[ID] == SCRATCH_W))
      core_c[ID].scratchwrange[rangeid]++;
    
    // If in SPCHECK mode, traces are only interesting if referring to private/scratch space
    if (ACCTRACE && status[ID] == IS_MEASURING/* && !di*/)
      if (
           !SPCHECK && (is_ext_read || is_ext_write) ||
           core_ct[ID] == DCACHE_HIT ||
           core_ct[ID] == ICACHE_HIT ||
           core_ct[ID] == DCACHE_W ||
           core_ct[ID] == ICACHE_W ||
           core_ct[ID] == DCACHE_WT_MISS ||
           core_ct[ID] == ICACHE_WT_MISS ||
           core_ct[ID] == SCRATCH_R ||
           core_ct[ID] == SCRATCH_W ||
           core_ct[ID] == ISCRATCH_R ||
           core_ct[ID] == ISCRATCH_W
         )
        fprintf(ftrace[ID], "%08x\t%d\t%d\n", addr & 0xFFFFFFFC, is_ext_read, is_ext_write);
  }
  else
    if (status[ID] == IS_MEASURING)
    {
      core_c[ID].stalled++;
      core_c[ID].counter++;
    }
}

#ifdef LXBUILD
void Statistics::inspectTGENaccess(bool rdwr, int maxfifo)
{
  if (rdwr)
    tgen_read_max_fifo = maxfifo;
  else
    tgen_write_max_fifo = maxfifo;
}
#endif


///////////////////////////////////////////////////////////////////////////////
// startMeasuring - Starts collecting statistics for a processor (and associated
//                  DMA controller if present).
void Statistics::startMeasuring(uint ID)
{
  if (status_global == READY_TO_MEASURE)
    global_start_time_crit = sc_simulation_time();
  status_global = IS_MEASURING; // The first start swi starts the measures for global objects
  if (status[ID] == READY_TO_MEASURE)
  {
    status[ID] = IS_MEASURING;
    core_measuring++;
    
    start_sim_time_crit[ID] = time(NULL);
    start_time_crit[ID] = sc_simulation_time();
    
    for (uint k=0; k<ID; k++)
      printf("\t");
    printf("Processor %hu starts measuring\n", (uint)ID);
  }
  else if (status[ID] == IS_MEASURING && AUTOSTARTMEASURING)
  {
    // Do nothing, silently accept this case
  }
  else
  {
      /* A core can continue to run after the "swi stop". If we are measuring the 
         OS calls, they can invoke "swi start" */
    if (status[ID] == READY_TO_SHUTDOWN) return; /* simply ignore it */
    printf("Fatal error: Processor %hu sent an unexpected \"Start measure\" signal\n", (uint)ID);
    exit(1);
  }
}


///////////////////////////////////////////////////////////////////////////////
// stopMeasuring - Stops collecting statistics for a processor (and associated
//                 DMA controller if present).
void Statistics::stopMeasuring(uint ID)
{
  if (status_global == IS_MEASURING)
    global_total_time_crit += 
      sc_simulation_time() - global_start_time_crit;
  
  status_global = READY_TO_MEASURE; // The first stop swi stops the measures for global objects

  if (status[ID] == IS_MEASURING)
  {
    status[ID] = READY_TO_MEASURE;
    core_measuring --;
    
    core_c[ID].total_sim_time_crit += time(NULL) - start_sim_time_crit[ID];
    core_c[ID].total_time_crit += sc_simulation_time() - start_time_crit[ID];

    for (uint k=0; k<ID; k++)
      printf("\t");
    printf("Processor %hu stops measuring\n", (uint)ID);
  }
  else
  {
    if (status[ID] == READY_TO_SHUTDOWN) return; /* simply ignore it */
    printf("Fatal error: Processor %hu sent an unexpected \"Stop measure\" signal\n", (uint)ID);
    exit(1);
  }
};


///////////////////////////////////////////////////////////////////////////////
// quit - Marks a processor as ready to quit the simulation. When all cores
//        will have called this function, the simulation will be terminated.
void Statistics::quit(uint ID)
{
    /* If this core is measuring, continue to measure, but remember that it 
       finished */
  if ( (status[ID] == READY_TO_MEASURE) || (status[ID] == IS_MEASURING) )
  {
    if (status[ID] == READY_TO_MEASURE)
      status[ID] = READY_TO_SHUTDOWN;
    readytoterm++;
  
    for (uint k=0; k<ID; k++)
      printf("\t");
    printf("Processor %hu shuts down\n", (uint)ID);
    if (readytoterm == N_CORES)
    {
      printf("\n\nSimulation ended\n");
      dumpEverything();
      sc_stop();
      printf("\n\n");
    }
  }
  else
  {
    printf("Fatal error: Processor %hu sent an unexpected \"Quit\" signal\n", (uint)ID);
    exit(1);
  }
};


///////////////////////////////////////////////////////////////////////////////
// requestsAccess - Marks the time when a bus master requested a bus access.
void Statistics::requestsAccess(uint ID)
{
  master_s[ID].request_access = sc_simulation_time();
};


///////////////////////////////////////////////////////////////////////////////
// beginsAccess - Marks the time when a bus master was actually granted bus access.
void Statistics::beginsAccess(uint8_t min, bool reading, uint8_t burst, uint ID)
{
  uint transl_ID = (ID < N_CORES ? ID : ID - N_CORES);
  uint32_t wait_time;
  
  master_s[ID].start_access = sc_simulation_time();
  wait_time = (uint32_t)(master_s[ID].start_access - master_s[ID].request_access);
  
  master_s[ID].is_accessing = true;
  master_s[ID].data_on_bus = burst;

  if (status[transl_ID] == IS_MEASURING)
  {
    if (wait_time == min)
      master_c[ID].free ++;

    master_c[ID].accesses ++;
    master_c[ID].tot_waittime += wait_time;
    if (wait_time > master_c[ID].max_waittime)
      master_c[ID].max_waittime = wait_time;
    if (wait_time < master_c[ID].min_waittime)
      master_c[ID].min_waittime = wait_time;

    if (reading)
    {
      master_c[ID].accesses_r ++;
      master_c[ID].tot_waittime_r += wait_time;
      if (wait_time > master_c[ID].max_waittime_r)
        master_c[ID].max_waittime_r = wait_time;
      if (wait_time < master_c[ID].min_waittime_r)
        master_c[ID].min_waittime_r = wait_time;
    }
    if (!reading)
    {
      master_c[ID].accesses_w ++;
      master_c[ID].tot_waittime_w += wait_time;
      if (wait_time > master_c[ID].max_waittime_w)
        master_c[ID].max_waittime_w = wait_time;
      if (wait_time < master_c[ID].min_waittime_w)
        master_c[ID].min_waittime_w = wait_time;
    }
    if (reading && burst == 1)
    {
      master_c[ID].accesses_sr ++;
      master_c[ID].tot_waittime_sr += wait_time;
      if (wait_time > master_c[ID].max_waittime_sr)
        master_c[ID].max_waittime_sr = wait_time;
      if (wait_time < master_c[ID].min_waittime_sr)
        master_c[ID].min_waittime_sr = wait_time;
    }
    if (!reading && burst == 1)
    {
      master_c[ID].accesses_sw ++;
      master_c[ID].tot_waittime_sw += wait_time;
      if (wait_time > master_c[ID].max_waittime_sw)
        master_c[ID].max_waittime_sw = wait_time;
      if (wait_time < master_c[ID].min_waittime_sw)
        master_c[ID].min_waittime_sw = wait_time;
    }
    if (reading && burst > 1)
    {
      master_c[ID].accesses_br ++;
      master_c[ID].tot_waittime_br += wait_time;
      if (wait_time > master_c[ID].max_waittime_br)
        master_c[ID].max_waittime_br = wait_time;
      if (wait_time < master_c[ID].min_waittime_br)
        master_c[ID].min_waittime_br = wait_time;
    }
    if (!reading && burst > 1)
    {
      master_c[ID].accesses_bw ++;
      master_c[ID].tot_waittime_bw += wait_time;
      if (wait_time > master_c[ID].max_waittime_bw)
        master_c[ID].max_waittime_bw = wait_time;
      if (wait_time < master_c[ID].min_waittime_bw)
        master_c[ID].min_waittime_bw = wait_time;
    }
  }
};
    
    
///////////////////////////////////////////////////////////////////////////////
// endsAccess - Marks the time when a bus master completed a bus access.
void Statistics::endsAccess(bool reading, uint8_t burst, uint ID)
{
  uint transl_ID = (ID < N_CORES ? ID : ID - N_CORES);
  uint32_t comp_time;
  
  master_s[ID].finish_access = sc_simulation_time();
  comp_time = (uint32_t)(master_s[ID].finish_access - master_s[ID].request_access);
  
  if (status[transl_ID] == IS_MEASURING)
  {
    master_c[ID].tot_comptime += comp_time;
    if (comp_time > master_c[ID].max_comptime)
      master_c[ID].max_comptime = comp_time;
    if (comp_time < master_c[ID].min_comptime)
      master_c[ID].min_comptime = comp_time;

    if (reading)
    {
      master_c[ID].tot_comptime_r += comp_time;
      if (comp_time > master_c[ID].max_comptime_r)
        master_c[ID].max_comptime_r = comp_time;
      if (comp_time < master_c[ID].min_comptime_r)
        master_c[ID].min_comptime_r = comp_time;
    }
    if (!reading)
    {
      master_c[ID].tot_comptime_w += comp_time;
      if (comp_time > master_c[ID].max_comptime_w)
        master_c[ID].max_comptime_w = comp_time;
      if (comp_time < master_c[ID].min_comptime_w)
        master_c[ID].min_comptime_w = comp_time;
    }
    if (reading && burst == 1)
    {
      master_c[ID].tot_comptime_sr += comp_time;
      if (comp_time > master_c[ID].max_comptime_sr)
        master_c[ID].max_comptime_sr = comp_time;
      if (comp_time < master_c[ID].min_comptime_sr)
        master_c[ID].min_comptime_sr = comp_time;
    }
    if (!reading && burst == 1)
    {
      master_c[ID].tot_comptime_sw += comp_time;
      if (comp_time > master_c[ID].max_comptime_sw)
        master_c[ID].max_comptime_sw = comp_time;
      if (comp_time < master_c[ID].min_comptime_sw)
        master_c[ID].min_comptime_sw = comp_time;
    }
    if (reading && burst > 1)
    {
      master_c[ID].tot_comptime_br += comp_time;
      if (comp_time > master_c[ID].max_comptime_br)
        master_c[ID].max_comptime_br = comp_time;
      if (comp_time < master_c[ID].min_comptime_br)
        master_c[ID].min_comptime_br = comp_time;
    }
    if (!reading && burst > 1)
    {
      master_c[ID].tot_comptime_bw += comp_time;
      if (comp_time > master_c[ID].max_comptime_bw)
        master_c[ID].max_comptime_bw = comp_time;
      if (comp_time < master_c[ID].min_comptime_bw)
        master_c[ID].min_comptime_bw = comp_time;
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
// busFreed - Marks the time when a bus master freed the bus after an access.
void Statistics::busFreed(uint ID)
{
  master_s[ID].is_accessing = false;
}


///////////////////////////////////////////////////////////////////////////////
// putOCPTransactionCommand - An OCP master is asserting a non-idle MCmd.
void Statistics::putOCPTransactionCommand(uint8_t cmd, uint8_t burst, uint ID)
{
  master_ocp_s[ID].mcmd_asserted = sc_simulation_time();
}


///////////////////////////////////////////////////////////////////////////////
// getOCPTransactionCommandAccept - An OCP master is sampling SCmdAccept.
void Statistics::getOCPTransactionCommandAccept(uint8_t cmd, uint8_t burst, uint ID)
{
  uint transl_ID = (ID < N_CORES ? ID : ID - N_CORES);
  uint32_t cmdacc_time;
  
  master_ocp_s[ID].scmdaccept_sampled = sc_simulation_time();
  cmdacc_time = (uint32_t)(sc_simulation_time() - master_ocp_s[ID].mcmd_asserted);

  if (status[transl_ID] == IS_MEASURING)
  {
    master_ocp_c[ID].accesses ++;
    master_ocp_c[ID].tot_cmdacctime += cmdacc_time;
    if (cmdacc_time > master_ocp_c[ID].max_cmdacctime)
      master_ocp_c[ID].max_cmdacctime = cmdacc_time;
    if (cmdacc_time < master_ocp_c[ID].min_cmdacctime)
      master_ocp_c[ID].min_cmdacctime = cmdacc_time;
    
    switch (cmd)
    {
      case OCPCMDREAD:
            master_ocp_c[ID].accesses_r ++;
            master_ocp_c[ID].tot_cmdacctime_r += cmdacc_time;
            if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_r)
              master_ocp_c[ID].max_cmdacctime_r = cmdacc_time;
            if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_r)
              master_ocp_c[ID].min_cmdacctime_r = cmdacc_time;
            if (burst == 1)
            {
              master_ocp_c[ID].accesses_sr ++;
              master_ocp_c[ID].tot_cmdacctime_sr += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_sr)
                master_ocp_c[ID].max_cmdacctime_sr = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_sr)
                master_ocp_c[ID].min_cmdacctime_sr = cmdacc_time;
            }
            else
            {
              master_ocp_c[ID].accesses_br ++;
              master_ocp_c[ID].tot_cmdacctime_br += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_br)
                master_ocp_c[ID].max_cmdacctime_br = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_br)
                master_ocp_c[ID].min_cmdacctime_br = cmdacc_time;
            }
            break;
      case OCPCMDWRITE:
            master_ocp_c[ID].accesses_wp ++;
            master_ocp_c[ID].tot_cmdacctime_wp += cmdacc_time;
            if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_wp)
              master_ocp_c[ID].max_cmdacctime_wp = cmdacc_time;
            if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_wp)
              master_ocp_c[ID].min_cmdacctime_wp = cmdacc_time;
            if (burst == 1)
            {
              master_ocp_c[ID].accesses_swp ++;
              master_ocp_c[ID].tot_cmdacctime_swp += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_swp)
                master_ocp_c[ID].max_cmdacctime_swp = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_swp)
                master_ocp_c[ID].min_cmdacctime_swp = cmdacc_time;
            }
            else
            {
              master_ocp_c[ID].accesses_bwp ++;
              master_ocp_c[ID].tot_cmdacctime_bwp += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_bwp)
                master_ocp_c[ID].max_cmdacctime_bwp = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_bwp)
                master_ocp_c[ID].min_cmdacctime_bwp = cmdacc_time;
            }
            break;
      case OCPCMDWRNP:
            master_ocp_c[ID].accesses_wnp ++;
            master_ocp_c[ID].tot_cmdacctime_wnp += cmdacc_time;
            if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_wnp)
              master_ocp_c[ID].max_cmdacctime_wnp = cmdacc_time;
            if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_wnp)
              master_ocp_c[ID].min_cmdacctime_wnp = cmdacc_time;
            if (burst == 1)
            {
              master_ocp_c[ID].accesses_swnp ++;
              master_ocp_c[ID].tot_cmdacctime_swnp += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_swnp)
                master_ocp_c[ID].max_cmdacctime_swnp = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_swnp)
                master_ocp_c[ID].min_cmdacctime_swnp = cmdacc_time;
            }
            else
            {
              master_ocp_c[ID].accesses_bwnp ++;
              master_ocp_c[ID].tot_cmdacctime_bwnp += cmdacc_time;
              if (cmdacc_time > master_ocp_c[ID].max_cmdacctime_bwnp)
                master_ocp_c[ID].max_cmdacctime_bwnp = cmdacc_time;
              if (cmdacc_time < master_ocp_c[ID].min_cmdacctime_bwnp)
                master_ocp_c[ID].min_cmdacctime_bwnp = cmdacc_time;
            }
            break;
      default:
            printf("Fatal error: OCP command %hu by processor %hu can not be decoded!\n", cmd, ID);
            exit(1);
            break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// getOCPTransactionResp - An OCP master is sampling SRespLast.
void Statistics::getOCPTransactionResp(uint8_t cmd, uint8_t burst, uint ID)
{
  uint transl_ID = (ID < N_CORES ? ID : ID - N_CORES);
  uint32_t comp_time;
  
  master_ocp_s[ID].sresplast_sampled = sc_simulation_time();
  comp_time = (uint32_t)(sc_simulation_time() - master_ocp_s[ID].mcmd_asserted);

  if (status[transl_ID] == IS_MEASURING)
  {
    master_ocp_c[ID].tot_comptime += comp_time;
    if (comp_time > master_ocp_c[ID].max_comptime)
      master_ocp_c[ID].max_comptime = comp_time;
    if (comp_time < master_ocp_c[ID].min_comptime)
      master_ocp_c[ID].min_comptime = comp_time;
    
    switch (cmd)
    {
      case OCPCMDREAD:
            master_ocp_c[ID].tot_comptime_r += comp_time;
            if (comp_time > master_ocp_c[ID].max_comptime_r)
              master_ocp_c[ID].max_comptime_r = comp_time;
            if (comp_time < master_ocp_c[ID].min_comptime_r)
              master_ocp_c[ID].min_comptime_r = comp_time;
            if (burst == 1)
            {
              master_ocp_c[ID].tot_comptime_sr += comp_time;
              if (comp_time > master_ocp_c[ID].max_comptime_sr)
                master_ocp_c[ID].max_comptime_sr = comp_time;
              if (comp_time < master_ocp_c[ID].min_comptime_sr)
                master_ocp_c[ID].min_comptime_sr = comp_time;
            }
            else
            {
              master_ocp_c[ID].tot_comptime_br += comp_time;
              if (comp_time > master_ocp_c[ID].max_comptime_br)
                master_ocp_c[ID].max_comptime_br = comp_time;
              if (comp_time < master_ocp_c[ID].min_comptime_br)
                master_ocp_c[ID].min_comptime_br = comp_time;
            }
            break;
      case OCPCMDWRNP:
            master_ocp_c[ID].tot_comptime_wnp += comp_time;
            if (comp_time > master_ocp_c[ID].max_comptime_wnp)
              master_ocp_c[ID].max_comptime_wnp = comp_time;
            if (comp_time < master_ocp_c[ID].min_comptime_wnp)
              master_ocp_c[ID].min_comptime_wnp = comp_time;
            if (burst == 1)
            {
              master_ocp_c[ID].tot_comptime_swnp += comp_time;
              if (comp_time > master_ocp_c[ID].max_comptime_swnp)
                master_ocp_c[ID].max_comptime_swnp = comp_time;
              if (comp_time < master_ocp_c[ID].min_comptime_swnp)
                master_ocp_c[ID].min_comptime_swnp = comp_time;
            }
            else
            {
              master_ocp_c[ID].tot_comptime_bwnp += comp_time;
              if (comp_time > master_ocp_c[ID].max_comptime_bwnp)
                master_ocp_c[ID].max_comptime_bwnp = comp_time;
              if (comp_time < master_ocp_c[ID].min_comptime_bwnp)
                master_ocp_c[ID].min_comptime_bwnp = comp_time;
            }
            break;
      default:
            printf("Fatal error: OCP command %hu by processor %hu can not be decoded!\n", cmd, ID);
            exit(1);
            break;
    }
  }
}


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// register_xpipes_ni - Assigns a unique ID to a xpipes NI.
unsigned short int Statistics::register_xpipes_ni(sc_string name,
                                                  unsigned int ID,
                                                  int source,
                                                  bool is_target_ni,
                                                  bool is_initiator_ni)
{
  // Check to see if this is "the other half" (e.g. ni_response vs ni_request)
  // of an already registered NI, and in that case, return the same ID as for
  // that module.
  // Pay attention: only ni_request and ni_resend modules know their "source"
  // register, while ni_response and ni_receive will pass a dummy value of -1.
  // So, record it only if >= 0 (i.e. if we're registering the half NI which
  // knows the actual value)
  for (uint i = 0; i < xpipes_registered_nis; i ++)
  {
    if
      (
        (
          (xpipes_ni_status[i].is_target_ni && is_target_ni && !is_initiator_ni) ||
          (xpipes_ni_status[i].is_initiator_ni && !is_target_ni && is_initiator_ni) ||
          (xpipes_ni_status[i].is_target_ni && is_target_ni && is_initiator_ni)
        ) &&
        xpipes_ni_status[i].ID == ID
      )
    {
      // The missing half of an already registered NI, the one having "source" visibility
      if ((!is_target_ni || !is_initiator_ni) && source >= 0)
      {
        xpipes_ni_status[i].source = (unsigned int)source;
        /*printf("Associating ID %2u to the NI having internal ID %2u, source %3d (0x%04x), type %s (%s call)\n",
          i, ID, source, (unsigned int)source, is_target_ni ? "target" : "initiator", is_target_ni ? "ni_resend" : "ni_request");*/
      }
      // The missing half of an already registered NI, the one without "source" visibility
      else if ((!is_target_ni || !is_initiator_ni) && source < 0)
      {
        /*printf("Associating ID %2u to the NI having internal ID %2u, source %3d (0x%04x), type %s (%s call)\n",
          i, ID, source, (unsigned int)source, is_target_ni ? "target" : "initiator", is_target_ni ? "ni_receive" : "ni_response");*/
      }
      // An extra channel of an initiator/target NI
      else if (is_target_ni && is_initiator_ni)
      {
        // FIXME the code will only really work if the redundancy channel call arrives after the ID is already given
        // FIXME use proper naming for future extensions (such as fully dual NIs or interrupt-enabled NIs)
        xpipes_ni_status[i].is_target_ni    = true;
        xpipes_ni_status[i].is_initiator_ni = true;
        xpipes_ni_status[i].shadow          = source;
        /*printf("Associating ID %2u to the NI having internal ID %2u, source %3d (0x%04x), type extra_initiator (redundancy channel call)\n",
          i, ID, source, (unsigned int)source);*/
      }
      // Error
      else
      {
        printf("Fatal Error when registering NIs: unknown registration data provided!\n");
        exit(1);
      }
      return i;
    }
  }

  if (xpipes_registered_nis >= max_xpipes_registered_nis)
  {
    fprintf(stderr, "Fatal Error: too many xpipes NIs trying to register (current = %hu, max = %hu)\n",
      xpipes_registered_nis + 1, max_xpipes_registered_nis);
    exit(1);
  }

  xpipes_ni_status[xpipes_registered_nis].registered          = true;
  xpipes_ni_status[xpipes_registered_nis].name                = name;
  xpipes_ni_status[xpipes_registered_nis].ID                  = ID;
  // Pay attention: only ni_request and ni_resend modules know their "source"
  // register, while ni_response and ni_receive will pass a dummy value of -1.
  // So, record it only if > 0 (i.e. if we're registering the half NI which
  // knows the actual value)
  if (source >= 0)
    xpipes_ni_status[xpipes_registered_nis].source            = (unsigned int)source;
  xpipes_ni_status[xpipes_registered_nis].is_target_ni        = is_target_ni;
  xpipes_ni_status[xpipes_registered_nis].is_initiator_ni     = is_initiator_ni;
  xpipes_ni_status[xpipes_registered_nis].shadow              = 0xffffffff;
  
  /*printf("Associating ID %2u to the NI having internal ID %2u, source %3d (0x%04x), type %s (%s call)\n",
    xpipes_registered_nis, ID, source, (unsigned int)source,
    (is_target_ni && is_initiator_ni) ? "extra_initiator" :
    (is_target_ni ? "target" : "initiator"),
    (is_target_ni && is_initiator_ni) ? "redundancy channel" :
    (is_target_ni ? (source == -1 ? "ni_receive" : "ni_resend") :
    (source == -1 ? "ni_response" : "ni_request")));*/
  
  // Initialize this NI's packet statistics per target and per packet
  // type: SR (single read),
  //       SWP (single write - posted),
  //       SWNP (single write - non posted),
  //       BR (burst read),
  //       BWP (burst write - posted),
  //       BWNP (burst write - non posted)
  for (uint i = 0; i < max_xpipes_registered_nis; i ++)
  {
    for (uint j = 0; j < 6; j ++)
    {
      xpipes_ni_status[xpipes_registered_nis].min_ow_latency[i][j] = 5000;
      xpipes_ni_status[xpipes_registered_nis].max_ow_latency[i][j] = 0;
      xpipes_ni_status[xpipes_registered_nis].tot_ow_latency[i][j] = 0;
      xpipes_ni_status[xpipes_registered_nis].min_rt_latency[i][j] = 5000;
      xpipes_ni_status[xpipes_registered_nis].max_rt_latency[i][j] = 0;
      xpipes_ni_status[xpipes_registered_nis].tot_rt_latency[i][j] = 0;
      xpipes_ni_status[xpipes_registered_nis].sent_packets[i][j]   = 0;
    }
  }
  // Initialize this NI's overall packet statistics per packet type
  for (uint i = 0; i < 6; i ++)
  {
    xpipes_ni_status[xpipes_registered_nis].overall_min_ow_latency[i] = 5000;
    xpipes_ni_status[xpipes_registered_nis].overall_max_ow_latency[i] = 0;
    xpipes_ni_status[xpipes_registered_nis].overall_tot_ow_latency[i] = 0;
    xpipes_ni_status[xpipes_registered_nis].overall_min_rt_latency[i] = 5000;
    xpipes_ni_status[xpipes_registered_nis].overall_max_rt_latency[i] = 0;
    xpipes_ni_status[xpipes_registered_nis].overall_tot_rt_latency[i] = 0;
    xpipes_ni_status[xpipes_registered_nis].overall_sent_packets[i]   = 0;
  }
  
  for (uint i = 0; i < MAX_IN_FLIGHT_PACKETS + 1; i ++)
    xpipes_ni_status[xpipes_registered_nis].in_flight_packets[i].flying = false;
  
  return (++ xpipes_registered_nis - 1);
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// register_xpipes_buffer - Assigns a unique ID to a xpipes output buffer.
unsigned short int Statistics::register_xpipes_buffer(sc_string name,
                                                      unsigned short int locations)
{
  if (xpipes_registered_buffers >= max_xpipes_registered_buffers)
  {
    fprintf(stderr, "Fatal Error: too many xpipes buffers trying to register (current = %hu, max = %hu)\n",
      xpipes_registered_buffers + 1, max_xpipes_registered_buffers);
    exit(1);
  }
  
  xpipes_buffer_status[xpipes_registered_buffers].registered = true;
  xpipes_buffer_status[xpipes_registered_buffers].name       = name;
  xpipes_buffer_status[xpipes_registered_buffers].locations  = locations;
  xpipes_buffer_status[xpipes_registered_buffers].min_usage  = 0;
  xpipes_buffer_status[xpipes_registered_buffers].max_usage  = 0;
  xpipes_buffer_status[xpipes_registered_buffers].tot_usage  = 0;
  
  return (++ xpipes_registered_buffers - 1);
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// register_xpipes_link_receiver - Assigns a unique ID to a xpipes link receiver.
unsigned short int Statistics::register_xpipes_link_receiver(sc_string name,
                                                             unsigned short int ports)
{
  if (xpipes_registered_link_receivers >= max_xpipes_registered_link_receivers)
  {
    fprintf(stderr, "Fatal Error: too many xpipes link receivers trying to register (current = %hu, max = %hu)\n",
      xpipes_registered_link_receivers + 1, max_xpipes_registered_link_receivers);
    exit(1);
  }
  
  xpipes_link_receiver_status[xpipes_registered_link_receivers].registered  = true;
  xpipes_link_receiver_status[xpipes_registered_link_receivers].name        = name;
  xpipes_link_receiver_status[xpipes_registered_link_receivers].ports       = ports;
  for (uint i = 0; i < MAX_SWITCH_INPUTS; i ++)
  {
    xpipes_link_receiver_status[xpipes_registered_link_receivers].acked[i]  = 0;
    xpipes_link_receiver_status[xpipes_registered_link_receivers].nacked[i] = 0;
  }
  
  return (++ xpipes_registered_link_receivers - 1);
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_packet_request - Inspects the (round-trip) latency of a packet
//                                 across a xpipes fabric (called by ni_request).
void Statistics::inspect_xpipes_packet_request(unsigned short int ID,
                                               sc_uint<MCMDWD> cmd,
                                               sc_uint<MADDRWD> addr,
                                               sc_uint<MBURSTLENGTHWD> blen)
{
  uint i;
  
  ASSERT(ID <= max_xpipes_registered_nis);
  ASSERT(xpipes_ni_status[ID].registered);
  ASSERT(xpipes_ni_status[ID].is_initiator_ni);
  switch (cmd)
  {
    case OCPCMDWRITE: for (i = 0; i < MAX_IN_FLIGHT_PACKETS; i ++)
                      {
                        // FIXME highly inefficient linear search
                        if (!xpipes_ni_status[ID].in_flight_packets[i].flying)
                        {
                          /*printf("Called by ID %3hu @ %7.1f: adding a packet (source = 0x%x, cmd = 0x%x, address = 0x%08x, blen = %u) to slot %u\n",
                            ID, sc_simulation_time(), xpipes_ni_status[ID].source, (uint)cmd, (uint)addr, (uint)blen, i);*/
                          xpipes_ni_status[ID].in_flight_packets[i].flying       = true;
                          xpipes_ni_status[ID].in_flight_packets[i].source_ID    = ID;
                          // xpipes_ni_status[ID].in_flight_packets[i].target_ID still unknown
                          xpipes_ni_status[ID].in_flight_packets[i].cmd          = cmd;
                          xpipes_ni_status[ID].in_flight_packets[i].addr         = addr;
                          xpipes_ni_status[ID].in_flight_packets[i].blen         = blen;
                          xpipes_ni_status[ID].in_flight_packets[i].time_request = sc_simulation_time();
                          // xpipes_ni_status[ID].in_flight_packets[i].time_receive still unknown
                          // xpipes_ni_status[ID].in_flight_packets[i].time_resend still unknown
                          if (core_measuring == N_CORES)
                            xpipes_ni_status[ID].in_flight_packets[i].launched_while_stats_active = true;
                          else
                            xpipes_ni_status[ID].in_flight_packets[i].launched_while_stats_active = false;
                          break;
                        }
                      }
                      // We really SHOULD have found a free packet slot. If not, go
                      // down in flames (=> increase MAX_IN_FLIGHT_PACKETS next time!!)
                      ASSERT(i < MAX_IN_FLIGHT_PACKETS);
                      break;
    case OCPCMDREAD:
    case OCPCMDWRNP:  /*printf("Called by ID %3hu @ %7.1f: adding a packet (source = 0x%x, cmd = 0x%x, address = 0x%08x, blen = %u) to slot %u\n",
                        ID, sc_simulation_time(), xpipes_ni_status[ID].source, (uint)cmd, (uint)addr, (uint)blen, MAX_IN_FLIGHT_PACKETS);*/
                      // Use the "special entry" MAX_IN_FLIGHT_PACKETS to hold packets which will need a response
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].flying       = true;
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].source_ID    = ID;
                      // xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].target_ID still unknown
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].cmd          = cmd;
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].addr         = addr;
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].blen         = blen;
                      xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].time_request = sc_simulation_time();
                      // xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].time_receive still unknown
                      // xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].time_resend still unknown
                      if (core_measuring == N_CORES)
                        xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].launched_while_stats_active = true;
                      else
                        xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].launched_while_stats_active = false;
                      break;
    default:          printf("Fatal error: Invalid command %u detected from NI request %hu - %s @ %10.1f\n",
                        (uint)cmd, ID, xpipes_ni_status[ID].name.c_str(), sc_simulation_time());
                      exit(1);
  }
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_packet_receive - Inspects the (round-trip) latency of a packet
//                                 across a xpipes fabric (called by ni_receive).
void Statistics::inspect_xpipes_packet_receive(unsigned short int ID,
                                               unsigned int source,
                                               sc_uint<MADDRWD> addr,
                                               sc_uint<MCMDWD> cmd)
{
  // In-flight packet index from the ni_request
  int ni_match_index = -1, packet_match_index = -1;
  float time_request = sc_simulation_time(), time_receive = 0.0;
  bool add_to_counters = false;
  unsigned int ow_time;
  sc_uint<MBURSTLENGTHWD> blen;
  TRANS_TYPE t_type;

  ASSERT(ID <= max_xpipes_registered_nis);
  ASSERT(xpipes_ni_status[ID].registered);
  ASSERT(xpipes_ni_status[ID].is_target_ni);
  
  // If it is a write, it might come from a main memory for which this is the backup. Check this first
  if (cmd == OCPCMDWRITE)
  {
    for (uint i = 0; i < max_xpipes_registered_nis; i ++)
    {
      if (xpipes_ni_status[i].registered && xpipes_ni_status[i].shadow == xpipes_ni_status[ID].source && xpipes_ni_status[i].is_target_ni && xpipes_ni_status[i].is_initiator_ni)
      {
        /*printf("Called by ID %3hu @ %7.1f: attempt to find an outstanding packet (source = 0x%x, address = 0x%08x) in the list of NI ID %u (shadow %u)\n",
          ID, sc_simulation_time(), source, (uint)addr, i, xpipes_ni_status[i].shadow);*/
        ni_match_index = i;
        float min_sequential_order = time_request;
        
        for (uint j = 0; j < MAX_IN_FLIGHT_PACKETS; j ++)
        {
          // Catch the oldest forwarded packet
          if (xpipes_ni_status[i].in_flight_packets[j].flying &&
              xpipes_ni_status[i].in_flight_packets[j].time_request < time_request &&
              xpipes_ni_status[i].in_flight_packets[j].time_request < min_sequential_order)
          {
            ASSERT(xpipes_ni_status[i].in_flight_packets[j].cmd == OCPCMDWRITE);
            packet_match_index = j;
            time_request = xpipes_ni_status[i].in_flight_packets[j].time_request;
          }
        }
      }
      
      if (ni_match_index >= 0 && packet_match_index >= 0)
        break;
    }
  }
  
  // FIXME HIGHLY INEFFICIENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // If the check against a main memory yielded nothing, check whether there was any of the initiator NIs trying to talk to this NI
  if (ni_match_index < 0 || packet_match_index < 0)
  {
    ni_match_index = -1;
    packet_match_index = -1;
    
    for (uint i = 0; i < max_xpipes_registered_nis; i ++)
    {
      if (xpipes_ni_status[i].registered && xpipes_ni_status[i].source == source)
      {
        /*printf("Called by ID %3hu @ %7.1f: attempt to find an outstanding packet (source = 0x%x, address = 0x%08x) in the list of NI ID %u\n",
          ID, sc_simulation_time(), source, (uint)addr, i);*/
        ASSERT(xpipes_ni_status[i].is_initiator_ni);
        ni_match_index = i;
        
        for (uint j = 0; j < MAX_IN_FLIGHT_PACKETS + 1; j ++)
        {
          if (xpipes_ni_status[i].in_flight_packets[j].flying &&
              xpipes_ni_status[i].in_flight_packets[j].addr == addr &&
              xpipes_ni_status[i].in_flight_packets[j].cmd == cmd &&
              xpipes_ni_status[i].in_flight_packets[j].time_request < time_request)
          {
            packet_match_index = j;
            time_request = xpipes_ni_status[i].in_flight_packets[j].time_request;
            break;
          }
        }
      }
      
      if (ni_match_index >= 0 && packet_match_index >= 0)
        break;
    }
  }
  
  // The packet we received MUST be marked "in flight" somewhere!!
  ASSERT(ni_match_index >= 0);
  if (packet_match_index < 0)
    printf("Called by ID %3hu @ %7.1f: going to fail while matching packet (source = 0x%x, address = 0x%08x), found partial match in NI ID %d\n",
      ID, sc_simulation_time(), source, (uint)addr, ni_match_index);
  ASSERT(packet_match_index >= 0);
  
  ASSERT(cmd == xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].cmd);
  
  if (cmd == OCPCMDWRITE)
  {
    xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].flying = false;
    /*printf("Called by ID %3hu @ %7.1f: removing an outstanding write packet (source = 0x%x, address = 0x%08x) from slot %d of the list of NI ID %u\n",
      ID, sc_simulation_time(), source, (uint)addr, packet_match_index, ni_match_index);*/
  }
  blen = xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].blen;
  xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].target_ID = ID;
  time_receive = sc_simulation_time();
  xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].time_receive = time_receive;
  add_to_counters = xpipes_ni_status[ni_match_index].in_flight_packets[packet_match_index].launched_while_stats_active;
  ow_time = (unsigned int)(time_receive - time_request);
  
  switch (cmd)
  {
    case OCPCMDWRITE: if (blen == 1)
                        t_type = SINGLEWRITEPOSTED;
                      else
                        t_type = BURSTWRITEPOSTED;
                      break;
    case OCPCMDREAD:  if (blen == 1)
                        t_type = SINGLEREAD;
                      else
                        t_type = BURSTREAD;
                      break;
    case OCPCMDWRNP:  if (blen == 1)
                        t_type = SINGLEWRITENONPOSTED;
                      else
                        t_type = BURSTWRITENONPOSTED;
                      break;
    default:          printf("Fatal error: Invalid command %u detected from NI receive %hu - %s @ %10.1f\n",
                        (uint)cmd, ID, xpipes_ni_status[ID].name.c_str(), sc_simulation_time());
                      exit(1);
  }
  
  if (add_to_counters)
  {
    xpipes_ni_status[ni_match_index].sent_packets[ID][t_type] ++;
    xpipes_ni_status[ni_match_index].overall_sent_packets[t_type] ++;
    xpipes_overall_sent_packets[t_type] ++;
    
    xpipes_ni_status[ni_match_index].tot_ow_latency[ID][t_type] += ow_time;
    xpipes_ni_status[ni_match_index].overall_tot_ow_latency[t_type] += ow_time;
    xpipes_overall_tot_ow_latency[t_type] += ow_time;
    
    if (ow_time > xpipes_ni_status[ni_match_index].max_ow_latency[ID][t_type])
      xpipes_ni_status[ni_match_index].max_ow_latency[ID][t_type] = ow_time;
    if (ow_time > xpipes_ni_status[ni_match_index].overall_max_ow_latency[t_type])
      xpipes_ni_status[ni_match_index].overall_max_ow_latency[t_type] = ow_time;
    if (ow_time > xpipes_overall_max_ow_latency[t_type])
      xpipes_overall_max_ow_latency[t_type] = ow_time;
      
    if (ow_time < xpipes_ni_status[ni_match_index].min_ow_latency[ID][t_type])
      xpipes_ni_status[ni_match_index].min_ow_latency[ID][t_type] = ow_time;
    if (ow_time < xpipes_ni_status[ni_match_index].overall_min_ow_latency[t_type])
      xpipes_ni_status[ni_match_index].overall_min_ow_latency[t_type] = ow_time;
    if (ow_time < xpipes_overall_min_ow_latency[t_type])
      xpipes_overall_min_ow_latency[t_type] = ow_time;
  }
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_packet_resend - Inspects the (round-trip) latency of a packet
//                                across a xpipes fabric (called by ni_resend).
void Statistics::inspect_xpipes_packet_resend(unsigned short int ID)
{
  uint i;
  
  ASSERT(ID <= max_xpipes_registered_nis);
  ASSERT(xpipes_ni_status[ID].registered);
  ASSERT(xpipes_ni_status[ID].is_target_ni);
  
  // FIXME INEFFICIENT
  for (i = 0; i < max_xpipes_registered_nis; i ++)
  {
    if (xpipes_ni_status[i].registered &&
        xpipes_ni_status[i].in_flight_packets[MAX_IN_FLIGHT_PACKETS].flying &&
        xpipes_ni_status[i].in_flight_packets[MAX_IN_FLIGHT_PACKETS].target_ID == ID)
    {
      ASSERT(xpipes_ni_status[i].is_initiator_ni);
      xpipes_ni_status[i].in_flight_packets[MAX_IN_FLIGHT_PACKETS].time_resend = sc_simulation_time();
      break;
    }
  }
    
  ASSERT(i <= max_xpipes_registered_nis);
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_packet_response - Inspects the (round-trip) latency of a packet
//                                  across a xpipes fabric (called by ni_response).
void Statistics::inspect_xpipes_packet_response(unsigned short int ID)
{
  unsigned int rt_time;
  unsigned short int tID;
  TRANS_TYPE t_type;
  
  ASSERT(ID <= max_xpipes_registered_nis);
  ASSERT(xpipes_ni_status[ID].registered);
  ASSERT(xpipes_ni_status[ID].is_initiator_ni);
  ASSERT(xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].flying);

  rt_time = (unsigned int)(sc_simulation_time() - xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].time_request);
  tID = xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].target_ID;
  
  switch (xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].cmd)
  {
    case OCPCMDWRITE: if (xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].blen == 1)
                        t_type = SINGLEWRITEPOSTED;
                      else
                        t_type = BURSTWRITEPOSTED;
                      break;
    case OCPCMDREAD:  if (xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].blen == 1)
                        t_type = SINGLEREAD;
                      else
                        t_type = BURSTREAD;
                      break;
    case OCPCMDWRNP:  if (xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].blen == 1)
                        t_type = SINGLEWRITENONPOSTED;
                      else
                        t_type = BURSTWRITENONPOSTED;
                      break;
    default:          printf("Fatal error: Invalid command %u detected from NI response %hu - %s @ %10.1f\n",
                        (uint)xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].cmd, ID,
                        xpipes_ni_status[ID].name.c_str(), sc_simulation_time());
                      exit(1);
  }
  
  if (xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].launched_while_stats_active)
  {
    xpipes_ni_status[ID].tot_rt_latency[tID][t_type] += rt_time;
    xpipes_ni_status[ID].overall_tot_rt_latency[t_type] += rt_time;
    xpipes_overall_tot_rt_latency[t_type] += rt_time;
    
    if (rt_time > xpipes_ni_status[ID].max_rt_latency[tID][t_type])
      xpipes_ni_status[ID].max_rt_latency[tID][t_type] = rt_time;
    if (rt_time > xpipes_ni_status[ID].overall_max_rt_latency[t_type])
      xpipes_ni_status[ID].overall_max_rt_latency[t_type] = rt_time;
    if (rt_time > xpipes_overall_max_rt_latency[t_type])
      xpipes_overall_max_rt_latency[t_type] = rt_time;
      
    if (rt_time < xpipes_ni_status[ID].min_rt_latency[tID][t_type])
      xpipes_ni_status[ID].min_rt_latency[tID][t_type] = rt_time;
    if (rt_time < xpipes_ni_status[ID].overall_min_rt_latency[t_type])
      xpipes_ni_status[ID].overall_min_rt_latency[t_type] = rt_time;
    if (rt_time < xpipes_overall_min_rt_latency[t_type])
      xpipes_overall_min_rt_latency[t_type] = rt_time;
  }
  
  xpipes_ni_status[ID].in_flight_packets[MAX_IN_FLIGHT_PACKETS].flying = false;
  
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_buffer - Inspects the usage figures of xpipes output buffers.
void Statistics::inspect_xpipes_buffer(unsigned short int ID, unsigned short int filled)
{
  if (ID > max_xpipes_registered_buffers || !xpipes_buffer_status[ID].registered)
  {
    fprintf(stderr,
      "Fatal Error: attempt to track xpipes buffer status from unregistered/non-existing buffer %hu\n",
      ID);
    exit(1);
  }
  
  if (core_measuring == N_CORES)
  {
    if (filled > xpipes_buffer_status[ID].max_usage)
      xpipes_buffer_status[ID].max_usage = filled;
    
    xpipes_buffer_status[ID].tot_usage += filled;
  }
}
#endif


#ifdef XPIPESBUILD
/////////////////////////////////////////////////////////////////////////////
// inspect_xpipes_flow - Inspects the usage figures and retransmission ratios
//                       of xpipes links.
void Statistics::inspect_xpipes_flow(unsigned short int ID,
                                     sc_uint<MAX_SWITCH_INPUTS> ack_vector,
                                     sc_uint<MAX_SWITCH_INPUTS> ack_valid_vector,
                                     sc_uint<MAX_SWITCH_INPUTS> dummy)
{
  if (ID > max_xpipes_registered_link_receivers || !xpipes_link_receiver_status[ID].registered)
  {
    fprintf(stderr,
      "Fatal Error: attempt to track xpipes buffer status from unregistered/non-existing link receiver %hu\n",
      ID);
    exit(1);
  }
  
  if (core_measuring == N_CORES)
    for (unsigned short int i = 0; i < xpipes_link_receiver_status[ID].ports; i ++)
    {
      if (ack_valid_vector[i] && ack_vector[i])
        xpipes_link_receiver_status[ID].acked[i] ++;
      else if (ack_valid_vector[i] && !ack_vector[i])
        xpipes_link_receiver_status[ID].nacked[i] ++;
    }
}
#endif


///////////////////////////////////////////////////////////////////////////////
// printTimeResults - Dumps some information about simulation elapsed times.
void Statistics::printTimeResults()
{
  total_time = difftime(time(NULL), start_time);
  uint64_t cycles = (uint64_t)(sc_simulation_time() / CLOCKPERIOD);
  
  fprintf(fstat, "Simulation executed: %s", asctime(localtime(&start_time)));
  fprintf(fstat, "Elapsed time - overall simulation: %lu:%02u minutes\n", (unsigned long int)floor(total_time / 60),
            (unsigned int)(fmod(total_time, 60)) );
  fprintf(fstat, "Total simulated master system cycles: %llu (%.0f ns)\n", cycles, sc_simulation_time());
  fprintf(fstat, "CPU cycles simulated per second: %.1f\n", (double)(cycles * N_CORES)/total_time);

  for (uint i=0; i<N_CORES; i++)
    fprintf(fstat, "Elapsed time - processor %d critical section: %lu:%02u minutes\n", i,
              (unsigned long int)floor(core_c[i].total_sim_time_crit / 60),
              (unsigned int)(fmod(core_c[i].total_sim_time_crit, 60)) );
}


///////////////////////////////////////////////////////////////////////////////
// printInterconnectionResults - Dumps some information about interconnection usage.
void Statistics::printInterconnectionResults()
{
  double avg_time = 0.0;
  unsigned short int i;
  
  ACCESS_COUNTERS total_results;
  memset(&total_results, 0, sizeof(ACCESS_COUNTERS));
  // Non-zero initializations
  total_results.min_waittime    = 100000;
  total_results.min_waittime_sr = 100000;
  total_results.min_waittime_sw = 100000;
  total_results.min_waittime_br = 100000;
  total_results.min_waittime_bw = 100000;
  total_results.min_waittime_r  = 100000;
  total_results.min_waittime_w  = 100000;
  total_results.min_comptime    = 100000;
  total_results.min_comptime_sr = 100000;
  total_results.min_comptime_sw = 100000;
  total_results.min_comptime_br = 100000;
  total_results.min_comptime_bw = 100000;
  total_results.min_comptime_r  = 100000;
  total_results.min_comptime_w  = 100000;
  
  for (i = 0; i < N_CORES; i ++)
    avg_time += core_c[i].total_time_crit;
  avg_time /= N_CORES;
    
  fprintf(fstat, "-----------------------\n");
  fprintf(fstat, "Interconnect statistics\n");
  fprintf(fstat, "-----------------------\n");
  fprintf(fstat, "Overall exec time             = %lu master system cycles (%.2f ns)\n", one_core_exec, (double)one_core_exec * CLOCKPERIOD);
  fprintf(fstat, "1-CPU average exec time       = %lu master system cycles (%.0f ns)\n", (unsigned long int)(avg_time / CLOCKPERIOD), avg_time);
  fprintf(fstat, "Concurrent exec time          = %lu master system cycles (%.2f ns)\n", all_cores_exec, (double)all_cores_exec * CLOCKPERIOD);
  fprintf(fstat, "Bus busy                      = %lu master system cycles (%.2f%% of %lu)\n", bus_busy,
            (double)bus_busy * 100 / (double)(all_cores_exec ? all_cores_exec : 1), all_cores_exec);
  fprintf(fstat, "Bus transferring data         = %lu master system cycles (%.2f%% of %lu, %.2f%% of %lu)\n", transferring,
            (double)transferring * 100 / (double)(all_cores_exec ? all_cores_exec : 1), all_cores_exec,
            (double)transferring * 100 / (double)(bus_busy ? bus_busy : 1), bus_busy);
  // x ? x : 1 is a hack to avoid the possibility of 0/0 divisions

  for (i = 0; i < N_MASTERS; i ++)
  {
    if (master_c[i].accesses)
      total_results.accesses         += master_c[i].accesses;
    if (master_c[i].accesses_r)
    {
      total_results.accesses_r       += master_c[i].accesses_r;
      total_results.tot_waittime_r   += master_c[i].tot_waittime_r;
      total_results.tot_comptime_r   += master_c[i].tot_comptime_r;
      if (master_c[i].min_waittime_r  < total_results.min_waittime_r)
        total_results.min_waittime_r  = master_c[i].min_waittime_r;
      if (master_c[i].min_comptime_r  < total_results.min_comptime_r)
        total_results.min_comptime_r  = master_c[i].min_comptime_r;
      if (master_c[i].max_waittime_r  > total_results.max_waittime_r)
        total_results.max_waittime_r  = master_c[i].max_waittime_r;
      if (master_c[i].max_comptime_r  > total_results.max_comptime_r)
        total_results.max_comptime_r  = master_c[i].max_comptime_r;
    }
    if (master_c[i].accesses_w)
    {
      total_results.accesses_w       += master_c[i].accesses_w;
      total_results.tot_waittime_w   += master_c[i].tot_waittime_w;
      total_results.tot_comptime_w   += master_c[i].tot_comptime_w;
      if (master_c[i].min_waittime_w  < total_results.min_waittime_w)
        total_results.min_waittime_w  = master_c[i].min_waittime_w;
      if (master_c[i].min_comptime_w  < total_results.min_comptime_w)
        total_results.min_comptime_w  = master_c[i].min_comptime_w;
      if (master_c[i].max_waittime_w  > total_results.max_waittime_w)
        total_results.max_waittime_w  = master_c[i].max_waittime_w;
      if (master_c[i].max_comptime_w  > total_results.max_comptime_w)
        total_results.max_comptime_w  = master_c[i].max_comptime_w;
    }
    if (master_c[i].accesses_sr)
    {
      total_results.accesses_sr      += master_c[i].accesses_sr;
      total_results.tot_waittime_sr  += master_c[i].tot_waittime_sr;
      total_results.tot_comptime_sr  += master_c[i].tot_comptime_sr;
      if (master_c[i].min_waittime_sr < total_results.min_waittime_sr)
        total_results.min_waittime_sr = master_c[i].min_waittime_sr;
      if (master_c[i].min_comptime_sr < total_results.min_comptime_sr)
        total_results.min_comptime_sr = master_c[i].min_comptime_sr;
      if (master_c[i].max_waittime_sr > total_results.max_waittime_sr)
        total_results.max_waittime_sr = master_c[i].max_waittime_sr;
      if (master_c[i].max_comptime_sr > total_results.max_comptime_sr)
        total_results.max_comptime_sr = master_c[i].max_comptime_sr;
    }
    if (master_c[i].accesses_br)
    {
      total_results.accesses_br      += master_c[i].accesses_br;
      total_results.tot_waittime_br  += master_c[i].tot_waittime_br;
      total_results.tot_comptime_br  += master_c[i].tot_comptime_br;
      if (master_c[i].min_waittime_br < total_results.min_waittime_br)
        total_results.min_waittime_br = master_c[i].min_waittime_br;
      if (master_c[i].min_comptime_br < total_results.min_comptime_br)
        total_results.min_comptime_br = master_c[i].min_comptime_br;
      if (master_c[i].max_waittime_br > total_results.max_waittime_br)
        total_results.max_waittime_br = master_c[i].max_waittime_br;
      if (master_c[i].max_comptime_br > total_results.max_comptime_br)
        total_results.max_comptime_br = master_c[i].max_comptime_br;
    }
    if (master_c[i].accesses_sw)
    {
      total_results.accesses_sw      += master_c[i].accesses_sw;
      total_results.tot_waittime_sw  += master_c[i].tot_waittime_sw;
      total_results.tot_comptime_sw  += master_c[i].tot_comptime_sw;
      if (master_c[i].min_waittime_sw < total_results.min_waittime_sw)
        total_results.min_waittime_sw = master_c[i].min_waittime_sw;
      if (master_c[i].min_comptime_sw < total_results.min_comptime_sw)
        total_results.min_comptime_sw = master_c[i].min_comptime_sw;
      if (master_c[i].max_waittime_sw > total_results.max_waittime_sw)
        total_results.max_waittime_sw = master_c[i].max_waittime_sw;
      if (master_c[i].max_comptime_sw > total_results.max_comptime_sw)
        total_results.max_comptime_sw = master_c[i].max_comptime_sw;
    }
    if (master_c[i].accesses_bw)
    {
      total_results.accesses_bw      += master_c[i].accesses_bw;
      total_results.tot_waittime_bw  += master_c[i].tot_waittime_bw;
      total_results.tot_comptime_bw  += master_c[i].tot_comptime_bw;
      if (master_c[i].min_waittime_bw < total_results.min_waittime_bw)
        total_results.min_waittime_bw = master_c[i].min_waittime_bw;
      if (master_c[i].min_comptime_bw < total_results.min_comptime_bw)
        total_results.min_comptime_bw = master_c[i].min_comptime_bw;
      if (master_c[i].max_waittime_bw > total_results.max_waittime_bw)
        total_results.max_waittime_bw = master_c[i].max_waittime_bw;
      if (master_c[i].max_comptime_bw > total_results.max_comptime_bw)
        total_results.max_comptime_bw = master_c[i].max_comptime_bw;
    }
  }

  // FIXME this print code is duplicated with that of the master
  fprintf(fstat, "Bus Accesses                  = %lu (%lu SR, %lu SW, %lu BR, %lu BW: %lu R, %lu W)\n", total_results.accesses,
            total_results.accesses_sr, total_results.accesses_sw, total_results.accesses_br, total_results.accesses_bw,
            total_results.accesses_r, total_results.accesses_w);
  if (total_results.accesses_r)
  {
    fprintf(fstat, "Time (ns) to bus access (R)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_r, total_results.accesses_r, total_results.max_waittime_r,
                (double)total_results.tot_waittime_r / (double)(total_results.accesses_r),
                total_results.min_waittime_r);
    fprintf(fstat, "Time (ns) to bus compl. (R)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_r, total_results.accesses_r, total_results.max_comptime_r,
                (double)total_results.tot_comptime_r / (double)(total_results.accesses_r),
                total_results.min_comptime_r);
  }
  if (total_results.accesses_w)
  {
    fprintf(fstat, "Time (ns) to bus access (W)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_w, total_results.accesses_w, total_results.max_waittime_w,
                (double)total_results.tot_waittime_w / (double)(total_results.accesses_w),
                total_results.min_waittime_w);
    fprintf(fstat, "Time (ns) to bus compl. (W)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_w, total_results.accesses_w, total_results.max_comptime_w,
                (double)total_results.tot_comptime_w / (double)(total_results.accesses_w),
                total_results.min_comptime_w);
  }
  if (total_results.accesses_sr)
  {
    fprintf(fstat, "Time (ns) to bus access (SR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_sr, total_results.accesses_sr, total_results.max_waittime_sr,
                (double)total_results.tot_waittime_sr / (double)(total_results.accesses_sr),
                total_results.min_waittime_sr);
    fprintf(fstat, "Time (ns) to bus compl. (SR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_sr, total_results.accesses_sr, total_results.max_comptime_sr,
                (double)total_results.tot_comptime_sr / (double)(total_results.accesses_sr),
                total_results.min_comptime_sr);
  }
  if (total_results.accesses_sw)
  {
    fprintf(fstat, "Time (ns) to bus access (SW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_sw, total_results.accesses_sw, total_results.max_waittime_sw,
                (double)total_results.tot_waittime_sw / (double)(total_results.accesses_sw),
                total_results.min_waittime_sw);
    fprintf(fstat, "Time (ns) to bus compl. (SW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_sw, total_results.accesses_sw, total_results.max_comptime_sw,
                (double)total_results.tot_comptime_sw / (double)(total_results.accesses_sw),
                total_results.min_comptime_sw);
  }
  if (total_results.accesses_br)
  {
    fprintf(fstat, "Time (ns) to bus access (BR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_br, total_results.accesses_br, total_results.max_waittime_br,
                (double)total_results.tot_waittime_br / (double)(total_results.accesses_br),
                total_results.min_waittime_br);
    fprintf(fstat, "Time (ns) to bus compl. (BR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_br, total_results.accesses_br, total_results.max_comptime_br,
                (double)total_results.tot_comptime_br / (double)(total_results.accesses_br),
                total_results.min_comptime_br);
  }
  if (total_results.accesses_bw)
  {
    fprintf(fstat, "Time (ns) to bus access (BW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_waittime_bw, total_results.accesses_bw, total_results.max_waittime_bw,
                (double)total_results.tot_waittime_bw / (double)(total_results.accesses_bw),
                total_results.min_waittime_bw);
    fprintf(fstat, "Time (ns) to bus compl. (BW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_bw, total_results.accesses_bw, total_results.max_comptime_bw,
                (double)total_results.tot_comptime_bw / (double)(total_results.accesses_bw),
                total_results.min_comptime_bw);
  }
}


///////////////////////////////////////////////////////////////////////////////
// printOCPLatencyResults - Dumps information about OCP transaction latencies.
void Statistics::printOCPLatencyResults()
{
  double avg_time = 0.0;
  unsigned short int i;
  
  OCP_COUNTERS total_results;
  memset(&total_results, 0, sizeof(OCP_COUNTERS));
  // Non-zero initializations
  total_results.min_cmdacctime      = 100000;
  total_results.min_cmdacctime_sr   = 100000;
  total_results.min_cmdacctime_swp  = 100000;
  total_results.min_cmdacctime_swnp = 100000;
  total_results.min_cmdacctime_br   = 100000;
  total_results.min_cmdacctime_bwp  = 100000;
  total_results.min_cmdacctime_bwnp = 100000;
  total_results.min_cmdacctime_r    = 100000;
  total_results.min_cmdacctime_wp   = 100000;
  total_results.min_cmdacctime_wnp  = 100000;
  total_results.min_comptime        = 100000;
  total_results.min_comptime_sr     = 100000;
  total_results.min_comptime_swnp   = 100000;
  total_results.min_comptime_br     = 100000;
  total_results.min_comptime_bwnp   = 100000;
  total_results.min_comptime_r      = 100000;
  total_results.min_comptime_wnp    = 100000;

  for (i = 0; i < N_CORES; i ++)
    avg_time += core_c[i].total_time_crit;
  avg_time /= N_CORES;
  
  for (i = 0; i < N_MASTERS; i ++)
  {
    if (master_ocp_c[i].accesses)
      total_results.accesses                 += master_ocp_c[i].accesses;
    if (master_ocp_c[i].accesses_r)
    {
      total_results.accesses_r               += master_ocp_c[i].accesses_r;
      total_results.tot_cmdacctime_r         += master_ocp_c[i].tot_cmdacctime_r;
      total_results.tot_comptime_r           += master_ocp_c[i].tot_comptime_r;
      if (master_ocp_c[i].min_cmdacctime_r    < total_results.min_cmdacctime_r)
        total_results.min_cmdacctime_r        = master_ocp_c[i].min_cmdacctime_r;
      if (master_ocp_c[i].min_comptime_r      < total_results.min_comptime_r)
        total_results.min_comptime_r          = master_ocp_c[i].min_comptime_r;
      if (master_ocp_c[i].max_cmdacctime_r    > total_results.max_cmdacctime_r)
        total_results.max_cmdacctime_r        = master_ocp_c[i].max_cmdacctime_r;
      if (master_ocp_c[i].max_comptime_r      > total_results.max_comptime_r)
        total_results.max_comptime_r          = master_ocp_c[i].max_comptime_r;
    }
    if (master_ocp_c[i].accesses_wp)
    {
      total_results.accesses_wp              += master_ocp_c[i].accesses_wp;
      total_results.tot_cmdacctime_wp        += master_ocp_c[i].tot_cmdacctime_wp;
      if (master_ocp_c[i].min_cmdacctime_wp   < total_results.min_cmdacctime_wp)
        total_results.min_cmdacctime_wp       = master_ocp_c[i].min_cmdacctime_wp;
      if (master_ocp_c[i].max_cmdacctime_wp   > total_results.max_cmdacctime_wp)
        total_results.max_cmdacctime_wp       = master_ocp_c[i].max_cmdacctime_wp;
    }
    if (master_ocp_c[i].accesses_wnp)
    {
      total_results.accesses_wnp             += master_ocp_c[i].accesses_wnp;
      total_results.tot_cmdacctime_wnp       += master_ocp_c[i].tot_cmdacctime_wnp;
      total_results.tot_comptime_wnp         += master_ocp_c[i].tot_comptime_wnp;
      if (master_ocp_c[i].min_cmdacctime_wnp  < total_results.min_cmdacctime_wnp)
        total_results.min_cmdacctime_wnp      = master_ocp_c[i].min_cmdacctime_wnp;
      if (master_ocp_c[i].min_comptime_wnp    < total_results.min_comptime_wnp)
        total_results.min_comptime_wnp        = master_ocp_c[i].min_comptime_wnp;
      if (master_ocp_c[i].max_cmdacctime_wnp  > total_results.max_cmdacctime_wnp)
        total_results.max_cmdacctime_wnp      = master_ocp_c[i].max_cmdacctime_wnp;
      if (master_ocp_c[i].max_comptime_wnp    > total_results.max_comptime_wnp)
        total_results.max_comptime_wnp        = master_ocp_c[i].max_comptime_wnp;
    }
    if (master_ocp_c[i].accesses_sr)
    {
      total_results.accesses_sr              += master_ocp_c[i].accesses_sr;
      total_results.tot_cmdacctime_sr        += master_ocp_c[i].tot_cmdacctime_sr;
      total_results.tot_comptime_sr          += master_ocp_c[i].tot_comptime_sr;
      if (master_ocp_c[i].min_cmdacctime_sr   < total_results.min_cmdacctime_sr)
        total_results.min_cmdacctime_sr       = master_ocp_c[i].min_cmdacctime_sr;
      if (master_ocp_c[i].min_comptime_sr     < total_results.min_comptime_sr)
        total_results.min_comptime_sr         = master_ocp_c[i].min_comptime_sr;
      if (master_ocp_c[i].max_cmdacctime_sr   > total_results.max_cmdacctime_sr)
        total_results.max_cmdacctime_sr       = master_ocp_c[i].max_cmdacctime_sr;
      if (master_ocp_c[i].max_comptime_sr     > total_results.max_comptime_sr)
        total_results.max_comptime_sr         = master_ocp_c[i].max_comptime_sr;
    }
    if (master_ocp_c[i].accesses_br)
    {
      total_results.accesses_br              += master_ocp_c[i].accesses_br;
      total_results.tot_cmdacctime_br        += master_ocp_c[i].tot_cmdacctime_br;
      total_results.tot_comptime_br          += master_ocp_c[i].tot_comptime_br;
      if (master_ocp_c[i].min_cmdacctime_br   < total_results.min_cmdacctime_br)
        total_results.min_cmdacctime_br       = master_ocp_c[i].min_cmdacctime_br;
      if (master_ocp_c[i].min_comptime_br     < total_results.min_comptime_br)
        total_results.min_comptime_br         = master_ocp_c[i].min_comptime_br;
      if (master_ocp_c[i].max_cmdacctime_br   > total_results.max_cmdacctime_br)
        total_results.max_cmdacctime_br       = master_ocp_c[i].max_cmdacctime_br;
      if (master_ocp_c[i].max_comptime_br     > total_results.max_comptime_br)
        total_results.max_comptime_br         = master_ocp_c[i].max_comptime_br;
    }
    if (master_ocp_c[i].accesses_swp)
    {
      total_results.accesses_swp             += master_ocp_c[i].accesses_swp;
      total_results.tot_cmdacctime_swp       += master_ocp_c[i].tot_cmdacctime_swp;
      if (master_ocp_c[i].min_cmdacctime_swp  < total_results.min_cmdacctime_swp)
        total_results.min_cmdacctime_swp      = master_ocp_c[i].min_cmdacctime_swp;
      if (master_ocp_c[i].max_cmdacctime_swp  > total_results.max_cmdacctime_swp)
        total_results.max_cmdacctime_swp      = master_ocp_c[i].max_cmdacctime_swp;
    }
    if (master_ocp_c[i].accesses_bwp)
    {
      total_results.accesses_bwp             += master_ocp_c[i].accesses_bwp;
      total_results.tot_cmdacctime_bwp       += master_ocp_c[i].tot_cmdacctime_bwp;
      if (master_ocp_c[i].min_cmdacctime_bwp  < total_results.min_cmdacctime_bwp)
        total_results.min_cmdacctime_bwp      = master_ocp_c[i].min_cmdacctime_bwp;
      if (master_ocp_c[i].max_cmdacctime_bwp  > total_results.max_cmdacctime_bwp)
        total_results.max_cmdacctime_bwp      = master_ocp_c[i].max_cmdacctime_bwp;
    }
    if (master_ocp_c[i].accesses_swnp)
    {
      total_results.accesses_swnp            += master_ocp_c[i].accesses_swnp;
      total_results.tot_cmdacctime_swnp      += master_ocp_c[i].tot_cmdacctime_swnp;
      total_results.tot_comptime_swnp        += master_ocp_c[i].tot_comptime_swnp;
      if (master_ocp_c[i].min_cmdacctime_swnp < total_results.min_cmdacctime_swnp)
        total_results.min_cmdacctime_swnp     = master_ocp_c[i].min_cmdacctime_swnp;
      if (master_ocp_c[i].min_comptime_swnp   < total_results.min_comptime_swnp)
        total_results.min_comptime_swnp       = master_ocp_c[i].min_comptime_swnp;
      if (master_ocp_c[i].max_cmdacctime_swnp > total_results.max_cmdacctime_swnp)
        total_results.max_cmdacctime_swnp     = master_ocp_c[i].max_cmdacctime_swnp;
      if (master_ocp_c[i].max_comptime_swnp   > total_results.max_comptime_swnp)
        total_results.max_comptime_swnp       = master_ocp_c[i].max_comptime_swnp;
    }
    if (master_ocp_c[i].accesses_bwnp)
    {
      total_results.accesses_bwnp            += master_ocp_c[i].accesses_bwnp;
      total_results.tot_cmdacctime_bwnp      += master_ocp_c[i].tot_cmdacctime_bwnp;
      total_results.tot_comptime_bwnp        += master_ocp_c[i].tot_comptime_bwnp;
      if (master_ocp_c[i].min_cmdacctime_bwnp < total_results.min_cmdacctime_bwnp)
        total_results.min_cmdacctime_bwnp     = master_ocp_c[i].min_cmdacctime_bwnp;
      if (master_ocp_c[i].min_comptime_bwnp   < total_results.min_comptime_bwnp)
        total_results.min_comptime_bwnp       = master_ocp_c[i].min_comptime_bwnp;
      if (master_ocp_c[i].max_cmdacctime_bwnp > total_results.max_cmdacctime_bwnp)
        total_results.max_cmdacctime_bwnp     = master_ocp_c[i].max_cmdacctime_bwnp;
      if (master_ocp_c[i].max_comptime_bwnp   > total_results.max_comptime_bwnp)
        total_results.max_comptime_bwnp       = master_ocp_c[i].max_comptime_bwnp;
    }
  }
  
  fprintf(fstat, "--------------------------\n");
  fprintf(fstat, "OCP performance statistics\n");
  fprintf(fstat, "--------------------------\n");
  // FIXME this print code should be duplicated for each master
  fprintf(fstat, "-----\n");
  fprintf(fstat, "OCP Accesses                   = %lu (%lu SR, %lu BR, %lu SWP, %lu BWP, %lu SWNP, %lu BWNP: %lu R, %lu WP, %lu WNP)\n", total_results.accesses,
            total_results.accesses_sr, total_results.accesses_br, total_results.accesses_swp, total_results.accesses_bwp, total_results.accesses_swnp,
            total_results.accesses_bwnp, total_results.accesses_r, total_results.accesses_wp, total_results.accesses_wnp);
  fprintf(fstat, "-----\n");
  if (total_results.accesses_sr)
  {
    fprintf(fstat, "Time (ns) to cmd accept (SR)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_sr, total_results.accesses_sr, total_results.max_cmdacctime_sr,
                (double)total_results.tot_cmdacctime_sr / (double)(total_results.accesses_sr),
                total_results.min_cmdacctime_sr);
    fprintf(fstat, "Time (ns) to last resp. (SR)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_sr, total_results.accesses_sr, total_results.max_comptime_sr,
                (double)total_results.tot_comptime_sr / (double)(total_results.accesses_sr),
                total_results.min_comptime_sr);
  }
  if (total_results.accesses_br)
  {
    fprintf(fstat, "Time (ns) to cmd accept (BR)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_br, total_results.accesses_br, total_results.max_cmdacctime_br,
                (double)total_results.tot_cmdacctime_br / (double)(total_results.accesses_br),
                total_results.min_cmdacctime_br);
    fprintf(fstat, "Time (ns) to last resp. (BR)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_br, total_results.accesses_br, total_results.max_comptime_br,
                (double)total_results.tot_comptime_br / (double)(total_results.accesses_br),
                total_results.min_comptime_br);
  }
  if (total_results.accesses_swp)
  {
    fprintf(fstat, "Time (ns) to cmd accept (SWP)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_swp, total_results.accesses_swp, total_results.max_cmdacctime_swp,
                (double)total_results.tot_cmdacctime_swp / (double)(total_results.accesses_swp),
                total_results.min_cmdacctime_swp);
  }
  if (total_results.accesses_bwp)
  {
    fprintf(fstat, "Time (ns) to cmd accept (BWP)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_bwp, total_results.accesses_bwp, total_results.max_cmdacctime_bwp,
                (double)total_results.tot_cmdacctime_bwp / (double)(total_results.accesses_bwp),
                total_results.min_cmdacctime_bwp);
  }
  if (total_results.accesses_swnp)
  {
    fprintf(fstat, "Time (ns) to cmd accept (SWNP) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_swnp, total_results.accesses_swnp, total_results.max_cmdacctime_swnp,
                (double)total_results.tot_cmdacctime_swnp / (double)(total_results.accesses_swnp),
                total_results.min_cmdacctime_swnp);
    fprintf(fstat, "Time (ns) to last resp. (SWNP) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_swnp, total_results.accesses_swnp, total_results.max_comptime_swnp,
                (double)total_results.tot_comptime_swnp / (double)(total_results.accesses_swnp),
                total_results.min_comptime_swnp);
  }
  if (total_results.accesses_bwnp)
  {
    fprintf(fstat, "Time (ns) to cmd accept (BWNP) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_bwnp, total_results.accesses_bwnp, total_results.max_cmdacctime_bwnp,
                (double)total_results.tot_cmdacctime_bwnp / (double)(total_results.accesses_bwnp),
                total_results.min_cmdacctime_bwnp);
    fprintf(fstat, "Time (ns) to last resp. (BWNP) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_bwnp, total_results.accesses_bwnp, total_results.max_comptime_bwnp,
                (double)total_results.tot_comptime_bwnp / (double)(total_results.accesses_bwnp),
                total_results.min_comptime_bwnp);
  }
  if (total_results.accesses_r)
  {
    fprintf(fstat, "-----\n");
    fprintf(fstat, "Time (ns) to cmd accept (R)    = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_r, total_results.accesses_r, total_results.max_cmdacctime_r,
                (double)total_results.tot_cmdacctime_r / (double)(total_results.accesses_r),
                total_results.min_cmdacctime_r);
    fprintf(fstat, "Time (ns) to last resp. (R)    = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_r, total_results.accesses_r, total_results.max_comptime_r,
                (double)total_results.tot_comptime_r / (double)(total_results.accesses_r),
                total_results.min_comptime_r);
    fprintf(fstat, "-----\n");
  }
  if (total_results.accesses_wp)
  {
    fprintf(fstat, "-----\n");
    fprintf(fstat, "Time (ns) to cmd accept (WP)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_wp, total_results.accesses_wp, total_results.max_cmdacctime_wp,
                (double)total_results.tot_cmdacctime_wp / (double)(total_results.accesses_wp),
                total_results.min_cmdacctime_wp);
    fprintf(fstat, "-----\n");
  }
  if (total_results.accesses_wnp)
  {
    fprintf(fstat, "-----\n");
    fprintf(fstat, "Time (ns) to cmd accept (WNP)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_cmdacctime_wnp, total_results.accesses_wnp, total_results.max_cmdacctime_wnp,
                (double)total_results.tot_cmdacctime_wnp / (double)(total_results.accesses_wnp),
                total_results.min_cmdacctime_wnp);
    fprintf(fstat, "Time (ns) to last resp. (WNP)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
                total_results.tot_comptime_wnp, total_results.accesses_wnp, total_results.max_comptime_wnp,
                (double)total_results.tot_comptime_wnp / (double)(total_results.accesses_wnp),
                total_results.min_comptime_wnp);
    fprintf(fstat, "-----\n");
  }
}


#ifdef XPIPESBUILD
///////////////////////////////////////////////////////////////////////////////
// printXpipesResults - Dumps some information about xpipes fabric usage.
void Statistics::printXpipesResults()
{
  double avg_time = 0.0;
  unsigned short int i, j;
  
  for (i=0; i<N_CORES; i++)
    avg_time += core_c[i].total_time_crit;
  avg_time /= N_CORES;
    
  fprintf(fstat, "-----------------\n");
  fprintf(fstat, "xpipes statistics\n");
  fprintf(fstat, "-----------------\n");
  fprintf(fstat, "Overall exec time             = %lu master system cycles (%.2f ns)\n", one_core_exec, (double)one_core_exec * CLOCKPERIOD);
  fprintf(fstat, "1-CPU average exec time       = %lu master system cycles (%.0f ns)\n", (unsigned long int)(avg_time / CLOCKPERIOD), avg_time);
  fprintf(fstat, "Concurrent exec time          = %lu master system cycles (%.2f ns)\n", all_cores_exec, (double)all_cores_exec * CLOCKPERIOD);

  // Platform overall
  // FIXME if possible, try to refactorize this section of duplicated output code
  fprintf(fstat, "\nOverall traffic on the xpipes platform (latencies expressed in ns):\n");
  fprintf(fstat, "    ---------------------------------------------------------------------------\n");
  fprintf(fstat, "    |                   |   SR   |   BR   |   SWP  |   BWP  |  SWNP  |  BWNP  |\n");
  fprintf(fstat, "    ---------------------------------------------------------------------------\n");
  fprintf(fstat, "    |Packets            | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
    xpipes_overall_sent_packets[SINGLEREAD], xpipes_overall_sent_packets[BURSTREAD],
    xpipes_overall_sent_packets[SINGLEWRITEPOSTED], xpipes_overall_sent_packets[BURSTWRITEPOSTED],
    xpipes_overall_sent_packets[SINGLEWRITENONPOSTED], xpipes_overall_sent_packets[BURSTWRITENONPOSTED]);
  fprintf(fstat, "    ---------------------------------------------------------------------------\n");
  fprintf(fstat, "    |1-way  latency avg |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |\n",
    (float)xpipes_overall_tot_ow_latency[SINGLEREAD] /
    (float)(xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_sent_packets[SINGLEREAD] : 1),
    (float)xpipes_overall_tot_ow_latency[BURSTREAD] /
    (float)(xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_sent_packets[BURSTREAD] : 1),
    (float)xpipes_overall_tot_ow_latency[SINGLEWRITEPOSTED] /
    (float)(xpipes_overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_overall_sent_packets[SINGLEWRITEPOSTED] : 1),
    (float)xpipes_overall_tot_ow_latency[BURSTWRITEPOSTED] /
    (float)(xpipes_overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_overall_sent_packets[BURSTWRITEPOSTED] : 1),
    (float)xpipes_overall_tot_ow_latency[SINGLEWRITENONPOSTED] /
    (float)(xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] : 1),
    (float)xpipes_overall_tot_ow_latency[BURSTWRITENONPOSTED] /
    (float)(xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_sent_packets[BURSTWRITENONPOSTED] : 1));
  fprintf(fstat, "    |1-way  latency min | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
    xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_min_ow_latency[SINGLEREAD] : 0,
    xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_min_ow_latency[BURSTREAD] : 0,
    xpipes_overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_overall_min_ow_latency[SINGLEWRITEPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_overall_min_ow_latency[BURSTWRITEPOSTED] : 0,
    xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_min_ow_latency[SINGLEWRITENONPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_min_ow_latency[BURSTWRITENONPOSTED] : 0);
  fprintf(fstat, "    |1-way  latency max | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
    xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_max_ow_latency[SINGLEREAD] : 0,
    xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_max_ow_latency[BURSTREAD] : 0,
    xpipes_overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_overall_max_ow_latency[SINGLEWRITEPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_overall_max_ow_latency[BURSTWRITEPOSTED] : 0,
    xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_max_ow_latency[SINGLEWRITENONPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_max_ow_latency[BURSTWRITENONPOSTED] : 0);
  fprintf(fstat, "    ---------------------------------------------------------------------------\n");
  fprintf(fstat, "    |R-trip latency avg |%7.2f |%7.2f |        |        |%7.2f |%7.2f |\n",
    (float)xpipes_overall_tot_rt_latency[SINGLEREAD] /
    (float)(xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_sent_packets[SINGLEREAD] : 1),
    (float)xpipes_overall_tot_rt_latency[BURSTREAD] /
    (float)(xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_sent_packets[BURSTREAD] : 1),
    (float)xpipes_overall_tot_rt_latency[SINGLEWRITENONPOSTED] /
    (float)(xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] : 1),
    (float)xpipes_overall_tot_rt_latency[BURSTWRITENONPOSTED] /
    (float)(xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_sent_packets[BURSTWRITENONPOSTED] : 1));
  fprintf(fstat, "    |R-trip latency min | %6lu | %6lu |        |        | %6lu | %6lu |\n",
    xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_min_rt_latency[SINGLEREAD] : 0,
    xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_min_rt_latency[BURSTREAD] : 0,
    xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_min_rt_latency[SINGLEWRITENONPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_min_rt_latency[BURSTWRITENONPOSTED] : 0);
  fprintf(fstat, "    |R-trip latency max | %6lu | %6lu |        |        | %6lu | %6lu |\n",
    xpipes_overall_sent_packets[SINGLEREAD] ? xpipes_overall_max_rt_latency[SINGLEREAD] : 0,
    xpipes_overall_sent_packets[BURSTREAD] ? xpipes_overall_max_rt_latency[BURSTREAD] : 0,
    xpipes_overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_overall_max_rt_latency[SINGLEWRITENONPOSTED] : 0,
    xpipes_overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_overall_max_rt_latency[BURSTWRITENONPOSTED] : 0);
  fprintf(fstat, "    ---------------------------------------------------------------------------\n\n");
      
  for (i = 0; i < max_xpipes_registered_nis; i ++)
  {
    if (xpipes_ni_status[i].registered && xpipes_ni_status[i].is_initiator_ni)
    {
      fprintf(fstat, "\nxpipes NI %hu - %s (internal ID %u, source 0x%x, type %s):\n",
        i, xpipes_ni_status[i].name.c_str(), xpipes_ni_status[i].ID, xpipes_ni_status[i].source,
        (xpipes_ni_status[i].is_target_ni && xpipes_ni_status[i].is_initiator_ni ? "initiator_target" :
        (xpipes_ni_status[i].is_target_ni ? "target" : "initiator")));
        
      // Overall per packet type
      // FIXME if possible, try to refactorize this section of duplicated output code
      if ((xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] + xpipes_ni_status[i].overall_sent_packets[BURSTREAD] +
           xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED] + xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED] +
           xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] + xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED]) > 0)
      {
        fprintf(fstat, "  Overall traffic generated by this NI (latencies expressed in ns):\n");
        fprintf(fstat, "      ---------------------------------------------------------------------------\n");
        fprintf(fstat, "      |                   |   SR   |   BR   |   SWP  |   BWP  |  SWNP  |  BWNP  |\n");
        fprintf(fstat, "      ---------------------------------------------------------------------------\n");
        fprintf(fstat, "      |Packets            | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
          xpipes_ni_status[i].overall_sent_packets[SINGLEREAD], xpipes_ni_status[i].overall_sent_packets[BURSTREAD],
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED], xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED],
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED], xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED]);
        fprintf(fstat, "      ---------------------------------------------------------------------------\n");
        fprintf(fstat, "      |1-way  latency avg |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |\n",
          (float)xpipes_ni_status[i].overall_tot_ow_latency[SINGLEREAD] /
          (float)(xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] : 1),
          (float)xpipes_ni_status[i].overall_tot_ow_latency[BURSTREAD] /
          (float)(xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_sent_packets[BURSTREAD] : 1),
          (float)xpipes_ni_status[i].overall_tot_ow_latency[SINGLEWRITEPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED] : 1),
          (float)xpipes_ni_status[i].overall_tot_ow_latency[BURSTWRITEPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED] : 1),
          (float)xpipes_ni_status[i].overall_tot_ow_latency[SINGLEWRITENONPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] : 1),
          (float)xpipes_ni_status[i].overall_tot_ow_latency[BURSTWRITENONPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] : 1));
        fprintf(fstat, "      |1-way  latency min | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
          xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_min_ow_latency[SINGLEREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_min_ow_latency[BURSTREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_ni_status[i].overall_min_ow_latency[SINGLEWRITEPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_ni_status[i].overall_min_ow_latency[BURSTWRITEPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_min_ow_latency[SINGLEWRITENONPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_min_ow_latency[BURSTWRITENONPOSTED] : 0);
        fprintf(fstat, "      |1-way  latency max | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
          xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_max_ow_latency[SINGLEREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_max_ow_latency[BURSTREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITEPOSTED] ? xpipes_ni_status[i].overall_max_ow_latency[SINGLEWRITEPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITEPOSTED] ? xpipes_ni_status[i].overall_max_ow_latency[BURSTWRITEPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_max_ow_latency[SINGLEWRITENONPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_max_ow_latency[BURSTWRITENONPOSTED] : 0);
        fprintf(fstat, "      ---------------------------------------------------------------------------\n");
        fprintf(fstat, "      |R-trip latency avg |%7.2f |%7.2f |        |        |%7.2f |%7.2f |\n",
          (float)xpipes_ni_status[i].overall_tot_rt_latency[SINGLEREAD] /
          (float)(xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] : 1),
          (float)xpipes_ni_status[i].overall_tot_rt_latency[BURSTREAD] /
          (float)(xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_sent_packets[BURSTREAD] : 1),
          (float)xpipes_ni_status[i].overall_tot_rt_latency[SINGLEWRITENONPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] : 1),
          (float)xpipes_ni_status[i].overall_tot_rt_latency[BURSTWRITENONPOSTED] /
          (float)(xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] : 1));
        fprintf(fstat, "      |R-trip latency min | %6lu | %6lu |        |        | %6lu | %6lu |\n",
          xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_min_rt_latency[SINGLEREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_min_rt_latency[BURSTREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_min_rt_latency[SINGLEWRITENONPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_min_rt_latency[BURSTWRITENONPOSTED] : 0);
        fprintf(fstat, "      |R-trip latency max | %6lu | %6lu |        |        | %6lu | %6lu |\n",
          xpipes_ni_status[i].overall_sent_packets[SINGLEREAD] ? xpipes_ni_status[i].overall_max_rt_latency[SINGLEREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTREAD] ? xpipes_ni_status[i].overall_max_rt_latency[BURSTREAD] : 0,
          xpipes_ni_status[i].overall_sent_packets[SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].overall_max_rt_latency[SINGLEWRITENONPOSTED] : 0,
          xpipes_ni_status[i].overall_sent_packets[BURSTWRITENONPOSTED] ? xpipes_ni_status[i].overall_max_rt_latency[BURSTWRITENONPOSTED] : 0);
        fprintf(fstat, "      ---------------------------------------------------------------------------\n");
      }
      
      // Per destination and packet type
      for (j = 0; j < max_xpipes_registered_nis; j ++)
      {
        if (xpipes_ni_status[j].is_target_ni &&
            (xpipes_ni_status[i].sent_packets[j][SINGLEREAD] + xpipes_ni_status[i].sent_packets[j][BURSTREAD] +
             xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED] + xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED] +
             xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] + xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED]) > 0)
        {
          fprintf(fstat, "  Traffic towards xpipes target NI %hu - %s (internal ID %u, source 0x%x)\n  (latencies expressed in ns):\n",
            j, xpipes_ni_status[j].name.c_str(), xpipes_ni_status[j].ID, xpipes_ni_status[j].source);
          fprintf(fstat, "      ---------------------------------------------------------------------------\n");
          fprintf(fstat, "      |                   |   SR   |   BR   |   SWP  |   BWP  |  SWNP  |  BWNP  |\n");
          fprintf(fstat, "      ---------------------------------------------------------------------------\n");
          fprintf(fstat, "      |Packets            | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
            xpipes_ni_status[i].sent_packets[j][SINGLEREAD], xpipes_ni_status[i].sent_packets[j][BURSTREAD],
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED], xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED],
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED], xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED]);
          fprintf(fstat, "      ---------------------------------------------------------------------------\n");
          fprintf(fstat, "      |1-way  latency avg |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |%7.2f |\n",
            (float)xpipes_ni_status[i].tot_ow_latency[j][SINGLEREAD] /
            (float)(xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].sent_packets[j][SINGLEREAD] : 1),
            (float)xpipes_ni_status[i].tot_ow_latency[j][BURSTREAD] /
            (float)(xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].sent_packets[j][BURSTREAD] : 1),
            (float)xpipes_ni_status[i].tot_ow_latency[j][SINGLEWRITEPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED] ? xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED] : 1),
            (float)xpipes_ni_status[i].tot_ow_latency[j][BURSTWRITEPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED] ? xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED] : 1),
            (float)xpipes_ni_status[i].tot_ow_latency[j][SINGLEWRITENONPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] : 1),
            (float)xpipes_ni_status[i].tot_ow_latency[j][BURSTWRITENONPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] : 1));
          fprintf(fstat, "      |1-way  latency min | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
            xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].min_ow_latency[j][SINGLEREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].min_ow_latency[j][BURSTREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED] ? xpipes_ni_status[i].min_ow_latency[j][SINGLEWRITEPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED] ? xpipes_ni_status[i].min_ow_latency[j][BURSTWRITEPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].min_ow_latency[j][SINGLEWRITENONPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].min_ow_latency[j][BURSTWRITENONPOSTED] : 0);
          fprintf(fstat, "      |1-way  latency max | %6lu | %6lu | %6lu | %6lu | %6lu | %6lu |\n",
            xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].max_ow_latency[j][SINGLEREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].max_ow_latency[j][BURSTREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITEPOSTED] ? xpipes_ni_status[i].max_ow_latency[j][SINGLEWRITEPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITEPOSTED] ? xpipes_ni_status[i].max_ow_latency[j][BURSTWRITEPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].max_ow_latency[j][SINGLEWRITENONPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].max_ow_latency[j][BURSTWRITENONPOSTED] : 0);
          fprintf(fstat, "      ---------------------------------------------------------------------------\n");
          fprintf(fstat, "      |R-trip latency avg |%7.2f |%7.2f |        |        |%7.2f |%7.2f |\n",
            (float)xpipes_ni_status[i].tot_rt_latency[j][SINGLEREAD] /
            (float)(xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].sent_packets[j][SINGLEREAD] : 1),
            (float)xpipes_ni_status[i].tot_rt_latency[j][BURSTREAD] /
            (float)(xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].sent_packets[j][BURSTREAD] : 1),
            (float)xpipes_ni_status[i].tot_rt_latency[j][SINGLEWRITENONPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] : 1),
            (float)xpipes_ni_status[i].tot_rt_latency[j][BURSTWRITENONPOSTED] /
            (float)(xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] : 1));
          fprintf(fstat, "      |R-trip latency min | %6lu | %6lu |        |        | %6lu | %6lu |\n",
            xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].min_rt_latency[j][SINGLEREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].min_rt_latency[j][BURSTREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].min_rt_latency[j][SINGLEWRITENONPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].min_rt_latency[j][BURSTWRITENONPOSTED] : 0);
          fprintf(fstat, "      |R-trip latency max | %6lu | %6lu |        |        | %6lu | %6lu |\n",
            xpipes_ni_status[i].sent_packets[j][SINGLEREAD] ? xpipes_ni_status[i].max_rt_latency[j][SINGLEREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTREAD] ? xpipes_ni_status[i].max_rt_latency[j][BURSTREAD] : 0,
            xpipes_ni_status[i].sent_packets[j][SINGLEWRITENONPOSTED] ? xpipes_ni_status[i].max_rt_latency[j][SINGLEWRITENONPOSTED] : 0,
            xpipes_ni_status[i].sent_packets[j][BURSTWRITENONPOSTED] ? xpipes_ni_status[i].max_rt_latency[j][BURSTWRITENONPOSTED] : 0);
          fprintf(fstat, "      ---------------------------------------------------------------------------\n");
        }
      }
    }
  }
  
  fprintf(fstat, "\n---------\n");
  
  for (i = 0; i < max_xpipes_registered_buffers; i ++)
  {
    if (xpipes_buffer_status[i].registered && xpipes_buffer_status[i].max_usage > 0)
    {
      fprintf(fstat, "\nxpipes output buffer %hu - %s:\n", i, xpipes_buffer_status[i].name.c_str());
      fprintf(fstat, "Out of %hu available locations: used min %hu, max %hu, avg %.2f\n",
        xpipes_buffer_status[i].locations, xpipes_buffer_status[i].min_usage,
        xpipes_buffer_status[i].max_usage,
        (float)xpipes_buffer_status[i].tot_usage / (float)(all_cores_exec ? all_cores_exec : 1));
      // x ? x : 1 is a hack to avoid the possibility of 0/0 divisions
    }
  }
  
  fprintf(fstat, "\n---------\n");

  for (i = 0; i < max_xpipes_registered_link_receivers; i ++)
  {
    if (xpipes_link_receiver_status[i].registered)
    {
      fprintf(fstat, "\nxpipes link receiver %hu - %s:\n", i, xpipes_link_receiver_status[i].name.c_str());
      for (j = 0; j < xpipes_link_receiver_status[i].ports; j ++)
      {
        fprintf(fstat, "  From input port %hu out of %hu: ACKed %llu flits, NACKed %llu flit sequences,\n",
          j, xpipes_link_receiver_status[i].ports,
          xpipes_link_receiver_status[i].acked[j],
          xpipes_link_receiver_status[i].nacked[j]);
        fprintf(fstat, "    effective link usage (ACKs/cycles) %3.2f%%\n",
          (double)((double)xpipes_link_receiver_status[i].acked[j] * 100.0 / (double)(all_cores_exec ? all_cores_exec : 1)));
        // x ? x : 1 is a hack to avoid the possibility of 0/0 divisions
      }
    }
  }
  
  fprintf(fstat, "\n");
}
#endif


///////////////////////////////////////////////////////////////////////////////
// printMasterResults - Dumps some information about master performance.
void Statistics::printMasterResults(uint ID)
{
  fprintf(fstat, "Bus Accesses                  = %lu (%lu SR, %lu SW, %lu BR, %lu BW: %lu R, %lu W)\n", master_c[ID].accesses,
            master_c[ID].accesses_sr, master_c[ID].accesses_sw, master_c[ID].accesses_br, master_c[ID].accesses_bw,
            master_c[ID].accesses_r, master_c[ID].accesses_w);
  if (master_c[ID].accesses_r)
  {
    fprintf(fstat, "Time (ns) to bus access (R)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_r, master_c[ID].accesses_r, master_c[ID].max_waittime_r,
              (double)master_c[ID].tot_waittime_r / (double)(master_c[ID].accesses_r),
              master_c[ID].min_waittime_r);
    fprintf(fstat, "Time (ns) to bus compl. (R)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_r, master_c[ID].accesses_r, master_c[ID].max_comptime_r,
              (double)master_c[ID].tot_comptime_r / (double)(master_c[ID].accesses_r),
              master_c[ID].min_comptime_r);
  }
  if (master_c[ID].accesses_w)
  {
    fprintf(fstat, "Time (ns) to bus access (W)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_w, master_c[ID].accesses_w, master_c[ID].max_waittime_w,
              (double)master_c[ID].tot_waittime_w / (double)(master_c[ID].accesses_w),
              master_c[ID].min_waittime_w);
    fprintf(fstat, "Time (ns) to bus compl. (W)   = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_w, master_c[ID].accesses_w, master_c[ID].max_comptime_w,
              (double)master_c[ID].tot_comptime_w / (double)(master_c[ID].accesses_w),
              master_c[ID].min_comptime_w);
  }
  if (master_c[ID].accesses_sr)
  {
    fprintf(fstat, "Time (ns) to bus access (SR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_sr, master_c[ID].accesses_sr, master_c[ID].max_waittime_sr,
              (double)master_c[ID].tot_waittime_sr / (double)(master_c[ID].accesses_sr),
              master_c[ID].min_waittime_sr);
    fprintf(fstat, "Time (ns) to bus compl. (SR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_sr, master_c[ID].accesses_sr, master_c[ID].max_comptime_sr,
              (double)master_c[ID].tot_comptime_sr / (double)(master_c[ID].accesses_sr),
              master_c[ID].min_comptime_sr);
  }
  if (master_c[ID].accesses_br)
  {
    fprintf(fstat, "Time (ns) to bus access (BR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_br, master_c[ID].accesses_br, master_c[ID].max_waittime_br,
              (double)master_c[ID].tot_waittime_br / (double)(master_c[ID].accesses_br),
              master_c[ID].min_waittime_br);
    fprintf(fstat, "Time (ns) to bus compl. (BR)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_br, master_c[ID].accesses_br, master_c[ID].max_comptime_br,
              (double)master_c[ID].tot_comptime_br / (double)(master_c[ID].accesses_br),
              master_c[ID].min_comptime_br);
  }
  if (master_c[ID].accesses_sw)
  {
    fprintf(fstat, "Time (ns) to bus access (SW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_sw, master_c[ID].accesses_sw, master_c[ID].max_waittime_sw,
              (double)master_c[ID].tot_waittime_sw / (double)(master_c[ID].accesses_sw),
              master_c[ID].min_waittime_sw);
    fprintf(fstat, "Time (ns) to bus compl. (SW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_sw, master_c[ID].accesses_sw, master_c[ID].max_comptime_sw,
              (double)master_c[ID].tot_comptime_sw / (double)(master_c[ID].accesses_sw),
              master_c[ID].min_comptime_sw);
  }
  if (master_c[ID].accesses_bw)
  {
    fprintf(fstat, "Time (ns) to bus access (BW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime_bw, master_c[ID].accesses_bw, master_c[ID].max_waittime_bw,
              (double)master_c[ID].tot_waittime_bw / (double)(master_c[ID].accesses_bw),
              master_c[ID].min_waittime_bw);
    fprintf(fstat, "Time (ns) to bus compl. (BW)  = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime_bw, master_c[ID].accesses_bw, master_c[ID].max_comptime_bw,
              (double)master_c[ID].tot_comptime_bw / (double)(master_c[ID].accesses_bw),
              master_c[ID].min_comptime_bw);
  }
  if (master_c[ID].accesses)
  {
    fprintf(fstat, "Time (ns) to bus access (tot) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_waittime, master_c[ID].accesses, master_c[ID].max_waittime,
              (double)master_c[ID].tot_waittime / (double)(master_c[ID].accesses),
              master_c[ID].min_waittime);
    fprintf(fstat, "Time (ns) to bus compl. (tot) = %lu over %lu accesses (max %lu, avg %.2f, min %lu)\n",
              master_c[ID].tot_comptime, master_c[ID].accesses, master_c[ID].max_comptime,
              (double)master_c[ID].tot_comptime / (double)(master_c[ID].accesses),
              master_c[ID].min_comptime);
    fprintf(fstat, "Wrapper overhead cycles       = %lu\n", 2 * master_c[ID].accesses);
    fprintf(fstat, "Total bus activity cycles     = %lu (bus completion + wrapper OH)\n",
              master_c[ID].tot_comptime + 2 * master_c[ID].accesses);
    fprintf(fstat, "\"Free\" bus accesses           = %lu (%.2f%% of %lu)\n",
              master_c[ID].free, (double)(master_c[ID].free * 100) / double(master_c[ID].accesses),
              master_c[ID].accesses);
  }
}


///////////////////////////////////////////////////////////////////////////////
// printSWARMCoreResults - Dumps some information about SWARM processor performance and the interaction with the interconnect.
void Statistics::printSWARMCoreResults(uint ID)
{ 
  /* If all is OK, all of the following prints should result in sequences of identical values
  #ifdef DEBUGGING
    fprintf(fstat, "Bus accesses: %Ld %Ld %lu %lu %lu\n", statistic_arm_end[ID].n_bus_req -
              statistic_arm_begin[ID].n_bus_req, statistic_mast_end[ID].accesses - statistic_mast_begin[ID].accesses,
              accesses[ID], c_miss[ID] + insidew[ID] + outsidew[ID] + non_cache[ID] - dmar[ID] - dmaw[ID],
              accesses_sr[ID] + accesses_sw[ID] + accesses_br[ID]);
    fprintf(fstat, "Bursts: %lu %Ld %Ld %lu %lu %lu\n", bursts[ID], statistic_arm_end[ID].bursts -
              statistic_arm_begin[ID].bursts, statistic_arm_end[ID].cache_miss - statistic_arm_begin[ID].cache_miss,
              c_miss[ID], refill[ID] * 1/5, accesses_br[ID]);
    fprintf(fstat, "Cycles to bus access: %lu %lu\n", current_tot_waittime[ID],
              current_tot_waittime_sr[ID] + current_tot_waittime_sw[ID] + current_tot_waittime_br[ID]);
    fprintf(fstat, "Cycles to bus completion: %lu %lu\n", current_tot_comptime[ID],
              current_tot_comptime_sr[ID] + current_tot_comptime_sw[ID] + current_tot_comptime_br[ID]);
    fprintf(fstat, "Cache hits: %lu %Ld\n", cached[ID],
              statistic_arm_end[ID].cache_hit - statistic_arm_begin[ID].cache_hit);
    fprintf(fstat, "Global cycles: %lu %Ld %lu\n", counter[ID], total_cycles_crit[ID],
              cached[ID] + c_miss[ID] + refill[ID] + insidew[ID] + outsidew[ID] + non_cache[ID] + hacked[ID] +
              internal[ID] + waiting[ID]);
    fprintf(fstat, "Internal cycles: %Ld %lu\n", statistic_arm_end[ID].real_cycles -
              statistic_arm_begin[ID].real_cycles, cached[ID] + c_miss[ID] + refill[ID] + insidew[ID] + outsidew[ID] +
              non_cache[ID] + hacked[ID] + internal[ID]);
    fprintf(fstat, "Cacheable reads: %lu %Ld\n", insider[ID] + outsider[ID],
              statistic_arm_end[ID].cache_hit - statistic_arm_begin[ID].cache_hit);
    fprintf(fstat, "Bus activity cycles: %lu %lu\n", current_tot_comptime[ID] + 2 * accesses[ID],
              waiting[ID] - dmarwait[ID] - dmawwait[ID] + accesses[ID] + 3 * c_miss[ID]);
    fprintf(fstat, "Total wait cycles: %lu %lu\n\n\n", waiting[ID],
              privaterwait[ID] + privatewwait[ID] + sharedrwait[ID] + sharedwwait[ID] + semaphorerwait[ID] +
              semaphorewwait[ID] + intwwait[ID] + dmarwait[ID] + dmawwait[ID] +
	      coreslaverwait[ID] + coreslavewwait[ID] + smartmemrwait[ID] + smartmemwwait[ID]);
  #endif*/

  fprintf(fstat, "-----------------\n");
  fprintf(fstat, "SWARM Processor %d\n", ID);
  fprintf(fstat, "-----------------\n");
  //fprintf(fstat, "Core cycles                = %Ld\n",
  //          statistic_arm_end[ID].core_cycles - statistic_arm_begin[ID].core_cycles);
  fprintf(fstat, "Direct Accesses               = %llu to DMA\n", core_c[ID].dmar + core_c[ID].dmaw);
  
  if (CURRENT_INTERC != XPIPES)
    printMasterResults(ID);

  fprintf(fstat, "Idle cycles                   = %llu\n\n", core_c[ID].core_idle);

  fprintf(fstat, "+==================+=======================+\n");
  fprintf(fstat, "|                  |      Current setup    |\n");
  fprintf(fstat, "|                  |    Ext Acc     Cycles |\n");
  fprintf(fstat, "+==================+=======================+\n");
  fprintf(fstat, "| Private reads    |%10llu* %10llu |\n", core_c[ID].dc_miss + core_c[ID].ic_miss,
            core_c[ID].dc_hit + core_c[ID].ic_hit + core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].refill);
  fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].privaterwait);
  fprintf(fstat, "| Private writes   | %10llu %10llu |\n", core_c[ID].privatew, core_c[ID].privatew);
  fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].privatewwait);
  if (SCRATCH)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Scratch reads    |            %10llu |\n", core_c[ID].scratchr);
    fprintf(fstat, "| Scratch writes   |            %10llu |\n", core_c[ID].scratchw);
  }
  if (ISCRATCH)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| I-Scratch reads  |            %10llu |\n", core_c[ID].iscratchr);
    fprintf(fstat, "| I-Scratch writes |            %10llu |\n", core_c[ID].iscratchw);
  }
  if (CORESLAVE)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Queue reads      |            %10llu |\n", core_c[ID].queuer);
    fprintf(fstat, "| Queue writes     |            %10llu |\n", core_c[ID].queuew);
  }
  if (N_SHARED > 0)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Shared reads     | %10llu %10llu |\n", core_c[ID].sharedr, core_c[ID].sharedr * 2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].sharedrwait);
    fprintf(fstat, "| Shared writes    | %10llu %10llu |\n", core_c[ID].sharedw, core_c[ID].sharedw);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].sharedwwait);
  }
  if (N_SEMAPHORE > 0)
  {
    fprintf(fstat, "+------------------+-----------------------+\n");
    fprintf(fstat, "| Semaphore reads  | %10llu %10llu |\n", core_c[ID].semaphorer, core_c[ID].semaphorer*2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].semaphorerwait);
    fprintf(fstat, "| Semaphore writes | %10llu %10llu |\n", core_c[ID].semaphorew, core_c[ID].semaphorew);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].semaphorewwait);
  }
  if (N_INTERRUPT > 0)
  {
    fprintf(fstat, "+------------------+-----------------------+\n");
    fprintf(fstat, "| Interrupt writes | %10llu %10llu |\n", core_c[ID].intw, core_c[ID].intw);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].interruptwwait);
  }
  if (CORESLAVE)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Coreslave reads  | %10llu %10llu |\n", core_c[ID].coreslaver, core_c[ID].coreslaver * 2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].coreslaverwait);
    fprintf(fstat, "| Coreslave writes | %10llu %10llu |\n", core_c[ID].coreslavew, core_c[ID].coreslavew);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].coreslavewwait);
  }
  if (SMARTMEM)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Smartmemslave r  | %10llu %10llu |\n", core_c[ID].smartmemr, core_c[ID].smartmemr * 2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].smartmemrwait);
    fprintf(fstat, "| Smartmemslave w  | %10llu %10llu |\n", core_c[ID].smartmemw, core_c[ID].smartmemw);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].smartmemwwait);
    fprintf(fstat, "+------------------+-----------------------+\n");
  }
  if (DMA)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| DMA reads        | %10llu %10llu |\n", core_c[ID].dmar, core_c[ID].dmar * 2);
    fprintf(fstat, "| Wrapper waits    |            %10llu |\n", core_c[ID].dmarwait);
    fprintf(fstat, "| DMA writes       | %10llu %10llu |\n", core_c[ID].dmaw, core_c[ID].dmaw);
    fprintf(fstat, "| Wrapper waits    |            %10llu |\n", core_c[ID].dmawwait);
  }
  if (N_FFT > 0)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| FFT reads        | %10llu %10llu |\n", core_c[ID].fftr, core_c[ID].fftr * 2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].fftrwait);
    fprintf(fstat, "| FFT writes       | %10llu %10llu |\n", core_c[ID].fftw, core_c[ID].fftw);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].fftwwait);
  }
  if (FREQSCALINGDEVICE)
  {
    fprintf(fstat, "+==================+=======================+\n");
    fprintf(fstat, "| Frequency reads  | %10llu %10llu |\n", core_c[ID].freqr, core_c[ID].freqr * 2);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].freqrwait);
    fprintf(fstat, "| Frequency writes | %10llu %10llu |\n", core_c[ID].freqw, core_c[ID].freqw);
    fprintf(fstat, "| Bus+Wrapper waits|            %10llu |\n", core_c[ID].freqwwait);
  }
  fprintf(fstat, "+==================+=======================+\n");
  fprintf(fstat, "| Internal reads   |            %10llu |\n", core_c[ID].internalr);
  fprintf(fstat, "| Internal writes  |            %10llu |\n", core_c[ID].internalw);
  fprintf(fstat, "+==================+=======================+\n");
  fprintf(fstat, "| SWARM total      | %10llu %10llu |\n", 
            core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].privatew + core_c[ID].non_cache_acc,
            core_c[ID].dc_hit + core_c[ID].ic_hit + core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].refill +
            core_c[ID].privatew + core_c[ID].scratchr + core_c[ID].scratchw +
            core_c[ID].iscratchr + core_c[ID].iscratchw +
            core_c[ID].queuer + core_c[ID].queuew + core_c[ID].non_cache_cyc + core_c[ID].internal);            
  fprintf(fstat, "| Wait cycles total|            %10llu |\n", core_c[ID].waiting);
  fprintf(fstat, "| Pipeline stalls  |            %10llu |\n", core_c[ID].stalled);  
  fprintf(fstat, "+------------------+-----------------------+\n");
  fprintf(fstat, "| Overall total    | %10llu %10llu |\n",
            core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].scratchw + core_c[ID].iscratchw + core_c[ID].queuew + core_c[ID].privatew +
            core_c[ID].non_cache_acc, core_c[ID].counter + core_c[ID].waiting);
  fprintf(fstat, "+==================+=======================+\n\n");

  fprintf(fstat, "---Cache performance---\n");
  fprintf(fstat, "* Read bursts due to %llu cache misses out of %llu cacheable reads. Misses\n",
            core_c[ID].dc_miss + core_c[ID].ic_miss, core_c[ID].privater);

  if (CACHE_WRITE_POLICY == WT) {
    fprintf(fstat, "also cost %llu int cycles to refill. All writes were write-through.\n",
            core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].refill);
    fprintf(fstat, "Reads are done by reading tag and data in parallel (so data reads happen\n");
    fprintf(fstat, "even on cache misses); write-throughs always involve a tag read followed,\n");
    fprintf(fstat, "only in case of hit, by a data word write.\n");
  } else {
    fprintf(fstat, "also cost %llu int cycles to refill.\n",
            core_c[ID].dc_miss + core_c[ID].ic_miss + core_c[ID].refill);
    fprintf(fstat, "Reads are done by reading tag and data in parallel (so data reads happen\n");
    fprintf(fstat, "even on cache misses).\n");
  }    
  fprintf(fstat, "D-Cache: %llu read hits; %llu read misses (%llu single-word refills)\n",
            core_c[ID].dc_hit, core_c[ID].dc_miss, core_c[ID].dc_miss * 4);
  fprintf(fstat, "D-Cache: %llu write-through hits; %llu write-through misses\n", core_c[ID].dc_w, core_c[ID].dc_wm);
  fprintf(fstat, "D-Cache total: %llu tag reads, %llu tag writes\n",
            core_c[ID].dc_hit + core_c[ID].dc_miss + core_c[ID].dc_wm + core_c[ID].dc_w,
            core_c[ID].dc_miss);
  fprintf(fstat, "               %llu data reads, %llu data line writes, %llu data word writes\n",
            core_c[ID].dc_hit + core_c[ID].dc_miss, core_c[ID].dc_miss, core_c[ID].dc_w);
  fprintf(fstat, "D-Cache Miss Rate: %3.2f%%\n",
            (double)core_c[ID].dc_miss * 100.0 / ((double)(core_c[ID].dc_hit - core_c[ID].dc_miss) ? (double)(core_c[ID].dc_hit - core_c[ID].dc_miss) : 1));
  fprintf(fstat, "I-Cache: %llu read hits; %llu read misses (%llu single-word refills)\n",
            core_c[ID].ic_hit, core_c[ID].ic_miss, core_c[ID].ic_miss * 4);
  fprintf(fstat, "I-Cache: %llu write-through hits; %llu write-through misses\n", core_c[ID].ic_w, core_c[ID].ic_wm);
  fprintf(fstat, "I-Cache total: %llu tag reads, %llu tag writes\n",
            core_c[ID].ic_hit + core_c[ID].ic_miss + core_c[ID].ic_wm + core_c[ID].ic_w,
            core_c[ID].ic_miss);
  fprintf(fstat, "               %llu data reads, %llu data line writes, %llu data word writes\n",
            core_c[ID].ic_hit + core_c[ID].ic_miss, core_c[ID].ic_miss, core_c[ID].ic_w);
  fprintf(fstat, "I-Cache Miss Rate: %3.2f%%\n",
            (double)core_c[ID].ic_miss * 100.0 / ((double)(core_c[ID].ic_hit - core_c[ID].ic_miss) ? (double)(core_c[ID].ic_hit - core_c[ID].ic_miss) : 1));
  // x ? x : 1 is a hack to avoid the possibility of 0/0 divisions   
  
  // If a partitioned scratchpad was requested, analyze it in more detail
  if (SPCHECK)
  {
    fprintf(fstat, "---Scratchpad memory performance---\n");

    if (core_c[ID].scratchrangenumber == 0)
    {
      fprintf(fstat, "Partitioned SPM analysis was requested, but no valid SPM partitioning was\n");
      fprintf(fstat, "provided. SPM was managed as a contiguous SPM space.\n");
    }
    else
    {
      uint32_t begin, end;
      unsigned short int loop;
    
      fprintf(fstat, "Intercepted %llu read and %llu write accesses (total %llu out of %llu)\n",
                core_c[ID].scratchr, core_c[ID].scratchw, core_c[ID].scratchr + core_c[ID].scratchw,
                core_c[ID].scratchr + core_c[ID].privater + core_c[ID].scratchw + core_c[ID].privatew);
      for (loop = 0; loop < core_c[ID].scratchrangenumber; loop++)
      {
        addresser->ReturnRangeBounds(ID, loop, &begin, &end);
        fprintf(fstat, "Range %02hu (0x%08x - 0x%08x) (%8u B) catches %9llu accesses\n", loop + 1,
                  begin, end, (unsigned int)(end - begin + 1),
                  core_c[ID].scratchrrange[loop] + core_c[ID].scratchwrange[loop]);
      }
    }
  }

  
  if (DMA)
  {
    /* If all is OK, all of the following prints should result in sequences of identical values
    #ifdef DEBUGGING
      fprintf(fstat, "\nCycles to bus access: %lu %lu\n", current_tot_waittime[ID+N_CORES],
                current_tot_waittime_sr[ID+N_CORES] + current_tot_waittime_sw[ID+N_CORES] +
                current_tot_waittime_br[ID+N_CORES] + current_tot_waittime_bw[ID+N_CORES]);
      fprintf(fstat, "Cycles to bus completion: %lu %lu\n", current_tot_comptime[ID+N_CORES],
                current_tot_comptime_sr[ID+N_CORES] + current_tot_comptime_sw[ID+N_CORES] +
                current_tot_comptime_br[ID+N_CORES] + current_tot_comptime_bw[ID+N_CORES]);
    #endif*/
    fprintf(fstat, "\n----------------\n");
    fprintf(fstat, "DMA Controller %d\n", ID);
    fprintf(fstat, "----------------\n");

    printMasterResults(ID + N_CORES);

    fprintf(fstat, "\n");
  }
}

///////////////////////////////////////////////////////////////////////////////
// printLXCoreResults - Dumps some information about LX processor interaction with the interconnect.
void Statistics::printLXCoreResults(uint ID)
{
  fprintf(fstat, "-----------------\n");
  fprintf(fstat, "LX Processor %d\n", ID);
  fprintf(fstat, "-----------------\n");  
  if (CURRENT_INTERC != XPIPES)
    printMasterResults(ID);
}

///////////////////////////////////////////////////////////////////////////////
// printSmartmem - Dumps some information about the smartmem dma.
void Statistics::printSmartmem(uint ID)
{
  fprintf(fstat, "\n------------------------------\n");
  fprintf(fstat, "Smart Memory %d DMA Controller\n", ID - N_CORES - (DMA*N_CORES));
  fprintf(fstat, "------------------------------\n");

  printMasterResults(ID);

  fprintf(fstat, "\n--------------\n");
  fprintf(fstat, "Smart Memory %d\n", ID - N_CORES - (DMA*N_CORES));
  fprintf(fstat, "--------------\n");
  fprintf(fstat, "Read accesses   = %10lu\n", ext_smartmemr[ID- N_CORES - (DMA*N_CORES)]);
  fprintf(fstat, "Write accesses  = %10lu\n\n", ext_smartmemw[ID- N_CORES - (DMA*N_CORES)]);
}


///////////////////////////////////////////////////////////////////////////////
// printExtscratch - Dumps some information about the external scratch accesses.
void Statistics::printExtscratch(uint ID)
{
  fprintf(fstat, "\n-------------------------\n");
  fprintf(fstat, "External Scratch Memory %d\n", ID);
  fprintf(fstat, "-------------------------\n");
  fprintf(fstat, "Read accesses   = %10lu\n", ext_scratchr[ID]);
  fprintf(fstat, "Write accesses  = %10lu\n\n", ext_scratchw[ID]);
}


///////////////////////////////////////////////////////////////////////////////
// sync - Synchronizes statistics to platform clock.
void Statistics::sync()
{
  if (core_measuring > 0)
    one_core_exec ++;

  if (core_measuring == N_CORES)
  {
    all_cores_exec++;

    for (uint i = 0; i < N_MASTERS; i ++)
      if (master_s[i].is_accessing)
      {
        bus_busy ++;
        break;
      }

    for (uint i = 0; i < N_MASTERS; i ++)
      if (master_s[i].data_on_bus > 0)
      {
        transferring += master_s[i].data_on_bus;
        master_s[i].data_on_bus = 0;
      }
  }
}









///////////////////////////////////////////////////////////////////////////////
// synchronizer::loop - Periodically sends a tick to Statistics::sync.
void synchronizer::loop()
{
  while(true)
  {
    statobject->sync();
    wait();
  }
}
