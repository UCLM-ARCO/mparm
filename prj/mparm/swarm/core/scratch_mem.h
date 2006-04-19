///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         scratch_mem.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements MPARM scratch memory types
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCRATCH_MEM_H__
#define __SCRATCH_MEM_H__

#include "mem_class.h"
#include "power.h"
#include "stats.h"

//This implement a "normal" internal memory of a core
class Scratch_mem :public Mem_class
{ 
 private:
 uint32_t base_address; 
 
 inline uint32_t addressing(uint32_t addr)
 {return addr -=base_address;
 }

 public:
  
  Scratch_mem(char* typ,uint16_t id,uint32_t size,uint32_t base) : 
    Mem_class(id,size)
  {
   type = typ;
   base_address=base;
   printf("%s %d - Size: 0x%08x, Base Address: 0x%08x\n",
           type, ID, (int)size, base_address);
	   
   WHICH_TRACEX=SCRATCH_TRACEX;

   addresser->scratchMemoryDebug[ID]=myMemory;
  }
  
  uint32_t Read(uint32_t addr, uint8_t bw=0)
  {
   double pwr;
   if (POWERSTATS)
   {
    pwr = powerSCRATCH(ID, addresser->ReturnScratchSize(), 32, READop);
    statobject->inspectSCRATCHAccess(addr, true, pwr, ID);
   }
   return Mem_class :: Read(addr,bw);
  }
  
  void Write(uint32_t addr, uint32_t data, uint8_t bw=0)
  {
   double pwr;
   Mem_class :: Write(addr, data, bw);
   if (POWERSTATS)
   {
    pwr = powerSCRATCH(ID, addresser->ReturnScratchSize(), 32, WRITEop);
    statobject->inspectSCRATCHAccess(addr, false, pwr, ID);
   }
  }
  
};

//This class implements a scratchpad memory devoted to (partial)
//replacement of the I-Cache of the processor
class IScratch_mem :public Mem_class
{ 
 private:
 uint32_t base_address; 
 
 inline uint32_t addressing(uint32_t addr)
 {return addr -=base_address;
 }

 public:
  
  IScratch_mem(char* typ,uint16_t id,uint32_t size,uint32_t base) : 
    Mem_class(id,size)
  {
   type = typ;
   base_address=base;
   printf("%s %d - Size: 0x%08x, Base Address: 0x%08x\n",
           type, ID, (int)size, base_address);
	   
   WHICH_TRACEX=SCRATCH_TRACEX;
  }
  
  uint32_t Read(uint32_t addr, uint8_t bw=0)
  {
   double pwr;
   if (POWERSTATS)
   {
    pwr = powerSCRATCH(ID, ISCRATCHSIZE, 32, READop);
    statobject->inspectISCRATCHAccess(addr, true, pwr, ID);
   }
   return Mem_class :: Read(addr);
  }
  
  void Write(uint32_t addr, uint32_t data, uint8_t bw=0)
  {
   double pwr;
   Mem_class :: Write(addr, data, bw);
   if (POWERSTATS)
   {
    pwr = powerSCRATCH(ID, ISCRATCHSIZE, 32, WRITEop);
    statobject->inspectISCRATCHAccess(addr, false, pwr, ID);
   }
  }
  
};

//This implment a partitioned scratch-pad memory
class Scratch_part_mem :public Mem_class
{ 
 private:
  
 inline uint32_t addressing(uint32_t addr)
 {return addresser->Logical2PartScratch(ID, addr);
 }

 public:
  
  Scratch_part_mem(uint16_t id,uint32_t size) : 
    Mem_class(id,size)
  {
   uint8_t nRanges, loop;
   uint32_t start, finish, rangeSize, base;
   
   type = "Scratchpad_partitioned";

   addresser->scratchMemoryDebug[ID]=myMemory;
  
   // If performing a partitioned scratchpad analysis, we need to load
   // code on the SPM since boot. Uses addresser->pMemoryDebug pointers,
   // assuming that IDs will match and especially that external memories
   // will be instantiated before SPMs!
   
   //Poletti in questo pezzo a type ho sostituito my per sovrapposizione dei nomi...
   //type e' diventata una stringa indicante il typo di memoria
   //my e' l'id passato al costruttore
   
   nRanges = addresser->ReturnRangeNumber(ID);
    for (loop = 0, base = 0; loop < nRanges; loop++)
    {
      addresser->ReturnRangeBounds(ID, loop, &start, &finish);
      rangeSize = finish + 1 - start;
      memcpy(myMemory + base, addresser->pMemoryDebug[ID] + start, rangeSize);
      base += rangeSize;
    }
    printf("%s Memory %d: %hu ranges loaded from binary image\n", type, ID, nRanges);
  
  puts("");
   
  }
};

#if 1

#define N_NON_SEND_INT_SEM 2
//////////////////
//new VERSION OF THE SEMAPHORE queue
/////////////////
//This implement a "semaphore" internal memory of a core
class Scratch_queue_mem : public sc_module, public Mem_class
{ 
 private:
 
 uint32_t base_address; 
 bool * flag_suspend;
 
 inline uint32_t addressing(uint32_t addr)
 {return addr -=base_address;
 }

 public:  
  Scratch_queue_mem(sc_module_name typ,uint16_t id,uint32_t size,uint32_t base) : 
    sc_module(typ), Mem_class(id,size)
  {
   type = name();
   base_address=base;
   printf("%s %d - Size: 0x%08x, Base Address: 0x%08x\n",
           type, ID, (int)size, base_address);
  
   WHICH_TRACEX=SEMAPHORE_TRACEX;

   flag_suspend=new bool[size];
   
   for(uint i=0;i<size;i++)
   {
    flag_suspend[i]=false;
   }
  }
  
    
  //This is for send int to the wrapper
  sc_inout<bool> sendint;
  
  //overriding
  uint32_t Read(uint32_t addr, uint8_t bw=0)
  {
   uint32_t data;
  
   addr = addressing(addr);
  
   if (addr >= size)
   {
    printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
    exit(1);
   }

   data = *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC)));
  
   //Poletti: If is not zero decrement
   if (data!=0)
   {
    *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) = data-1; 
   }
   else
   {
    if(addr>=N_NON_SEND_INT_SEM*4)
     flag_suspend[addr>>2]=true;
     
    TRACEX(WHICH_TRACEX, 8,
          "%s:%d Set Flag int Addr:%x flag_suspend[%d]=%d\n", type, ID, addr, addr>>2,
	  flag_suspend[addr>>2]);
   }
   
   TRACEX(WHICH_TRACEX, 8,
          "%s:%d Addr:%x Read:%d Size:%d\n", type, ID, addr, data, 0);

   return data;
  };
  
  //overriding  
  void Write(uint32_t addr, uint32_t data, uint8_t bw=0)
  {
   addr = addressing(addr);

   if (addr >= size)
   {
    printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
    exit(1);
   }

   switch (bw)
   {
    case 0: // Write word
    {
      if(data==0)
      { 
       //Poletti: If write "0" I increment
       *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) += 1;
       

      TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x Increment write:%d Size:%d\n",
       type, ID, addr,*((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))), bw);
	    
      }
      else
       {
        *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) = data-1;
      
        TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x Write:%d Size:%d\n",
         type, ID, addr, data, bw); 

       }
     
     if(flag_suspend[addr>>2])
     {
       TRACEX(WHICH_TRACEX, 8,
          "%s:%d SEND_INT Addr:%x flag_suspend[%d]=%d\n", type, ID, addr, addr>>2,
	  flag_suspend[addr>>2]);
     
      sendint.write(true);
       flag_suspend[addr>>2]=false; 
     }  
       
     break;
    }
    default: // Error
    {
      printf("%s Memory %d: Bad write size request %u\n", type, ID, bw);
      exit(1);
    }
   }
  };

};

#else
//////////////////
//OLD VERSION OF THE SEMAPHORE
/////////////////

//This implement a "semaphore" internal memory of a core
class Scratch_queue_mem :public Mem_class
{ 
 private:
 
 uint32_t base_address; 
 
 inline uint32_t addressing(uint32_t addr)
 {return addr -=base_address;
 }

 public:
  
  Scratch_queue_mem(char* typ,uint16_t id,uint32_t size,uint32_t base) : 
    Mem_class(id,size)
  {
   type = typ;
   base_address=base;
   printf("%s %d - Size: 0x%08x, Base Address: 0x%08x\n",
           type, ID, (int)size, base_address);
   
   WHICH_TRACEX=SEMAPHORE_TRACEX;
  }
  
  
  //overriding
  uint32_t Read(uint32_t addr, uint8_t bw=0)
  {
   uint32_t data;
  
   addr = addressing(addr);
  
   if (addr >= size)
   {
    printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
    exit(1);
   }

   data = *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC)));
  
   //Poletti: If is not zero decrement
   if (data!=0)
   {
    *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) = data-1; 
   }
   
   TRACEX(WHICH_TRACEX, 8,
          "%s:%d Addr:%x Read:%d Size:%d\n", type, ID, addr, data, 0);

   return data;
  };
  
  //overriding  
  void Write(uint32_t addr, uint32_t data, uint8_t bw=0)
  {
   addr = addressing(addr);

   if (addr >= size)
   {
    printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
    exit(1);
   }

   switch (bw)
   {
    case 0: // Write word
    {
      if(data==0)
      { 
       //Poletti: If write "0" I increment
       *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) += 1;
       

      TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x Increment write:%d Size:%d\n",
       type, ID, addr,*((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))), bw);
	    
      }
      else
       {
        *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) = data-1;
      
        TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x Write:%d Size:%d\n",
         type, ID, addr, data, bw); 

       }
     break;
    }
    default: // Error
    {
      printf("%s Memory %d: Bad write size request %u\n", type, ID, bw);
      exit(1);
    }
   }
  };

};
#endif

#endif // __SCRATCH_MEM_H__
