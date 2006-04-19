///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_initiator_lx.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         ST_LX STBus initiator interface
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ST_INITIATORLX_H__
#define __ST_INITIATORLX_H__

#include "address.h"
#include "systemc.h"
#include "config.h"
#include "debug.h"
#include "SC_INITIATOR_T3.h"
#include "EXT_MEMORY_TARGET.h"

#include "sim_support.h"

//LX wrapper
#include "ast_iss_if.h"

//maximum number of word for burst transfer
#define MAX_DIM_BURST_LX 8

#include <occn.h>

#define AHBBUS_MAX_WORDS_PER_TRANS  32

//base transaction in the queue fifo
struct AhbBusTransaction
{
  Int8 opcode;                                  // opcode of the transaction
  UInt16 size;                                  // size in words of the transaction (0=free)
  UInt32 address;                               // address of the transaction
  UInt32 buffer[AHBBUS_MAX_WORDS_PER_TRANS];    // temporary buffer
  UInt8 be[AHBBUS_MAX_WORDS_PER_TRANS];         // byte enable
  unsigned int remaining_words,
    completed_words;
  sc_signal<lx_extmem_result> status;
};


template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint TRANS_TABLE_SIZE, uchar SOURCE, bool TRAFFIC_GEN, bool INI_DEBUG>
class STBus_initiator_lx : 
        public INITIATOR_T3<TDATA, TADDRESS, TBE, STBUS_SIZE,
                            TRANS_TABLE_SIZE, SOURCE, TRAFFIC_GEN, INI_DEBUG>,
        public ASTISSIf
{ 
  public:
    
    SC_HAS_PROCESS(STBus_initiator_lx);

    char *type;

    uint mem_id, idx_mem1;

    int totnumbyte;
    uchar nb; 

  QueueObject<int> id_fifo;
  Semaphore new_request;

  UInt32 tmp_buffer[AHBBUS_MAX_WORDS_PER_TRANS];    // temporary buffer for software accesses
  bool tmp_be[AHBBUS_MAX_WORDS_PER_TRANS];          // temporary byteenable array for software accesses

  vector<AhbBusTransaction *> trans;                // array of pending transactions
  
    
    STBus_initiator_lx(sc_module_name nm, unsigned int max_pipeline,
                       ASTISSIf::TRACE trace_value=ASTISSIf::TRACE_NO_ACCESSES,
		       ASTISSIf::DEBUG_LX_WRAPPER debug_value=ASTISSIf::DEBUG_NO_ACCESSES) :
		       
       INITIATOR_T3<TDATA, TADDRESS, TBE, STBUS_SIZE, TRANS_TABLE_SIZE, 
                    SOURCE, TRAFFIC_GEN, INI_DEBUG>(nm),
	
	ASTISSIf(trace_value, debug_value)		    
    {
      type="ST_INITIATOR_LX";

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
    
    

STBus_initiator_lx::~STBus_initiator_lx()
{
  
  for (unsigned int i=0;i<trans.size();i++)
    delete trans[i];
}    



// ***************************** transactions handling functions ****************************

// ------------------------------------------------------------------------------------------
// Function STBus_initiator_lx::GetTransactionId(opcode,size,address)
//
// Return the id of the transaction whose opcode is <opcode> (<opcode> can be either
// OCCN_READ or OCCN_WRITE) and having address <address> and size <size>, or -1 if such
// transaction does not exist.
// ------------------------------------------------------------------------------------------

inline
int STBus_initiator_lx::GetTransactionId(Int8 opcode,UInt16 size,UInt32 address)
{
  unsigned int i = trans.size();

  while (i--)
    if (trans[i]->opcode == opcode && trans[i]->size == size && trans[i]->address == address)
      return i;

  return -1;
}


// ------------------------------------------------------------------------------------------
// Function STBus_initiator_lx::GetFreeTransactionId()
//
// Return the id of a free position in the array of transactions, or -1 if the array is
// full.
// ------------------------------------------------------------------------------------------

inline
int STBus_initiator_lx::GetFreeTransactionId()
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


lx_extmem_result STBus_initiator_lx::_HwDataRead(UInt16 size,
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

#if 0
     for(uint i=0;i<size;i++)
      TRACEX(ST_INITIATOR_LX1, 8,
                  "data:%x\n",buffer[i]);	  
#endif    
    
    }

    return trans[id]->status;
  }      
}




lx_extmem_result STBus_initiator_lx::_HwDataWrite(UInt16 size,
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
		  
      for(uint i=0;i<size;i++)
         TRACEX(ST_INITIATOR_LX1, 9,
                  "data:%x\n",buffer[i]);		       
     
     }
    return trans[id]->status;
  }
}

lx_extmem_result STBus_initiator_lx::_SpecialOperation(lx_memory_special_op_ty operation,UInt16 size,
                                                  UInt32 address,UInt32 *buffer,
                                                  const lx_extmem_attributes *attributes)
{
  // just return LX_EXTMEM_COMPLETED, since no special operations are implemented
  return LX_EXTMEM_COMPLETED;
}





// ***************************** SystemC behaviour implementation ****************************


   int determinenumbyte(uint8_t size)
   {
    int retval=0;

    switch(size)
    {
      case 0x0f: { retval = 4; break;}
      case 0x0e: { retval = 4; break;}
      case 0x0d: { retval = 4; break;}
      case 0x0c: { retval = 2; break;}
      case 0x0b: { retval = 4; break;}
      case 0x0a: { retval = 2; break;}
      case 0x09: { retval = 2; break;}
      case 0x08: { retval = 1; break;}
      case 0x07: { retval = 4; break;}
      case 0x06: { retval = 2; break;}
      case 0x05: { retval = 2; break;}
      case 0x04: { retval = 1; break;}
      case 0x03: { retval = 2; break;}
      case 0x02: { retval = 1; break;}
      case 0x01: { retval = 1; break;}
      case 0x00: { retval = 0; break;}
     default: { printf("Irregular size and Numbytes %d\n",size); break;}
    }
    return retval;
   }

   int log_2(int a)
   {
    if (a == 0)
    {
     cerr << "Fatal Error: Cannot compute the base 2 logarithm of 0" << endl;
     exit(1);
    }

    int res = 0;
    a -= 1;

    while (a)
    {
     res ++;
     a >>= 1;
    }

    return res;
   } 
   
   uchar PREPARE_OP(char op, TADDRESS address, int bemask, int total_bursts, uchar *buffer)
   {
    uchar vtid;
    BETYPE vbe=(BETYPE)bemask;      /*  byte enable mask*/

    ASSERT( (op==0) || (op==1) );   /*  0 : read  -  1: write  */
    ASSERT(bus_width==4); /* 4 bytes */
    ASSERT(total_bursts==1 || total_bursts==2 || total_bursts==4 || total_bursts==8);

     TRACEX(ST_INITIATOR_LX, 10, 
          "SOURCE: %d - "
          "LX_STbus_interface::PREPARE_OP(%d, 0x%x, %d, %d, 0x%x)\n",
          (int)SOURCE, (int)op, (int)address, bemask, total_bursts, (int)buffer);

    if(trtable.full()) {
      cout << " Warning: The request Table is full !" << endl;
      return (TOO_MANY_REQUESTS);
    }
    
    //determine the size of the transfer
    totnumbyte=0;    
    for (int i=0;i<total_bursts;i++)
    {
     totnumbyte+=determinenumbyte(((bemask>>(4*i)) & 0x0000000F ));
    }
   
    if(totnumbyte!=0) nb=log_2(totnumbyte); 
    else nb=0;				
   
    vtid = set_req( (uchar)(op==0?LOAD_M:STORE_M), address, vbe, buffer, totnumbyte, total_bursts, true,
                    false, false);
		    		        
    return vtid;
  }


#define READ(address, size, total_bursts, vtid)                 OPERATION((char)0, address, size, 0, 0,  total_bursts, vtid)
#define WRITE(address, size, data, nburst, total_bursts, vtid)  OPERATION((char)1, address, size, data, nburst, total_bursts, vtid)


  int OPERATION(char op, TADDRESS address, uint32_t bemask, TDATA data, int nburst, int total_bursts, uchar& vtid)
  {
    TBE tbe;         /*  temporary byte enable  */
    BETYPE vbe=bemask;      /*  byte enable  */
    uchar tmp_opc;   /*  temporary opcode  */
    //uchar  nb = 0;   /*  num bytes = 2^n  */
    double t;
    //int totnumbyte=0;

    ASSERT( (op == 0) || (op == 1) );   //  0 : read  -  1: write
    ASSERT( bus_width == 4 );           // 32-bit bus (4 bytes)
    ASSERT(total_bursts==1 || total_bursts==2 || total_bursts==4 || total_bursts==8);

    TRACEX(ST_INITIATOR_LX, 10, "SOURCE:%d - ", (int)SOURCE);
    TRACEX(ST_INITIATOR_LX, 10, 
           "LX_STbus_interface::OPERATION:%d,add0x:%x,be_mask:%x, %d, %d\n",
          (int)op, (int)address, bemask, nburst, total_bursts);

    if(trtable.full()) {
    cout << " Warning: The request Table is full !" << endl;
      return (TOO_MANY_REQUESTS);
    }

    tmp_opc = (nb << 4) | (op==0?LOAD_M:STORE_M);
    tbe = vbe & be_mask;

    t = sc_simulation_time() / CLOCKPERIOD;

    if(op==1)
     {
      send_cell(vtid, address, data, tbe, tmp_opc, true, (op==0?true:nburst==total_bursts-1) );
     }
    else 
     {
      send_cell(vtid, address, data, tbe, tmp_opc, false, (op==0?true:nburst==total_bursts-1) );
     }                                         /* postable */

    return (int)((sc_simulation_time() / CLOCKPERIOD) - t);
  }

  int select_range (int init_bursts)
  {
   int ret_bursts=0;

   if ((init_bursts == 4) || (init_bursts == 2) || (init_bursts == 1))
   ret_bursts=init_bursts;

   if ( (init_bursts < 8) && (init_bursts > 4))
   ret_bursts = 4;

   if ( (init_bursts < 4) && (init_bursts > 2))
   ret_bursts = 2;

   return ret_bursts;
  }


//ovverriding of the base function
void initiator_behav()
{
     TDATA dout;
     TADDRESS address;
     bool rw=false,bezero=false,beginaccess=true;
    uint32_t bemask,mask;
    int bursts = 0;
    int t,messagecount=0,cellcount=0;
    int id;

    TRACEX(ST_INITIATOR_LX, 10,
           "SOURCE:%d - LX_STbus_interface::initiator_behav()\n",
           (int)SOURCE);

    while(1)
    {
      //waiting for new request
      new_request.wait();

      id = id_fifo.remove();
#ifdef OUTDEBUG
//building be concatenation as expected by the stbus node
      unsigned int tmp =0;
       for (unsigned int i = 0; i < trans[id]->remaining_words; i++)
	   tmp = tmp | (trans[id]->be[i]<<(i*4));
#endif
      
      TRACE_CLK_CNT(5);
      TRACEX(ST_INITIATOR_LX, 7,
             "%s:%d rw:%x address:0x%x, tot_words:%d be:%x\n",
	     type,SOURCE,(trans[id]->opcode==OCCN_READ)?1:0,trans[id]->address,trans[id]->remaining_words,tmp);

      ASSERT( (trans[id]->opcode == OCCN_WRITE) || (trans[id]->opcode == OCCN_READ) );
      if (trans[id]->opcode == OCCN_WRITE) rw=true;
      if (trans[id]->opcode == OCCN_READ) rw=false;

         
if (rw) {messagecount=0;cellcount=0;
         for (unsigned int i = 0; i < trans[id]->remaining_words; i++)
           { if (trans[id]->be[i]!=0)
                {messagecount++;cellcount++;
                if ((i+1 < trans[id]->remaining_words) && (trans[id]->be[i+1]!=0))  
                   {i++;cellcount++;}
                 }
	       else bezero=true;
	   }
           
	  if (bezero) SetMessage(messagecount);
         }
               
     while(trans[id]->remaining_words)
     {
      if ((STATS) && ((!(bezero)) || (trans[id]->completed_words==0)))
        statobject->requestsAccess(SOURCE);
     
      address = trans[id]->address + 4*trans[id]->completed_words;
      
      TRACE_CLK_CNT(8);
      TRACEX(ST_INITIATOR_LX, 8, "SOURCE:%d - ", (int)SOURCE);
      TRACEX(ST_INITIATOR_LX, 8,
             "LX_STbus_interface::initiator_behav() - address: 0x%x\n",
             (int)address);

      if(trans[id]->remaining_words >= MAX_DIM_BURST_LX)
        bursts = MAX_DIM_BURST_LX;
	else bursts=select_range(trans[id]->remaining_words);
        if (bursts == 0) {cout<<"BURST SIZE ZERO"<<endl; exit(-1);}
        
       //building be concatenation as expected by the stbus node
       bemask=0;
       for (int i = 0; i < bursts; i++)
          bemask = bemask | (trans[id]->be[i]<<(i*4));

      // WRITING
      // WRITE opcode
      if(rw)
      {
        uchar vtid;
        TRACE_CLK_CNT(8);

        TRACEX(ST_INITIATOR_LX, 8,
               "SOURCE:%d - LX_STbus_interface::initiator_behav() - writing\n",
               (int)SOURCE);

         if (trans[id]->completed_words==0) beginaccess=true;
         
	 if (trans[id]->be[trans[id]->completed_words]!=0) 
	 {
             dout=(*(trans[id]->buffer+trans[id]->completed_words));
             if (bezero)
	     {bursts=1;
              bemask =trans[id]->be[trans[id]->completed_words];
	      
	      /*
	      // POLETTI FIXME ATTENZIONE
	      // CHE POTREBBE CERCARE DI FARE 
	      // DEI BURSTS A DEGLI INDIRIZZI NON ALLINEATI
              
	      if (((trans[id]->completed_words+bursts) < trans[id]->remaining_words) && 
                             (trans[id]->be[trans[id]->completed_words+bursts]!=0))  
                             {bemask = bemask | ((trans[id]->be[trans[id]->completed_words+bursts]<<(bursts*4)));
                              bursts++;
                             }
              */
	     }
	     
	      vtid = PREPARE_OP((char)1, address, bemask, bursts, (uchar*)&dout);
              
	TRACEX(ST_INITIATOR_LX, 8,
                 "INITIATOR:%d start_write "
                 "addr:%x,be:%x,tot_burst:%d,vtid:%d\n",
	         SOURCE, address,bemask,bursts,vtid);

        if(bursts==1)
	{
           t = WRITE(address, bemask, (TDATA)dout, 0, bursts, vtid);
           
           if  ((STATS) && beginaccess) {
              statobject->beginsAccess(1, !rw, cellcount, SOURCE);
              beginaccess=false;
              }

         }
        else
	{
          for (int i = 0; i < bursts; i++)
          {
	   bemask=0;
	   bemask=trans[id]->be[trans[id]->completed_words];
	   t = WRITE(address, bemask, (TDATA)dout, i, bursts, vtid);
           
           
           //I'm not using the message and this is the first burst transaction
           //I'm using the message and this is the first transaction
	   if (STATS && ((!(bezero)) && (i==0)) || ((bezero)&&(beginaccess))) {
            statobject->beginsAccess(1, !rw, cellcount, SOURCE);
            beginaccess=false;
            }
            
           // cell sent
           if (i < bursts - 1)
           {
	    trans[id]->remaining_words--;
	    trans[id]->completed_words++;
            dout=(*(trans[id]->buffer+trans[id]->completed_words));
	   }
          }

        }//end of burst
        
       }//end of be==0
       trans[id]->completed_words++;
       trans[id]->remaining_words--;
      }

      // READING
      else
      {
        uchar vtid;
        uchar buf[4*16];   /* max 16 bursts, 4 bytes each */

        TRACE_CLK_CNT(8);
        TRACEX(ST_INITIATOR_LX, 8,
               "SOURCE:%d - LX_STbus_interface::initiator_behav() - reading\n",
               (int)SOURCE);

        vtid = PREPARE_OP((char)0, address, bemask, bursts, buf);

        TRACEX(ST_INITIATOR_LX, 8,
                 "INITIATOR:%d start_read "
                 "addr:%x,be:%x,burst:%d,vtid:%d\n",
                 SOURCE, address,bemask,bursts,vtid);

	t = READ(address, bemask, bursts, vtid);

        for (int i = 0; i < bursts; i++)
        {
          TRACE_CLK_CNT(8);
          TRACEX(ST_INITIATOR_LX, 8, "SOURCE:%d - ", (int)SOURCE);
          TRACEX(ST_INITIATOR_LX, 8,
                 "LX_STbus_interface::initiator_behav() - burst read #%d\n",
                 i);

          if (STATS && (i == 0))
            statobject->beginsAccess(1, !rw, bursts, SOURCE);

          while ( !(Out.r_req) || ((Out.r_tid)&0x0f)!=vtid )  /*  KLUDGE!! */
            ::wait();

          // cell received
          dout=*((TDATA*)(buf+bus_width*i));

          TRACEX(ST_INITIATOR_LX, 8, "data:%x - ", dout);

          mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);

          dout =(dout & mask);

          *(trans[id]->buffer+trans[id]->completed_words)=dout;

          TRACE_CLK_CNT(8);
          TRACEX(ST_INITIATOR_LX, 8, "SOURCE:%x - ", dout);

          trans[id]->completed_words++;
          trans[id]->remaining_words--;
          if (i < bursts - 1)
          ::wait();   // Time to let r_req go down to 0

        }
      }
      
 if ((STATS) && ((!(bezero)) || (trans[id]->remaining_words==0)))
        {
         if (rw) statobject->endsAccess(!rw, cellcount, SOURCE);
         else statobject->endsAccess(!rw, bursts, SOURCE);        
         statobject->busFreed(SOURCE);

         }
    
    } //end of while

    
    trans[id]->status = LX_EXTMEM_COMPLETED;
    bezero=false;
    
    } // end while(1)
  }

};

#endif // __ST_INITIATOR_H__
