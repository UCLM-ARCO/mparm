///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         AHB_initiator_lx.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         ST_LX STBus initiator interface
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AHB_INITIATORLX_H__
#define __AHB_INITIATORLX_H__

#include "systemc.h"

#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"
#include "debug.h"

#include "core_signal.h"
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"

#include "sim_support.h"
#include "ast_iss_if.h"

//maximum number of word for burst transfer
#define MAX_DIM_BURST_AHB 16

//Poggiali
//Preso da ahbbus_adapter, utilizziamo una queue OCCN per le transazioni pendenti
#include <occn.h>

#define AHBBUS_MAX_WORDS_PER_TRANS  32
#undef INI_DEBUG

//base transaction in the queue fifo
struct AhbBusTransaction
{
  Int8 opcode;                                  // opcode of the transaction
  UInt16 size;                                  // size in words of the transaction (0=free)
  UInt32 address;                               // address of the transaction
  UInt32 buffer[AHBBUS_MAX_WORDS_PER_TRANS];    // temporary buffer
  UInt8 be[AHBBUS_MAX_WORDS_PER_TRANS];         // byte enable
  unsigned int remaining_words, completed_words;
  sc_signal<lx_extmem_result> status;
};

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE,
          uint TRANS_TABLE_SIZE, unsigned char SOURCE, bool TRAFFIC_GEN, bool INI_DEBUG>

class AHBBus_initiator_lx :
          public ASTISSIf
	, public sc_module
{
  public:
  SC_HAS_PROCESS(AHBBus_initiator_lx);
  sc_port<ahb_interconnect_if> master_port;
  sc_in<bool>                  clock;
  sc_in<bool>                  reset;
  
  private: 

  char *type;
  uint mem_id, idx_mem1;
  QueueObject<int> id_fifo;
  Semaphore new_request;

  UInt32 tmp_buffer[AHBBUS_MAX_WORDS_PER_TRANS];    // temporary buffer for software accesses
  bool tmp_be[AHBBUS_MAX_WORDS_PER_TRANS];          // temporary byteenable array for software accesses

  vector<AhbBusTransaction *> trans;                // array of pending transactions


  public:
  AHBBus_initiator_lx(sc_module_name nm, unsigned int max_pipeline,
                       ASTISSIf::TRACE trace_value=ASTISSIf::TRACE_NO_ACCESSES,
		       ASTISSIf::DEBUG_LX_WRAPPER debug_value=ASTISSIf::DEBUG_NO_ACCESSES) :

	ASTISSIf(trace_value, debug_value),sc_module(nm)
    {

    SC_THREAD(initiator_behav);
	    sensitive_pos << clock;

      type="AHB_INITIATOR_LX";

//inizializzazione della coda Fifo dal codice del costruttore di ahhbus_adapter
	  for (unsigned int i=0;i<max_pipeline;i++)
	  {
	    AhbBusTransaction* tmp = new AhbBusTransaction;
	    tmp->size = 0;
	    trans.push_back(tmp);
	  }


     TRACEX(ST_INITIATOR_LX, 7,
             "%s:%d sc_name:%s \n",
             type,(int)SOURCE, (const char*)nm);
    }



AHBBus_initiator_lx::~AHBBus_initiator_lx()
{
//eliminazione della coda Fifo dal codice del distruttore di ahhbus_adapter

  for (unsigned int i=0;i<trans.size();i++)
    delete trans[i];
}

private:

// ***************************** transactions handling functions ****************************

// ------------------------------------------------------------------------------------------
// Function AHBBus_initiator_lx::GetTransactionId(opcode,size,address)
//
// Return the id of the transaction whose opcode is <opcode> (<opcode> can be either
// OCCN_READ or OCCN_WRITE) and having address <address> and size <size>, or -1 if such
// transaction does not exist.
// ------------------------------------------------------------------------------------------

inline
int AHBBus_initiator_lx::GetTransactionId(Int8 opcode,UInt16 size,UInt32 address)
{
  unsigned int i = trans.size();

  while (i--)
    if (trans[i]->opcode == opcode && trans[i]->size == size && trans[i]->address == address)
      return i;

  return -1;
}


// ------------------------------------------------------------------------------------------
// Function AHBBus_initiator_lx::GetFreeTransactionId()
//
// Return the id of a free position in the array of transactions, or -1 if the array is
// full.
// ------------------------------------------------------------------------------------------

inline
int AHBBus_initiator_lx::GetFreeTransactionId()
{
  unsigned int i = trans.size();

  while (i--)
    if (trans[i]->size == 0)
      return i;

  return -1;
}

// ***************************** ASTISSIf interface implementation ****************************

  virtual lx_extmem_result _SwDataRead(UInt16 size,UInt32 address,UInt32 *buffer,UInt8 *byteenable)
    {
     N_uint32 *src = (N_uint32*)buffer;

     mem_id= addresser->MapPhysicalToSlave(address);
	    if(addresser->IsInterrupt(mem_id))
	     {printf("Trying to perform Software Read on a non \"READABLE\" device\n");
	      exit(1);
	     }

     idx_mem1=addresser->ReturnSlavePhysicalAddress(mem_id);

     for (unsigned int i=0; i<(size); i++)
     {
	    src[0]=addresser->pMem_classDebug[mem_id]->Read(address-idx_mem1+4*i, 0);
     }

     if (byteenable)
     for (N_uint32 i=0; i<size; i++)
      src[i]&=GetByteEnableMask(byteenable[i]);

    TRACEX(ST_INITIATOR_LX1, 10,
                  "%s:%d SW_Read Addr:%x, size:%u\n", type, SOURCE, address,size);

    for(uint i=0;i<size;i++)
      TRACEX(ST_INITIATOR_LX1, 10,
                  "data:%x\n",buffer[i]);

     return LX_EXTMEM_COMPLETED;

    }

  virtual lx_extmem_result _SwDataWrite(UInt16 size,UInt32 address,UInt32 *buffer,UInt8 *byteenable)
    {
     mem_id= addresser->MapPhysicalToSlave(address);
     if(addresser->IsInterrupt(mem_id))
       {printf("Trying to perform Software Read on a non \"READABLE\" device\n");
	exit(1);
       }

     idx_mem1=addresser->ReturnSlavePhysicalAddress(mem_id);

     if (!byteenable)
     {
      N_uint32 *src = (N_uint32*)buffer;

      for (unsigned int i=0; i<(size); i++)
      {
       addresser->pMem_classDebug[mem_id]->Write(address-idx_mem1+4*i, src[i], 0);
      }
     }
     else
     {
      for (unsigned int i=0; i<(size); i++)
      {
       tmp_buffer[0]=addresser->pMem_classDebug[mem_id]->Read(address-idx_mem1+4*i, 0);
      }

      N_uint32 mask;
      for (N_uint32 i=0; i<size; i++)
      {
       mask = GetByteEnableMask(byteenable[i]);

       tmp_buffer[i] = (tmp_buffer[i] & (~mask)) | (buffer[i] & mask);
      }

      for (unsigned int i=0; i<(size); i++)
      {
       addresser->pMem_classDebug[mem_id]->Write(address-idx_mem1+4*i, tmp_buffer[i], 0);
      }

     }

     TRACEX(ST_INITIATOR_LX1, 10,
                  "%s:%d SW_Write Addr:%x, size:%u\n", type, SOURCE, address,size);

    for(uint i=0;i<size;i++)
       TRACEX(ST_INITIATOR_LX1, 10,
                  "data:%x\n",buffer[i]);


    return LX_EXTMEM_COMPLETED;
   }


lx_extmem_result AHBBus_initiator_lx::_HwDataRead(UInt16 size,
                                            UInt32 address,
                                            UInt32 *buffer,
                                            UInt8 *byteenable,
                                            const lx_extmem_attributes *attributes)
{
  assert(size>0 && size<=AHBBUS_MAX_WORDS_PER_TRANS);

  int id = GetTransactionId(OCCN_READ,size,address);

  // Is this a simulation support access?
  if (addresser->PhysicalInSimSupportSpace(address))
  {
   if(size!=1) printf("Warning strange access to the simsupport address\n");

   simsuppobject->catch_sim_message( (address-addresser->ReturnSimSupportPhysicalAddress()),
   (uint*)buffer, 0, SOURCE);

   TRACEX(ST_INITIATOR_LX1, 8,
                  "%s:%d SWI_HW_Read Addr:%x, size:%u\n", type, SOURCE, address,size);

   return LX_EXTMEM_COMPLETED;
  }

  // request for new transaction
  if (id == -1)
  {

    // try to enqueue the transaction
    id = GetFreeTransactionId();

    if (id == -1)
    {
      cerr << endl
           << "Error: Too many pipelined transactions in AhbBusAdapter::_HwDataRead()" << endl
           << endl;

      exit(-1);
    }

    trans[id]->opcode = OCCN_READ;
    trans[id]->size = size;
    trans[id]->address = address;

    for (UInt16 i=0; i<size; i++)
      trans[id]->be[i] = (byteenable ? byteenable[i] : 0xF);

    trans[id]->remaining_words = size;
    trans[id]->completed_words = 0;
    trans[id]->status = LX_EXTMEM_PENDING;

    id_fifo.add(id);
    new_request.post();
    return LX_EXTMEM_PENDING;
  }

  // already existing transaction
  else
  {
    // transaction is completed
    if (trans[id]->status == LX_EXTMEM_COMPLETED)
    {

      trans[id]->size = 0;

      // copy the read data from the temporary buffer to the one passed to the function
      for (UInt16 i=0;i<size;i++){
        *(buffer+i)=(*(trans[id]->buffer+i));
      }


     TRACEX(ST_INITIATOR_LX1, 8,
                  "%s:%d HW_Read Addr:%x, size:%u\n", type, SOURCE, address,size);

     for(uint i=0;i<size;i++)
      TRACEX(ST_INITIATOR_LX1, 8,
                  "data:%x\n",buffer[i]);
    }

    return trans[id]->status;
  }
}




lx_extmem_result AHBBus_initiator_lx::_HwDataWrite(UInt16 size,
                                             UInt32 address,
                                             UInt32 *buffer,
                                             UInt8* byteenable,
                                             const lx_extmem_attributes *attributes)
{
  assert(size>0 && size<=AHBBUS_MAX_WORDS_PER_TRANS);

  // Is this a simulation support access?
  if (addresser->PhysicalInSimSupportSpace(address))
  {
   if(size!=1) printf("Warning strange access to the simsupport address\n");

   simsuppobject->catch_sim_message( (address-addresser->ReturnSimSupportPhysicalAddress()),
   (uint*)buffer, 1, SOURCE);

   TRACEX(ST_INITIATOR_LX1, 8,
                  "%s:%d SWI_HW_Write Addr:%x, size:%u\n", type, SOURCE, address,size);

   return LX_EXTMEM_COMPLETED;
  }

  int id = GetTransactionId(OCCN_WRITE,size,address);

  // request for new transaction
  if (id == -1)
  {
    // try to enqueue the transaction
    id = GetFreeTransactionId();

    if (id == -1)
    {
      cerr << endl
           << "Error: Too many pipelined transactions in AhbBusAdapter::_HwDataWrite()" << endl
           << endl;

      exit(-1);
    }

    trans[id]->opcode = OCCN_WRITE;
    trans[id]->size = size;
    trans[id]->address = address;

    for (UInt16 i=0; i<size; i++)
      trans[id]->be[i] = (byteenable ? byteenable[i] : 0xF);

    trans[id]->remaining_words = size;
    trans[id]->completed_words = 0;
    trans[id]->status = LX_EXTMEM_PENDING;

    // copy the data to write from the buffer passed to the function to the temporary one
    for (UInt16 i=0;i<size;i++)
      *(trans[id]->buffer+i)=(*(buffer+i));

    id_fifo.add(id);
    new_request.post();

    return LX_EXTMEM_PENDING ;
  }

  // already existing transaction
  else
  {
    // transaction is completed
    if (trans[id]->status == LX_EXTMEM_COMPLETED)
     {
      trans[id]->size = 0;

         TRACEX(ST_INITIATOR_LX1, 8,
                  "%s:%d HW_Write Addr:%x, size:%u\n", type, SOURCE, address,size);

      for(uint i=0;i<size;i++)
         TRACEX(ST_INITIATOR_LX1, 9,
                  "data:%x\n",buffer[i]);

     }
    return trans[id]->status;
  }
}

lx_extmem_result AHBBus_initiator_lx::_SpecialOperation(lx_memory_special_op_ty operation,UInt16 size,
                                                  UInt32 address,UInt32 *buffer,
                                                  const lx_extmem_attributes *attributes)
{
  // just return LX_EXTMEM_COMPLETED, since no special operations are implemented
  return LX_EXTMEM_COMPLETED;
}



  // ------------------------------ BEHAVIOR --------------------------------

//ovverriding of the base function
void AHBBus_initiator_lx::initiator_behav()
{
     TDATA din,dout, next_dout, din_temp;
     TADDRESS address;
     bool bezero=false,undef_incr=false,rw=false,betrue=false;
     UInt16 dimsize,countbe=0;
     uint32_t mask;
     unsigned long int bemask;   //FIXME Guarda che ho messo BEMASK a 64 bit
     int id,i,burst,Start_index;
     unsigned char local_ID;     //TODOanche global id e' cambiato in SOURCE!!!!
     UInt32 prev_address;

     AHB_master_port my_port;

    local_ID = master_port->get_master_ID();

    dimsize = 0x4;
    while(1)
    {
      //waiting for new request
      new_request.wait();
      id = id_fifo.remove();
      //wait inserita per annullare gli effetti del aumento del delta cycle della FIFO
      wait();

      if (STATS)
        statobject->requestsAccess(SOURCE);

     //set hburst
     if ((trans[id]->remaining_words == 1) || (trans[id]->remaining_words==4) ||
           (trans[id]->remaining_words==8)  || (trans[id]->remaining_words==16))
	  {
	   burst=trans[id]->remaining_words;
           switch (burst)
	    {
	    case 1 :  my_port.hburst = AHB_SINGLE;
	               break;
	    case 4 :  my_port.hburst = AHB_INCR4;
                       break;
            case 8 :  my_port.hburst = AHB_INCR8;
                       break;
            case 16: my_port.hburst = AHB_INCR16;
      	               break;
    	    default : cout<<"WRONG BURST SIZE "<< burst <<endl; exit(-1);
            }
	   }
     else {
            my_port.hburst = AHB_INCR;
	    burst=trans[id]->remaining_words;
	    undef_incr=true;
     	   }


     ASSERT( (trans[id]->opcode == OCCN_WRITE) || (trans[id]->opcode == OCCN_READ) );
     if (trans[id]->opcode == OCCN_WRITE) rw=true;
     if (trans[id]->opcode == OCCN_READ) rw=false;

     //if write && be i must change hburst and lock
     if (rw)
     {countbe=0;
     for (unsigned int i = 0; i < trans[id]->remaining_words; i++)
	{
        if (trans[id]->be[i] != 0) countbe++;
	 if (trans[id]->be[i] != 15)
	  {my_port.hlock = true;
	   undef_incr=true;
	   my_port.hburst = AHB_INCR;
          }
	}
     }

      address = trans[id]->address;
      my_port.htrans = AHB_NONSEQ;
      my_port.hbusreq = true;
      my_port.hsize = AHB_WORD;

       //byteenble
       //building be concatenation
       bemask=0;
       for (i = 0; i < burst; i++)
	   bemask = bemask | (trans[id]->be[i]<<(i*4));
       //Skip bus transaction since be=0
       if ((bemask == 0x0))
       {
        trans[id]->completed_words=trans[id]->remaining_words;
        trans[id]->remaining_words=0;
	printf("Transazione vuota burst:%d rw:%x!!!\n",trans[id]->completed_words,(trans[id]->opcode==OCCN_READ)?1:0);
	sc_stop();
       }

     if(rw)
      // ------------------------------ WRITE ACCESS ------------------------------
     {
      //eventual skip transitions if be == 0
      Start_index=0;
      while(trans[id]->be[trans[id]->completed_words]==0)
	{
	  trans[id]->completed_words++;
	  trans[id]->remaining_words--;
	  address += dimsize;
	  Start_index++;
	}


       next_dout = (*(trans[id]->buffer+trans[id]->completed_words));

      //set haddr, hwrite, hsize and aburst
      if(trans[id]->be[trans[id]->completed_words]==15)
           { my_port.hwrite = AHB_WRITE;
	     my_port.haddr = address;}
      else { mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);
              switch (trans[id]->be[trans[id]->completed_words])
		       { case 1: my_port.hwrite = AHB_WRITE;
		                  my_port.hsize = AHB_BYTE;
		                  my_port.haddr = address;
				  next_dout= (mask & next_dout);
				 break;
			 case 2: my_port.hwrite = AHB_WRITE;
			          my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 1;
				  next_dout= (mask & next_dout);
				  next_dout >>=8;
				  break;
			 case 3: my_port.hwrite = AHB_WRITE;
			          my_port.hsize = AHB_HALFWORD;
				  my_port.haddr = address;
				  next_dout= (mask & next_dout);
				  break;
			 case 4: my_port.hwrite = AHB_WRITE;
			          my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 2;
				  next_dout= (mask & next_dout);
			          next_dout >>=16;
				  break;
			 case 8: my_port.hwrite = AHB_WRITE;
			          my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 3;
				  next_dout= (mask & next_dout);
				  next_dout >>=24;
				  break;
			 case 12:my_port.hwrite = AHB_WRITE;
			          my_port.hsize = AHB_HALFWORD;
			          my_port.haddr = address + 2;
				  next_dout= (mask & next_dout);
				  next_dout >>=16;
				  break;
			 default :my_port.hwrite = AHB_READ;
				   my_port.haddr = address;
				   betrue=true;
				   break;
		        }
             my_port.hburst = AHB_SINGLE;
	    }

      // Let's request the bus
      master_port->ahb_interconnect_write(local_ID, &my_port);

      // Wait for the grant. Actually, we might get a zero-cycle
      // grant (e.g. if we're the only master on the bus)
      master_port->ahb_interconnect_read(local_ID, &my_port);
      while (!(my_port.hgrant && my_port.hready))
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      }

	if (STATS)
	  statobject->beginsAccess(2, !rw, countbe, SOURCE);
	  
      // Here, we really got the bus and pushed out the first address,
      // deassert hbusreq
      if (!undef_incr) my_port.hbusreq = false;

      master_port->ahb_interconnect_write(local_ID, &my_port);
      do
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      } while (!(my_port.hready));

      // spostata sopra! 13/09/2005
      //if (STATS)
      //  statobject->beginsAccess(2, !rw, countbe, SOURCE);


      for (i = Start_index; i < burst; i++)
      {
	if (betrue) //READ part (happend if be != 0, 1, 2, 3, 4, 8, 12, 15)
	      {
	      betrue=false;
	      my_port.hbusreq = true;
	      my_port.hsize = AHB_WORD;
	      my_port.hwrite = AHB_WRITE;
		 if ((i == burst -1) ||
		    (trans[id]->be[trans[id]->completed_words+1]!=15) )
		     my_port.hburst = AHB_SINGLE;
	         else my_port.hburst = AHB_INCR;

		 master_port->ahb_interconnect_write(local_ID, &my_port);
		   do
		   {
		    wait();
		    master_port->ahb_interconnect_read(local_ID, &my_port);
		   } while (!(my_port.hready));

		 //mask data && byteenable
		 mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);
		 din_temp = my_port.hrdata;
		 next_dout= ((mask & next_dout)|(~mask & din_temp));
	      }//end READ

	// If we're in a burst transfer, at this point we already have subsequent
        // data to process
	 prev_address = address;
	 dout = next_dout;
	 my_port.hwdata = dout;

	//eventual skip transitions if be == 0
	while((trans[id]->be[trans[id]->completed_words+1]==0) && (burst - 1 != i))
	   {
	     trans[id]->completed_words++;
             trans[id]->remaining_words--;
	     i++;
	     address += dimsize;
	     my_port.htrans = AHB_NONSEQ;
	     bezero=true;
	   }


	if (burst - 1 != i)
	//set haddr, hsize, htrans, hburst in next transitions
        {my_port.hsize = AHB_WORD;
	 address += dimsize;
	 my_port.haddr = address;
	 next_dout = (*(trans[id]->buffer+trans[id]->completed_words+1));

	       if (trans[id]->be[trans[id]->completed_words+1]==15)

		    //Normal write (be == f)
		    {my_port.hsize = AHB_WORD;
		     if (!bezero) my_port.htrans = AHB_SEQ;
		    }

	       else{mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words+1]);
		      switch (trans[id]->be[trans[id]->completed_words+1])
		        {
			//be == 1, 2, 3, 4, 8, 12 (change haddr and hasize)
		         case 1: my_port.hsize = AHB_BYTE;
		                  next_dout= (mask & next_dout);
				  break;
			 case 2: my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 1;
				  next_dout= (mask & next_dout);
				  next_dout >>=8;
				  break;
			 case 3: my_port.hsize = AHB_HALFWORD;
			          next_dout= (mask & next_dout);
				  break;
			 case 4: my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 2;
			          next_dout= (mask & next_dout);
				  next_dout >>=16;
				  break;
			 case 8: my_port.hsize = AHB_BYTE;
			          my_port.haddr = address + 3;
				  next_dout= (mask & next_dout);
				  next_dout >>=24;
				  break;
			 case 12:my_port.hsize = AHB_HALFWORD;
			          my_port.haddr = address + 2;
				  next_dout= (mask & next_dout);
				  next_dout >>=16;
				  break;
			 //set READ DATA
			 default :my_port.hwrite = AHB_READ;
				   my_port.haddr = address;
				   betrue=true;
				   break;
		        }
			my_port.hburst = AHB_SINGLE;
			my_port.htrans = AHB_NONSEQ;
			my_port.hbusreq = false;
		     }
                bezero=false;
	 }
	else //last transitions of burst
          {my_port.htrans = AHB_IDLE;
	   if (undef_incr)
	      {my_port.hbusreq = false;
	       undef_incr=false;}
	   my_port.hlock=false;
	  }


	master_port->ahb_interconnect_write(local_ID, &my_port);

        // Let's signal the input module that we're done with this beat
        wait();
        // Let's wait for the slave to finish writing (it may have already,
        // during the single cycle we were keeping ready_to_wrapper high)
        master_port->ahb_interconnect_read(local_ID, &my_port);
        while (my_port.hready != true)
        {
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        }

        if (STATS && (i == burst - 1))
          statobject->endsAccess(!rw, countbe, SOURCE);


        TRACEX(MASTER_TRACEX, 8, "%s: %s write 0x%08x at address 0x%08x at %10.1f ns\n%s",
          name(), burst == 1 ? "single" : "burst ", uint(dout),
          (uint)prev_address, sc_simulation_time(), (i == burst - 1) ? "\n" : "");

	 trans[id]->completed_words++;
         trans[id]->remaining_words--;

	 } // end for
      }
      else
      // ------------------------------ READ ACCESS ------------------------------
      {
      my_port.haddr = address;
      my_port.hwrite = AHB_READ;

      // Let's request the bus

      master_port->ahb_interconnect_write(local_ID, &my_port);

      // Wait for the grant. Actually, we might get a zero-cycle
      // grant (e.g. if we're the only master on the bus)
      master_port->ahb_interconnect_read(local_ID, &my_port);

      while (!(my_port.hgrant && my_port.hready))
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      }

	if (STATS)
	  statobject->beginsAccess(2,!rw, burst, SOURCE);

      // Here, we really got the bus and pushed out the first address,
      // deassert hbusreq
      if (!undef_incr) my_port.hbusreq = false;

     //compatbility
      master_port->ahb_interconnect_write(local_ID, &my_port);
      do
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      } while (!(my_port.hready));

      
      // spostata sopra 13/09/2005
      //if (STATS)
      //  statobject->beginsAccess(2,!rw, burst, SOURCE);

      // Increment the address for the next beat
      // (if in a single transaction, the address bus isn't ours any more by now)

      prev_address = address;
      address += dimsize;
      my_port.haddr = address;

      if (burst == 1)
	my_port.htrans = AHB_IDLE;
      else
        my_port.htrans = AHB_SEQ;

       master_port->ahb_interconnect_write(local_ID, &my_port);

       for (i = 0; i < burst; i ++)
       {

        do{
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        } while (my_port.hready != true);

       din = my_port.hrdata;
       mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);
       din = (din & mask);
       *(trans[id]->buffer+trans[id]->completed_words)=din;


        trans[id]->completed_words++;
        trans[id]->remaining_words--;

	 TRACEX(MASTER_TRACEX, 8, "%s: %s read  0x%08x at address 0x%08x at %10.1f ns\n%s",
         name(), burst == 1 ? "single" : "burst ", uint(din),
         (uint)prev_address, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
           
        prev_address = address;
	
        // Either ending the burst, or going on
        if (i >= burst - 2)
          {
	  my_port.htrans = AHB_IDLE;
	  if (undef_incr) {my_port.hbusreq = false;undef_incr=false;}
          }
	else
        {
          // Increment the address for the next beat
          address += dimsize;
          my_port.haddr = address;
          my_port.htrans = AHB_SEQ;
        }

        master_port->ahb_interconnect_write(local_ID, &my_port);

	} // end for
	
	if (STATS)
        statobject->endsAccess(!rw, burst, SOURCE);
      }//end read-write
   
    
    trans[id]->status = LX_EXTMEM_COMPLETED; 
    if (STATS) //FIXME should be one cycle later for writes
     statobject->busFreed(SOURCE);
     
   } //end while(1)
   
 }
}; 
#endif
