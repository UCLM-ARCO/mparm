///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         power.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Routines to track power consumption
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __POWER_H__
#define __POWER_H__

#include <systemc.h>
#include <vector>
#include "config.h"
#include "globals.h"
#include "debug.h"
#include "stats.h"


/* Semirandom values to avoid random collisions and to detect incorrect */
/* assignments                                                          */
#define STALLED 0x10DBB7CD
#define RUNNING 0x56B9AF11
#define POWER_IDLE    0x3EEBB986

#define READop  0xD3AC0116
#define WRITEop 0x35B5DC28
#define STALLop 0x4E8437DB
#define NOop    0x4E8437FF

#define READTAGRAM     0xA7A7AE9A
#define WRITETAGRAM    0x35749DFE
#define READDATA       0xE3CD4717
#define UPDATELINE     0xBC865342
#define WRITEDATALINE  0x09A57BA3
#define WRITEDATAWORD  0xE1220F64
#define READDIRTYRAM   0x87168122
#define WRITEDIRTYRAM  0x21342343
#define READBIT        0x2D4A53B3
#define WRITEBIT       0x34C54D34

//Poggiali POWER
/*Account for 1.5 mW for leakage when memory is not accessed.*/
#define RAM_LEAKAGE (CLOCKPERIOD*1.5)

/*************************************************************************/
/*                            Basic classes                              */
/*************************************************************************/

class Bdouble {
  private:
    double value;
    uint64 last_modified;  /* the cycle when the object was modified */

  public:
    Bdouble(double a=0.0){value=a; last_modified=0;}

    Bdouble& operator = (double a) {
      value = a;
      last_modified = (uint64)(sc_simulation_time());
      return *this;
    }

    Bdouble& operator += (double a) {
      value+=a;
      last_modified = (uint64) (sc_simulation_time());
      return *this;
    }

    void reset() {
             /* current cycle */
      uint64 curr = (uint64)(sc_simulation_time());
      ASSERT(curr >= last_modified);  /* going back in time??? */
                                      /* something is wrong    */
      if ( curr > last_modified ) value=0.0;
    }

        /* When we are using a vector this is more efficient, for it */
        /* avoid repeated calculation of curr_time                   */
    void reset(uint64 curr_time) {
      ASSERT(curr_time >= last_modified);
      if ( curr_time > last_modified ) value=0.0;
    }

    operator double() {return value;}
    operator double&() {return value;}
};


class Bdouble_vector {
  private:
    Bdouble *vector;
    int N;
  public:
    Bdouble_vector(int n) {
      N=n;
      vector=new Bdouble[n];
    }
    ~Bdouble_vector() {delete []vector;};
    Bdouble& operator [] (int i) {return vector[i];};
    void reset() {
      for (int k=0 ; k<N ; k++) vector[k].reset();
    }
    void reset(uint64 curr_time) {
      for (int k=0 ; k<N ; k++) vector[k].reset(curr_time);
    }
};


class double_vector {
  private:
    double *vector;
  public:
    double_vector(int n) {
      vector=new double[n];
      for(int k=0;k<n;k++) vector[k]=0;
    }
    ~double_vector() {delete []vector;}
    double& operator [] (int i) {return vector[i];}
};




/*************************************************************************/
/*                       Power recording classes                        */
/*************************************************************************/

class power {
  public:
    double_vector cores;
    double_vector rams;
    double_vector dcaches;
    double_vector icaches;    
    double_vector dmas;
    double_vector scratches;
    double_vector iscratches;
    double_vector bridges;
    double_vector buses_typ;
    double_vector buses_max;
    double_vector buses_min;
    double tot_energy, tot_energy_cores, tot_energy_rams, tot_energy_dcaches, tot_energy_icaches, tot_energy_dmas, tot_energy_scratches,
      tot_energy_iscratches, tot_energy_bridges, tot_energy_buses_typ, tot_energy_buses_min, tot_energy_buses_max;
    double bustyp, busmax, busmin;
  private:
    int n_cores, n_rams, n_dmas, n_caches, n_bridges, n_buses;
  public:
    power(uint ncores, uint nrams, uint nbridges, uint nbuses) : 
      cores(ncores), rams(nrams), dcaches(ncores), icaches(ncores), dmas((uint)DMA+N_SMARTMEM), scratches(ncores), iscratches(ncores),
      bridges(nbridges), buses_typ(nbuses), buses_max(nbuses), buses_min(nbuses)
    {      
       n_cores=ncores, n_rams=nrams, n_dmas=(uint)DMA+N_SMARTMEM, n_bridges=nbridges, n_buses=nbuses;
       resetValues();
       tot_energy=0.0;
       tot_energy_cores=0.0;
       tot_energy_rams=0.0;
       tot_energy_dcaches=0.0;
       tot_energy_icaches=0.0;
       tot_energy_dmas=0.0;
       tot_energy_scratches=0.0;
       tot_energy_iscratches=0.0;
       tot_energy_bridges=0.0;
       tot_energy_buses_typ=0.0;
       tot_energy_buses_min=0.0;
       tot_energy_buses_max=0.0;
    }

    //~power() {}

      /* Print results: the energy spent during measures */
    void dump(FILE *f) {
      if (!f) f=stdout;
      for (int k=0; k<n_cores; k++) {
        fprintf(f, "ARM %d:\n   core:     %12.2f [pJ]\n", k, (double)cores[k]);
        tot_energy += (double)cores[k];
        tot_energy_cores += (double)cores[k];
        if (SHARED_CACHE)
        {
          fprintf(f, "   ucache:   %12.2f [pJ]\n", (double)icaches[k]);
          tot_energy += (double)icaches[k];
          tot_energy_icaches += (double)icaches[k];
        }
        else
        {
          fprintf(f, "   icache:   %12.2f [pJ]\n", (double)icaches[k]);
          fprintf(f, "   dcache:   %12.2f [pJ]\n", (double)dcaches[k]);
          tot_energy += (double)icaches[k] + (double)dcaches[k];
          tot_energy_icaches += (double)icaches[k];
          tot_energy_dcaches += (double)dcaches[k];
        }
        fprintf(f, "   scratch:  %12.2f [pJ]\n", (double)scratches[k]);
        tot_energy += (double)scratches[k];
        tot_energy_scratches += (double)scratches[k];
        fprintf(f, "   i-scratch:%12.2f [pJ]\n", (double)iscratches[k]);
        tot_energy += (double)iscratches[k];
        tot_energy_iscratches += (double)iscratches[k];
      }
      for (int k=0; k<n_rams; k++) {
        fprintf(f, "RAM %02d:      %12.2f [pJ]\n", k, (double)rams[k]);
        tot_energy += (double)rams[k];
        tot_energy_rams += (double)rams[k];
      }
     
      for (int k=0; k<n_dmas; k++) {
        fprintf(f, "DMA %d:       %12.2f [pJ]\n", k, (double)dmas[k]);
        tot_energy += (double)dmas[k];
        tot_energy_dmas += (double)dmas[k];
      }
      
      for (int k=0; k<n_bridges; k++) {
        fprintf(f, "Bridge %d:    %12.2f [pJ]\n", k, (double)bridges[k]);
        tot_energy += (double)bridges[k];
        tot_energy_bridges += (double)bridges[k];
      }
      
      bustyp = 0.0;
      busmax = 0.0;
      busmin = 0.0;
      for (int k=0; k<n_buses; k++) {
        fprintf(f, "Bus %d:\n   typ:      %12.2f [pJ]\n", k, (double)buses_typ[k]);
        bustyp += (double)buses_typ[k];
        tot_energy_buses_typ += (double)buses_typ[k];
        fprintf(f, "   max:      %12.2f [pJ]\n", (double)buses_max[k]);
        busmax += (double)buses_max[k];
        tot_energy_buses_max += (double)buses_max[k];
        fprintf(f, "   min:      %12.2f [pJ]\n", (double)buses_min[k]);
        busmin += (double)buses_min[k];
        tot_energy_buses_min += (double)buses_min[k];
      }
      
      fprintf(f, "-------------------------------------\n");
      fprintf(f, "Partial sums:\n");
      fprintf(f, "   ARM cores:%12.2f [pJ]\n", tot_energy_cores);
      if (SHARED_CACHE)
        fprintf(f, "   ucaches:  %12.2f [pJ]\n", tot_energy_icaches);
      else
      {
        fprintf(f, "   icaches:  %12.2f [pJ]\n", tot_energy_icaches);
        fprintf(f, "   dcaches:  %12.2f [pJ]\n", tot_energy_dcaches);
      }
      fprintf(f, "   scratches:%12.2f [pJ]\n", tot_energy_scratches);
      fprintf(f, "   i-scratches:%10.2f [pJ]\n", tot_energy_iscratches);
      fprintf(f, "RAMs:        %12.2f [pJ]\n", tot_energy_rams);
      fprintf(f, "DMAs:        %12.2f [pJ]\n", tot_energy_dmas);
      fprintf(f, "Bridges:     %12.2f [pJ]\n", tot_energy_bridges);
      fprintf(f, "Buses:\n   typ:      %12.2f [pJ]\n", tot_energy_buses_typ);
      fprintf(f, "   max:      %12.2f [pJ]\n", tot_energy_buses_max);
      fprintf(f, "   min:      %12.2f [pJ]\n", tot_energy_buses_min);
      fprintf(f, "-------------------------------------\n");
      fprintf(f, "-------------------------------------\n");
      fprintf(f, "Total:       %12.2f [pJ] typ\n", tot_energy + (double)bustyp);
      fprintf(f, "Total:       %12.2f [pJ] max\n", tot_energy + (double)busmax);
      fprintf(f, "Total:       %12.2f [pJ] min\n", tot_energy + (double)busmin);
    }

      /* Print results: the power spent during measures */
      // FIXME the "mW" unit assumes that the time values will be expressed in ns
    void dump(FILE *f, double* core_time, double tottime) {
      if (!f) f=stdout;
      for (int k=0; k<n_cores; k++) {
        fprintf(f, "ARM %d:\n   core:     %12.2f [mW]\n", 
                k, (double)cores[k]/core_time[k]);
        if (SHARED_CACHE)
          fprintf(f, "   ucache:   %12.2f [mW]\n", 
                  (double)icaches[k]/core_time[0]);
        else
        {
          fprintf(f, "   icache:   %12.2f [mW]\n", 
                  (double)icaches[k]/core_time[0]);
          fprintf(f, "   dcache:   %12.2f [mW]\n", 
                  (double)dcaches[k]/core_time[0]);
        }
        fprintf(f, "   scratch:  %12.2f [mW]\n", 
                (double)scratches[k]/core_time[0]);
        fprintf(f, "   i-scratch:%12.2f [mW]\n", 
                (double)iscratches[k]/core_time[0]);
      }
      for (int k=0; k<n_rams; k++) {
        fprintf(f, "RAM %02d:      %12.2f [mW]\n", 
                k, (double)rams[k]/tottime);
      }
      
      for (int k=0; k<n_dmas; k++) {
        fprintf(f, "DMA %d:       %12.2f [mW]\n", 
                k, (double)dmas[k]/tottime);
      }
      
      for (int k=0; k<n_bridges; k++) {
        fprintf(f, "Bridge %d:    %12.2f [mW]\n", 
                k, (double)bridges[k]/tottime);
      }
    
      for (int k=0; k<n_buses; k++) {
        fprintf(f, "Bus %d:\n   typ:      %12.2f [mW]\n", 
                k, (double)buses_typ[k]/tottime);
        fprintf(f, "   max:      %12.2f [mW]\n", 
                (double)buses_max[k]/tottime);
        fprintf(f, "   min:      %12.2f [mW]\n", 
                (double)buses_min[k]/tottime);
      }
    } 
      
    void resetValues() {
      for (int k=0; k<n_cores; k++) {
        cores[k]=0.0;
        icaches[k]=0.0;
        dcaches[k]=0.0;
        scratches[k]=0.0;
        iscratches[k]=0.0;
      }
      for (int k=0; k<n_rams; k++) {
        rams[k]=0.0;
      }
      for (int k=0; k<n_dmas; k++) {
        dmas[k]=0.0;
      }
      for (int k=0; k<n_bridges; k++) {
        bridges[k]=0.0;
      }
      for (int k=0; k<n_buses; k++) {
        buses_typ[k]=0.0;
        buses_max[k]=0.0;
        buses_min[k]=0.0;
      }
    }
};


#ifdef RAM_COUNTER_WORKAROUND
  /* 
     At this time the counting of the RAMs accesses is not correct when 
     the cache coherence is enabled. In stats.cpp when a read involve a 
     cacheble address it is considered a cache refill (CORRECT) and a 
     private-mem access (WRONG).
     So we add this class and we enable it using the macro
     RAM_COUNTER_WORKAROUND in Makefile.defs
  */

class long_vector {
  private:
    long *vector;
  public:
    long_vector(int n) {
      vector=new long[n];
      for(int k=0;k<n;k++) vector[k]=0;
    }
    ~long_vector() {delete []vector;}
    long& operator [] (int i) {return vector[i];}
};


class ram_access {
  public:
    long_vector ram_reads;
    long_vector ram_writes;
    long_vector ram_stalls;    
    long_vector ram_noops;
  private:
    int m_nrams;
  public:
    ram_access(uint nrams) :
      ram_reads(nrams), ram_writes(nrams), ram_stalls(nrams),  ram_noops(nrams)
    {
      m_nrams=nrams;
    }

    void update(uint access_type, uint ID) {
      switch (access_type) {
        case READop:
          ram_reads[ID] ++;
          break;
        case WRITEop:
          ram_writes[ID] ++;
          break;
        case STALLop:
	  ram_stalls[ID]++;
          break;
        case NOop:
          ram_noops[ID] ++;
          break;	  
        default:
          break;
      }
    }

#if 0
      /* Print results: the number of memory accesses during measures */
    void dump(FILE *f) {
      if (!f) f=stdout;
      for (int k=0; k<n_rams; k++) {
        fprintf(f,
                "RAM %d:  %ld [reads]  %ld [writes]\n",
                k,
                (long)ram_reads[k],
                (long)ram_writes[k]);
      }
    }
#endif    
    
      /* Print results: the number of memory accesses during measures */
    void dump(FILE *f) {
      if (!f) f=stdout;

      for (int k=0; k<m_nrams; k++) {
        fprintf(f,
                "RAM %d:  %ld [reads]  %ld [writes] %ld [stalls] %ld [noops]\n",
                k,
                (long)ram_reads[k],
                (long)ram_writes[k],
                (long)ram_stalls[k],
                (long)ram_noops[k]);
	#ifdef EN_LEAKAGE	
	//FIXME Only the cores private memory and the shared are accounted for leakage
	if(addresser->IsPrivate(k) || addresser->IsLXPrivate(k) || addresser->IsShared(k))	
          fprintf(f,"\tidle energy %12.2f pJ\n", (long)ram_noops[k] * RAM_LEAKAGE );
        #endif	  
      }
    }

    void resetValues() {
      for (int k=0; k<m_nrams; k++) {
        ram_reads[k] = ram_writes[k] = 0;
      }
    }
};



class cache_access {
  public:

    cache_access() {
      cache_tag_reads = 0;
      cache_tag_writes = 0;
      cache_data_reads = 0;
      cache_dataline_writes = 0;
      cache_dataword_writes = 0;
      cache_dirty_reads = 0;
      cache_dirty_writes = 0;
      cache_dirty_bit_reads = 0;
      cache_dirty_bit_writes = 0;
    };

    void update(uint access_type) {
      switch (access_type) {
        case READTAGRAM:
          cache_tag_reads++;
          break;
        case WRITETAGRAM:
          cache_tag_writes++;
          break;
        case READDATA:
          cache_data_reads++;
          break;
        case WRITEDATALINE:
          cache_dataline_writes++;
          break;
        case WRITEDATAWORD:
          cache_dataword_writes++;
          break;
        case READDIRTYRAM:
          cache_dirty_reads++;
          break;
        case WRITEDIRTYRAM:
          cache_dirty_writes++;
          break;
        case READBIT:
          cache_dirty_bit_reads++;
          break;
        case WRITEBIT:
          cache_dirty_bit_writes++;
          break;
        default:
          break;
      }
    };

    void dump(FILE *f) {
      if (!f) f=stdout;
      fprintf(f,
              "%ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
              cache_tag_reads,
              cache_tag_writes,
              cache_data_reads,
              cache_dataline_writes,
              cache_dataword_writes,
              cache_dirty_reads,
              cache_dirty_writes,
              cache_dirty_bit_reads,
              cache_dirty_bit_writes);
    };

  private:
    long cache_tag_reads;
    long cache_tag_writes;
    long cache_data_reads;
    long cache_dataline_writes;
    long cache_dataword_writes;
    long cache_dirty_reads;
    long cache_dirty_writes;
    long cache_dirty_bit_reads;
    long cache_dirty_bit_writes;
};

class cache_access_vector {
  private:
    cache_access *vector;
    int N;
  public:
    cache_access_vector(uint n) {
      N=n;
      vector=new cache_access[n];
    };
    ~cache_access_vector() {delete []vector;};
    cache_access& operator [] (int i) {return vector[i];};
    void dump(FILE *f) {
      if (!f) f=stdout;
      for (int k=0; k<N; k++) {
        fprintf(f, "CACHE %d  -  ", k);
        vector[k].dump(f);
      }
    };
};

class cache_access_counter {
  private:
    cache_access_vector D_cache;
    cache_access_vector I_cache;

  public:
    cache_access_counter(uint ncaches) : D_cache(ncaches) , I_cache(ncaches) {};

    void update(uint access_type, uint ID, uint ID2) {
      if (ID2==0) I_cache[ID].update(access_type);
      else if (ID2==1) D_cache[ID].update(access_type);
    }

    void dump(FILE *f) {
      fprintf(f, "Cache Accesses:  tag_R tagW dataR datalW datawW dirtyR dirtyW bitR bitW\n");
      fprintf(f, "Instruction cache\n");
      I_cache.dump(f);
      fprintf(f, "Data cache\n");
      D_cache.dump(f);
    }
};
#endif


typedef enum
{
  BASE_TYP = 0,
  BASE_MIN = 1,
  HT_TYP   = 2,
  HT_MIN   = 3,
  HM_TYP   = 4,
  HM_MIN   = 5,
  HR_TYP   = 6,
  HR_MIN   = 7
} PAR_TYPE;


typedef enum
{
  BUS_TYP = 0,
  BUS_MIN = 1,
  BUS_MAX = 2
} BUS_ENERGY_LEVEL;


/*************************************************************************/
/*                       Power estimation functions                      */
/*************************************************************************/
double ARMEnergy(uint ID, int type, unsigned short int divider);
double powerRAM(uint ID, uint size, uint word_size, uint access_type);
double powerDMA(uint ID, uint size, uint word_size, uint access_type);
double powerSCRATCH(uint ID, uint size, uint word_size, uint access_type);
double powerDirectCache(uint ID, uint ID2, uint size, uint access_type);
double powerAssocCache(uint ID, uint ID2, uint size, uint word_size, uint access_type);
double powerSetAssocCache(uint ID, uint ID2, uint size, uint nWay, uint access_type);
#ifdef AMBAAHBBUILD
  // FIXME this puts an unwanted dependency on AMBA AHB headers
  double AMBAAHBEnergy(uint bus_ID, AHB_bus_activity monitor, BUS_ENERGY_LEVEL type);
#endif
double AMBAAHBBridgeEnergy(unsigned short int bridge_ID, bool master_side, bool interact_fifo);

void finalize_power(); // Calls all the final powers handlers installed
void install_final_power_handler(void (*func)(void*), void *arg);



/*************************************************************************/
/*                       Global variables declaration                    */
/*************************************************************************/
extern power *power_object;

#ifdef RAM_COUNTER_WORKAROUND
extern ram_access *ram_access_object;
extern cache_access_counter *cache_access_counter_object;
#endif



#endif // __POWER_H__

