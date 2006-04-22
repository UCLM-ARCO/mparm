///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         mem_class.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a generic memory class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEM_CLASS_H__
#define __MEM_CLASS_H__

#include <systemc.h>
#include "globals.h"
#include "debug.h"

#define MEM_WORD  0
#define MEM_BYTE  1
#define MEM_HWORD 2

class Mem_class
{
  protected:
    //ID of the memory
    uint16_t ID;
    uint32_t WHICH_TRACEX;
    
    //Pointer to the start of the instantiation of the memory
    char *myMemory;
    //Indicate the kind of the memory
    const char *type;
        
    //so that if needed the child class can simply derive it
    virtual uint32_t addressing(uint32_t addr)
    {
     return addr;
    };
    
    int load_program(char *);
    int load_srec_program(char *);
    long hexstringtonumber(char *str, int start, int len);

  public:
    //Size of the memory
    uint32_t size;
  
    Mem_class(uint16_t id, uint32_t size) : ID(id), size(size)
    {
       myMemory = new char[size]; 
       memset(myMemory, 0, size);
       WHICH_TRACEX = MEM_TRACEX;
    };

    virtual ~Mem_class(){};

//Read/Write function are virtual so we can derive a new memory class with custom methods...
    
///////////////////////////////////////////////////////////////////////////////
// Reads data from the memory.
    virtual uint32_t Read(uint32_t addr, uint8_t bw = MEM_WORD)
    {
     uint32_t data;
     
     addr = addressing(addr);

     if (addr >= size)
     {
      printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
      exit(1);
     }
  
     switch (bw)
     {    
      case MEM_WORD: // Read word
      {
       data = *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC)));
       break;
      }   
      case MEM_BYTE: // Read byte
      {
       data= *((uint32_t *)(myMemory + addr)); 
       data = (data & 0x000000FF);
       break;
      }
      case MEM_HWORD: // Read half word
      {
       data= *((uint32_t *)(myMemory + addr)); 
       data = (data & 0x0000FFFF);
       break;
      }
      default: // Error
      {
       printf("%s Memory %d: Bad read size request %u\n", type, ID, bw);
       exit(1);
      }
     }
     
     //TRACEX(WHICH_TRACEX, 8,"%s %d: Address: 0x%08x Read data:  0x%08x Size: %s Time: %.0f\n",
     // type, ID, addr, data, bw == 0 ? "word" : (bw == 1 ? "byte" : "hword"), sc_simulation_time());
     TRACEX(WHICH_TRACEX, 8,"%s %d: Address: 0x%08x Read data:  0x%08x Size: %s\n",
      type, ID, addr, data, bw == 0 ? "word" : (bw == 1 ? "byte" : "hword")); 
  
     return data;
    };
    
///////////////////////////////////////////////////////////////////////////////
// Writes data to the memory.
    virtual void Mem_class::Write(uint32_t addr, uint32_t data, uint8_t bw = MEM_WORD)
    {
     addr = addressing(addr);

     if (addr >= size)
     {
      printf("%s Memory %d: Bad address 0x%08x\n", type, ID, addr);
      exit(1);
     }

     switch (bw)
     {
      case MEM_WORD: // Write word
      {
       *((uint32_t *)(myMemory + (addr & 0xFFFFFFFC))) = data;
       break;
      }     
      case MEM_BYTE: // Write byte
      {
       data = data & 0x000000FF;
       *((char *)(myMemory + addr)) = (char)data;
       break;
      }    
      case MEM_HWORD: // Write half word
      {
       data = data & 0x0000FFFF;
       *((uint16_t *)(myMemory + (addr & 0xFFFFFFFE))) = (uint16_t)data;
       break;
      }
      default: // Error
      {
       printf("%s Memory %d: Bad write size request %u\n", type, ID, bw);
       exit(1);
      }
     } 
  
     //TRACEX(WHICH_TRACEX, 8,"%s %d: Address: 0x%08x Write data: 0x%08x Size: %s Time: %.0f\n",
     // type, ID, addr, data, bw == 0 ? "word" : (bw == 1 ? "byte" : "hword"), sc_simulation_time());
     TRACEX(WHICH_TRACEX, 8,"%s %d: Address: 0x%08x Write data: 0x%08x Size: %s\n",
      type, ID, addr, data, bw == 0 ? "word" : (bw == 1 ? "byte" : "hword"));
    };

};

#endif // __MEM_CLASS_H__
