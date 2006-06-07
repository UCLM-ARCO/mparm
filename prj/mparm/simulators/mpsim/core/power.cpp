///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         power.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Routines to track power consumption
//
///////////////////////////////////////////////////////////////////////////////

#include "power.h"

power *power_object;

// Function to get the power coefficients for the AMBA AHB model.
// Based on M. Caldari, A. Bona, V. Zaccaria, R. Zafalon -
// "High-level power characterization of the AMBA Bus interconnect"
double AMBAHB_EnergyParameter(PAR_TYPE type, unsigned short int n_masters, unsigned short int n_slaves)
{
 switch (type)
  {
    case BASE_TYP : return (20 * (0.1325043 + 0.005015675 * (n_masters + n_slaves)));
                    // The coefficient to the right represents the power consumed in a 20 ns cycle
                    break;
                    
    case BASE_MIN : return (20 * (0.0463282 + 0.008753513 * (n_masters + n_slaves)));
                    // The coefficient to the right represents the power consumed in a 20 ns cycle
                    break;
    
    case HT_TYP   : return ((0.7277189 + (-0.008409087) * (n_masters + n_slaves)) / 20.0);
                    // The coefficient to the left represents the energy consumed in a cycle, times a 20 ns period
                    break;
                    
    case HT_MIN   : return ((0.0366616 + 0.00644085 * (n_masters + n_slaves)) / 20.0);
                    // The coefficient to the left represents the energy consumed in a cycle, times a 20 ns period
                    break;
    
    case HM_TYP   : return (-0.1165328 + 0.168027425 * (n_masters + n_slaves));
                    // The coefficient to the right represents the energy consumed in a cycle
                    break;
                    
    case HM_MIN   : return (0.1752122 + 0.008798263 * (n_masters + n_slaves));
                    // The coefficient to the right represents the energy consumed in a cycle
                    break;
    
    case HR_TYP   : return (0.2865882 + 0.003996825 * (n_masters + n_slaves));
                    // The coefficient to the right represents the energy consumed in a cycle
                    break;
                    
    case HR_MIN   : return (0.0288205 + 0.00243345 * (n_masters + n_slaves));
                    // The coefficient to the right represents the energy consumed in a cycle
                    break;
    
    default       : printf("Fatal Error: Unexpected error in AMBAHB_EnergyParameter()!\n");
                    exit(1);
   
  }
};

/*
GP_DPHD_32x32m4    (128 B) (cycle time = 1.85 ns)
GP_DPHD_64x32m4    (256 B) (cycle time = 1.86 ns)
GP_DPHD_128x32m4   (512 B) (cycle time = 1.89 ns)
GP_DPHD_256x32m8   (1 kB)  (cycle time = 2.30 ns)
GP_DPHD_512x32m4   (2 kB)  (cycle time = 2.61 ns)
GP_DPHD_1024x32m8  (4 kB)  (cycle time = 2.85 ns)
GP_DPHD_2048x32m16 (8 kB)  (cycle time = 2.97 ns)
GP_DPHD_4096x32m16 (16 kB) (cycle time = 3.67 ns)
*/
#define mem_32bit_dualport_read_energy_pJ(size) \
        (                             \
         size == 128    ? 29.93  :    \
         size == 256    ? 30.42  :    \
         size == 512    ? 31.39  :    \
         size == 1024   ? 38.00  :    \
         size == 2048   ? 38.00  :    \
         size == 4096   ? 46.00  :    \
         size == 8192   ? 61.00  :    \
         size == 16384  ? 83.00  : -1 \
        )
#define mem_32bit_dualport_write_energy_pJ(size) \
        (                             \
         size == 128    ? 36.23  :    \
         size == 256    ? 37.15  :    \
         size == 512    ? 38.39  :    \
         size == 1024   ? 47.00  :    \
         size == 2048   ? 54.00  :    \
         size == 4096   ? 66.00  :    \
         size == 8192   ? 87.00  :    \
         size == 16384  ? 118.00 : -1 \
        )


/*
GP_SPSMALL_8x128m1_L   (128 B) (cycle time = 2.67 ns)
GP_SPSMALL_16x128m1_L  (256 B) (cycle time = 2.69 ns)
GP_SPSMALL_32x128m1_L  (512 B) (cycle time = 2.80 ns)
GP_SPSMALL_64x128m1_L  (1 kB)  (cycle time = 3.02 ns)
GP_SPSMALL_128x128m1_L (2 kB)  (cycle time = 3.49 ns)
GP_SPSMALL_256x128m1_L (4 kB)  (cycle time = 4.43 ns)
GP_SPHD_512x128m4      (8 kB)  (cycle time = 2.97 ns)
GP_SPHD_1024x128m4     (16 kB) (cycle time = 3.38 ns)
*/
#define mem_128bit_singleport_read_energy_pJ(size) \
        (                             \
         size == 128    ? 36.94  :    \
         size == 256    ? 37.72  :    \
         size == 512    ? 39.69  :    \
         size == 1024   ? 44.12  :    \
         size == 2048   ? 53.55  :    \
         size == 4096   ? 73.48  :    \
         size == 8192   ? 114.00 :    \
         size == 16384  ? 130.00 : -1 \
        )
#define mem_128bit_singleport_write_energy_pJ(size) \
        (                             \
         size == 128    ? 58.16  :    \
         size == 256    ? 59.03  :    \
         size == 512    ? 61.01  :    \
         size == 1024   ? 64.93  :    \
         size == 2048   ? 72.99  :    \
         size == 4096   ? 90.24  :    \
         size == 8192   ? 158.00 :    \
         size == 16384  ? 196.00 : -1 \
        )

/*
LP_SPSMALL_4x18m1_U   (cycle time = 3.80 ns)
LP_SPSMALL_8x17m1_U   (cycle time = 3.96 ns)
LP_SPSMALL_16x16m1_U  (cycle time = 4.32 ns)
GP_SPSMALL_32x15m1_U  (cycle time = 3.73 ns)
GP_SPSMALL_64x14m1_L  (cycle time = 2.53 ns)
GP_SPSMALL_128x13m1_L (cycle time = 3.20 ns)
GP_SPSMALL_256x12m2_L (cycle time = 3.99 ns)
GP_SPHD_512x11m8      (cycle time = 2.44 ns)
LP_SPHS_1024x10m4_U   (cycle time = 4.03 ns)
*/
#define tag_read_energy_pJ(rows, bits)             \
        (                                          \
         (rows == 4    && bits == 18) ? 4.96  :    \
         (rows == 8    && bits == 17) ? 4.88  :    \
         (rows == 16   && bits == 16) ? 4.93  :    \
         (rows == 32   && bits == 15) ? 5.16  :    \
         (rows == 64   && bits == 14) ? 5.86  :    \
         (rows == 128  && bits == 13) ? 7.06  :    \
         (rows == 256  && bits == 12) ? 9.44  :    \
         (rows == 512  && bits == 11) ? 18.93 :    \
         (rows == 1024 && bits == 10) ? 47.35 : -1 \
        )
#define tag_write_energy_pJ(rows, bits)            \
        (                                          \
         (rows == 4    && bits == 18) ? 6.14  :    \
         (rows == 8    && bits == 17) ? 5.98  :    \
         (rows == 16   && bits == 16) ? 5.94  :    \
         (rows == 32   && bits == 15) ? 6.13  :    \
         (rows == 64   && bits == 14) ? 6.83  :    \
         (rows == 128  && bits == 13) ? 7.91  :    \
         (rows == 256  && bits == 12) ? 10.66 :    \
         (rows == 512  && bits == 11) ? 26.86 :    \
         (rows == 1024 && bits == 10) ? 52.94 : -1 \
        )
        
/*
GP_SPUHD_2048x32m8_L         (8 kB)   (cycle time = 2.90 ns)
GP_SPUHD_4096x32m8_L         (16 kB)  (cycle time = 3.95 ns)
GP_SPUHD_8192x32m16_L        (32 kB)  (cycle time = 3.90 ns)
GP_SPLARGEUHD_16384x32m32_L  (64 kB)  (cycle time = 5.79 ns)
GP_SPLARGEUHD_32768x32m64_RL (128 kB) (cycle time = 7.01 ns)
GP_SPLARGEUHD_65536x32m64_RL (256 kB) (cycle time = 8.19 ns)
*/
#define mem_32bit_singleport_read_energy_pJ(size) \
        (                              \
         size <= 8192    ? 55.10  :    \
         size == 16384   ? 63.85  :    \
         size == 32768   ? 91.49  :    \
         size == 65536   ? 116.80 :    \
         size == 131072  ? 129.25 :    \
         size == 262144  ? 148.34 : -1 \
        )
#define mem_32bit_singleport_write_energy_pJ(size) \
        (                              \
         size <= 8192    ? 62.57  :    \
         size == 16384   ? 77.74  :    \
         size == 32768   ? 109.39 :    \
         size == 65536   ? 148.42 :    \
         size == 131072  ? 189.21 :    \
         size == 262144  ? 225.61 : -1 \
        )
//FIXME <= : temporary hack to allow int slave power models (just 336 bytes large...)

//#define TAG_POWER_READ(rows, bits)    (10.347 + 0.01077*rows + 2.8846*bits)
//#define TAG_POWER_WRITE(rows, bits)   (6.2581 + 0.01315*rows + 3.62316*bits)
//LOW POWER SPHS FROM 2K=>4K 
//#define TAG_POWER_READ(rows, bits)    (9.00746 + 0.0105061*rows + 2.768071*bits) //feangio - removed 28 04 2004
//#define TAG_POWER_WRITE(rows, bits)   (13.3703 + 0.0144185*rows + 2.737643*bits) //feangio - removed 28 04 2004
#define TAG_POWER_READ(rows, bits)    tag_read_energy_pJ(rows, bits)
#define TAG_POWER_WRITE(rows, bits)   tag_write_energy_pJ(rows, bits)
//#define CACHE_DATA_READ(size)         (101.1964074 + 0.0029291*size)
//#define CACHE_DATA_WRITE(size)        (121.1531395 + 0.0038397*size)
//Read energy:  119.72 + 0.003712636*size uW/MHz
//Write energy: 140.05 + 0.00416585*size  uW/MHz
//GENERAL PURPOSE SPSMALL MEMORY FROM 1KB==>2KB
//#define CACHE_DATA_READ(size)         (36.88 + 0.0094238*size) //feangio - removed 28 04 2004
//#define CACHE_DATA_WRITE(size)        (59.99 + 0.0083350*size) //feangio - removed 28 04 2004
#define CACHE_DATA_READ(size)         mem_128bit_singleport_read_energy_pJ(size)
#define CACHE_DATA_WRITE(size)        mem_128bit_singleport_write_energy_pJ(size)
//FIXME DMA: waiting for LUT-style models
#define DMA_READ(size)         (28.27153362 + 0.0040929*size)
#define DMA_WRITE(size)        (34.53540284 + 0.0081998*size)
//(119.72 + 0.003712636*size)
//(140.05 + 0.00416585*size)
//#define RAM_READ(size)         (102.38571 + 0.0001753*size)  //feangio - removed 28 04 2004
//#define RAM_WRITE(size)        (120.38714 + 0.00040139*size) //feangio - removed 28 04 2004
#define RAM_READ(size)                mem_32bit_singleport_read_energy_pJ(size)
#define RAM_WRITE(size)               mem_32bit_singleport_write_energy_pJ(size)

//GENERAL PURPOSE DPHD MEMORY FROM 1KB==>16KB
//#define SCRATCH_READ(size)	(34.98127003 + 0.0029663*size) //feangio - removed 28 04 2004
//#define SCRATCH_WRITE(size)	(50.03353035 + 0.0042048*size) //feangio - removed 28 04 2004
#define SCRATCH_READ(size)            mem_32bit_dualport_read_energy_pJ(size)
#define SCRATCH_WRITE(size)           mem_32bit_dualport_write_energy_pJ(size)


#ifdef RAM_COUNTER_WORKAROUND
ram_access *ram_access_object;
cache_access_counter *cache_access_counter_object;
#endif


/**********************************************************************
 *                                                                    *
 * Auxiliary functions                                                *
 *                                                                    *
 **********************************************************************/

#define ASSOCIATIVE_CACHE       4535
#define DIRECT_CACHE            6587
#define SET_ASSOCIATIVE_CACHE   6325


/* Not correct if a==0, but who wants LN2(0)? */
/* If not a power of two, rounds to the power of two immediately above */
inline
int LN2(long long int a)
{
  int k;
  if ((a)&(a-1)) k=0;
  else k=-1;
  for(;a;k++,a>>=1);
  return k;
}


inline
int cache_lines(int c_size, int cache_type, int n_ways)
{
  switch (cache_type) {
    case ASSOCIATIVE_CACHE:
    case DIRECT_CACHE:
      return c_size/16;
    case SET_ASSOCIATIVE_CACHE:
      return c_size/(16*n_ways);
    default:
      return 0;
  }
}


inline
int tag_bits(int c_size, unsigned long int ram_size, int cache_type, int n_ways)
{
  switch (cache_type) {
    case ASSOCIATIVE_CACHE:
      return LN2(ram_size)-4;
    case DIRECT_CACHE:
      return LN2(ram_size/c_size);
    case SET_ASSOCIATIVE_CACHE:
      return LN2(n_ways*ram_size/c_size);
    default:
      return 0;
  }
}




/**********************************************************************
 *                                                                    *
 * Energy evaluation functions                                        *
 *                                                                    *
 **********************************************************************/

// Estimation for the voltage scaling of an ARM core
// Returns the SQUARE of the voltage scaling of a core
// depending on current desired frequency; the number is
// expressed as relative to the maximum operating voltage and
// frequency
inline float ARMVoltageScaling(unsigned short int divider)
{
  //FIXME!!! assumes "ns" as the time unit...
  float clock_period = CLOCKPERIOD * divider;
  
  // Data taken from:
  // "Dynamic voltage scaling on a low-power microprocessor"
  // Johan Pouwelse, Koen Langendoen, Henk Sips
  // Proceedings of the 7th annual international conference on Mobile computing and networking, July 2001
  if (clock_period < 5.0)         // According to ST datasheets, the ARM core
  {                               // we have allows 200 MHz @ max voltage
    printf("Fatal Error: Unknown voltage scaling parameters for ARM cores above 200 MHz!\n");
    exit(1);
  }
  else if (clock_period < 11.0)   // Here, frequency scales by about 200 MHz/V
    return ((0.16667 + (4.16667 / clock_period)) * (0.16667 + (4.16667 / clock_period)));
  else                            // Below about 90 MHz, no more voltage scaling is allowed
    return 0.3;
  
  // ------------------------------------------------------------
  // LEGACY VOLTAGE VALUES
  // KEEP FOR COMPATIBILITY WITH PAST PAPERS
  // ------------------------------------------------------------
  // Voltage scaling:
  // 1.00 @ 200 MHz (maximum operating voltage and frequency)
  // 0.61 @ 100 MHz
  // 0.55 @  66 MHz
  // 0.47 @  50 MHz
  // 0.44 @  40 MHz and below
  // Rounds to the higher voltage step
  // if (clock_period < 5)
  // {
  //   printf("Fatal Error: Unknown voltage scaling parameters for ARM cores above 200 MHz!\n");
  //   exit(1);
  // }
  // else if (clock_period < 10)
  //   return 1.00;
  // else if (clock_period < 15)
  //   return (0.61 * 0.61);
  // else if (clock_period < 20)
  //   return (0.55 * 0.55);
  // else if (clock_period < 25)
  //   return (0.47 * 0.47);
  // else
  //   return (0.44 * 0.44);
}

// Estimation for the energy consumption of an ARM core
// Called by Statistics::inspectSWARMAccess during an ARM cycle
double ARMEnergy(uint ID, int type, unsigned short int divider)
{
  // Data : running energy consumption C = 0.055 mW/MHz = 55 pJ @ 1.2V (HCMOS9_ULL)
  //        stalled energy consumption 0.66*C
  //        idle energy consumption    0.10*C
  // voltage_ratio: allows for easy computation of voltage scaling approaches.
  //                This optional parameter scales the resulting energy by its square:
  //                voltage_ratio = 0.5 (half the voltage) -> one quarter the energy
  static double C = 55.0;

  switch (type)
  {
    case STALLED:
      return 0.66 * C * ARMVoltageScaling(divider);
      break;
    case RUNNING:
      return C * ARMVoltageScaling(divider);
      break;
    case POWER_IDLE:
      return 0.1 * C * ARMVoltageScaling(divider);
    default:
      ASSERT(0);
      return 0.0;
  }
}

#ifdef AMBAAHBBUILD
// Estimation for the energy consumption of an AMBA AHB bus
// Called by Statistics::inspectAMBAAHBAccess during an AMBA AHB cycle
double AMBAAHBEnergy(uint bus_ID, AHB_bus_activity monitor, BUS_ENERGY_LEVEL type)
{
  // Remember something of the previous status to be able to provide incremental energies
  // FIXME hardcoding to 16 buses max
  static int hmaster_count[16];
  static int n_cycles[16];
  static bool first_cycle = true;

  double energy;

  // Initialize the bus status
  if (first_cycle)
  {
    first_cycle = false;
 
    for (int i = 0; i < N_BUSES; i++)
    {
      hmaster_count[i] = 0;
      n_cycles[i] = 1;
    }
  }

  switch (type)
  {
    case BUS_TYP:
      energy = AMBAHB_EnergyParameter(BASE_TYP, monitor.n_masters, monitor.n_slaves) +
               AMBAHB_EnergyParameter(HT_TYP,  monitor.n_masters, monitor.n_slaves) *
                   (
                     ((double)((hmaster_count[bus_ID] + (int)monitor.htrans) * (hmaster_count[bus_ID] + (int)monitor.htrans)) / (double)(n_cycles[bus_ID] + 1.0) ) -
                     ((double)(hmaster_count[bus_ID] * hmaster_count[bus_ID]) / (double)(n_cycles[bus_ID]))
                   ) +
               AMBAHB_EnergyParameter(HM_TYP,   monitor.n_masters, monitor.n_slaves) * (int)monitor.hmaster +
               AMBAHB_EnergyParameter(HR_TYP,   monitor.n_masters, monitor.n_slaves) * (int)monitor.hready;
      return energy;
      break;
    case BUS_MIN:
      energy = AMBAHB_EnergyParameter(BASE_MIN, monitor.n_masters, monitor.n_slaves) +
               AMBAHB_EnergyParameter(HT_MIN,  monitor.n_masters, monitor.n_slaves) *
                   (
                     ((double)((hmaster_count[bus_ID] + (int)monitor.htrans) * (hmaster_count[bus_ID] + (int)monitor.htrans)) / (double)(n_cycles[bus_ID] + 1.0) ) -
                     ((double)(hmaster_count[bus_ID] * hmaster_count[bus_ID]) / (double)(n_cycles[bus_ID]))
                   ) +
               AMBAHB_EnergyParameter(HM_MIN,   monitor.n_masters, monitor.n_slaves) * (int)monitor.hmaster +
               AMBAHB_EnergyParameter(HR_MIN,   monitor.n_masters, monitor.n_slaves) * (int)monitor.hready;
      return energy;
      break;
    case BUS_MAX:
      // FIXME counters are incremented only now. This relies on BUS_MAX to be called last every time
      // (see Statistics::inspectAMBAAHBAccess)
      hmaster_count[bus_ID] += (int) monitor.htrans;
      n_cycles[bus_ID]++;
      return 0.0;
      break;
    default:
      printf("Fatal Error: Unexpected error in AMBAHB_EnergyParameter()!\n");
      exit(1);
  }
}
#endif

// Estimation for the energy consumption of an AMBA AHB bridge
// Called by Statistics::inspectAMBAAHBBridge during an AMBA AHB bridge cycle
double AMBAAHBBridgeEnergy(unsigned short int bridge_ID, bool master_side, bool interact_fifo)
{
  // Coefficients: taken from the STBus ones, multiplied by 20 (characterization clock = 50 MHz or 20 ns)
  // and divided by 2 (here we're splitting the power consumption in M + S sides instead of having a unified coefficient)
//  static double base_coeff = 20.0*0.96296800/2.0;
  static double base_coeff = 20.0*0.096296800/2.0;
  static double traffic_coeff = 20.0*0.13570500/2.0;
  // The "master_side" information is currently unused, may be exploited in future extensions

  return (base_coeff + traffic_coeff * (int)interact_fifo);
}

// Estimation for the energy consumption of a RAM memory
double powerRAM(uint ID, uint size, uint word_size, uint access_type)
{
  double energy=0;
  
  if (size > 256*1024)
    size = 256*1024; /* FIXME: Now memories are instantiated with a fixed, huge 
                      size (12MB). 256KB is more realistic. In the future
                      memories will have a run-time configurable, smaller size */

  if(addresser->IsPrivate(ID) || addresser->IsLXPrivate(ID) || addresser->IsShared(ID))
  {
   // Invalid input sizes are reported as -1 pJ energy
   switch (access_type) {
    case READop:
      energy = RAM_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: RAM energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEop:
      energy = RAM_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: RAM energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case STALLop:
      energy = 0.0;
      break;
    case NOop:
      energy = RAM_LEAKAGE;
      break;  
    default:
      energy = 0.0;
      ASSERT(0);
  }
 } 

#ifdef RAM_COUNTER_WORKAROUND
  if (statobject->IsGlobalMeasuring())
    ram_access_object->update(access_type, ID);
#endif

  return energy;
}

// Estimation for the energy consumption of a DMA engine
double powerDMA(uint ID, uint size, uint word_size, uint access_type)
{
  double energy;

  // Invalid input sizes are reported as -1 pJ energy
  switch (access_type) {
    case READop:
      energy = DMA_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: DMA energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEop:
      energy = DMA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: DMA energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case STALLop:
      energy = 0.0;
      break;
    default:
      energy = 0.0;
      ASSERT(0);
  }

  return energy;
}

// Estimation for the energy consumption of a scratchpad memory
double powerSCRATCH(uint ID, uint size, uint word_size, uint access_type)
{
  double energy;

  // Invalid input sizes are reported as -1 pJ energy
  switch (access_type) {
    case READop:
      energy = SCRATCH_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: scratchpad energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEop:
      energy = SCRATCH_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: scratchpad energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case STALLop:
      energy = 0.0;
      break;
    default:
      energy = 0.0;
      ASSERT(0);
  }

  return energy;
}

// Estimation for the energy consumption of a cache memory belonging to core "ID"
// Called by CDirectCache::Read, CDirectCache::WriteLine or
// CDirectCache::WriteWord during a direct mapped cache access
double powerDirectCache(uint ID, uint ID2, uint size, uint access_type)
{
  int lines, t_bits;
  double energy;
  lines = cache_lines(size, DIRECT_CACHE, 0);
  t_bits = tag_bits(size, addresser->ReturnPrivateSize(), DIRECT_CACHE, 0); /* At this moment only the private mem is cacheable*/
  switch (access_type) {
    case READTAGRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITETAGRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case READDATA:
      energy = CACHE_DATA_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case UPDATELINE:
    case WRITEDATALINE:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEDATAWORD:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy /= 4;  /* a cache line is composed of 4 words */
      energy *= 1.1; /* Writing a single word costs a bit more than 1/4 */
      break;
    case READDIRTYRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case WRITEDIRTYRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case READBIT:
      energy = TAG_POWER_READ(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEBIT:
      energy = TAG_POWER_WRITE(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    default:
      energy = 0.0;
      ASSERT(0);
  }

#ifdef RAM_COUNTER_WORKAROUND
  if (ID<N_CORES) /* It can be a section of a set-associative cache */
    if (statobject->IsMeasuring(ID))
      cache_access_counter_object->update(access_type, ID, ID2);
#endif

  return energy;
}

// Estimation for the energy consumption of a cache memory belonging to core "ID"
// Called by CAssociativeCache::Read, CAssociativeCache::WriteLine or
// CAssociativeCache::WriteWord during an associative cache access
double powerAssocCache(uint ID, uint ID2, uint size, uint word_size, uint access_type)
{
  int lines, t_bits;
  double energy;
  lines = cache_lines(size, ASSOCIATIVE_CACHE, 0);
  t_bits = tag_bits(size, addresser->ReturnPrivateSize(), ASSOCIATIVE_CACHE, 0); /* At this moment only the private mem is cacheable*/
  switch (access_type) {
    case READTAGRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITETAGRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case READDATA:
      energy = CACHE_DATA_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case UPDATELINE:
    case WRITEDATALINE:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEDATAWORD:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy /= 4;  /* a cache line is composed of 4 words */
      energy *= 1.1; /* See powerDirectCache */
      break;
    case READDIRTYRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case WRITEDIRTYRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case READBIT:
      energy = TAG_POWER_READ(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEBIT:
      energy = TAG_POWER_WRITE(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    default:
      energy = 0.0;
      ASSERT(0);
  }

#ifdef RAM_COUNTER_WORKAROUND
  if (statobject->IsMeasuring(ID))
    cache_access_counter_object->update(access_type, ID, ID2);
#endif

  return energy;
}

// Estimation for the energy consumption of a cache memory belonging to core "ID"
// Called by CSetAssociativeCache::Read, CSetAssociativeCache::WriteLine or
// CSetAssociativeCache::WriteWord during a set associative cache access
// This kind of cache is composed of a vector of direct-mapped caches: each of
// them is separately computed, then this function gets called to get the grand total
double powerSetAssocCache(uint ID, uint ID2, uint size, uint nWay, uint access_type)
{
  int lines, t_bits;
  double energy;
  lines = cache_lines(size, SET_ASSOCIATIVE_CACHE, nWay);
  t_bits = tag_bits(size, addresser->ReturnPrivateSize(), SET_ASSOCIATIVE_CACHE, nWay); /* At this moment only the private mem is cacheable*/
  switch (access_type) {
    case READTAGRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy *= nWay;
      break;
    case WRITETAGRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy *= nWay;
      break;
    case READDATA:
      energy = CACHE_DATA_READ(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case UPDATELINE:
    case WRITEDATALINE:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEDATAWORD:
      energy = CACHE_DATA_WRITE(size);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy /= 4;  /* a cache line is composed of 4 words */
      energy *= 1.1; /* See powerDirectCache */
      break;
    case READDIRTYRAM:
      energy = TAG_POWER_READ(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case WRITEDIRTYRAM:
      energy = TAG_POWER_WRITE(lines, t_bits);
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      energy = (energy/t_bits)*DCACHE_DIRTY_BITS;
      break;
    case READBIT:
      energy = TAG_POWER_READ(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    case WRITEBIT:
      energy = TAG_POWER_WRITE(lines, t_bits) * 0.03;
      if (energy < 0)
      {
        printf("Fatal error: cache energy models invoked with invalid/unknown size %u!\n", size);
        exit(1);
      }
      break;
    default:
      energy = 0.0;
      ASSERT(0);
  }

#ifdef RAM_COUNTER_WORKAROUND
  if (statobject->IsMeasuring(ID))
    cache_access_counter_object->update(access_type, ID, ID2);
#endif

  return energy;
}




typedef struct FINAL_POWER_HANDLERtag {
  void (*handler)(void*);
  void *argument;
} FINAL_POWER_HANDLER;

std::vector<FINAL_POWER_HANDLER> handlers;


void install_final_power_handler(void (*func)(void*), void *arg)
{
  FINAL_POWER_HANDLER h;
  h.handler = func;
  h.argument = arg;
  handlers.push_back(h);
}


void finalize_power()
{
  for(unsigned int i = 0; i < handlers.size(); i++)
    handlers[i].handler(handlers[i].argument);
}

