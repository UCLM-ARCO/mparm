///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         AXI_initiator_lx.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AXI_LX AXI initiator interface
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AXI_INITIATORLX_H__
#define __AXI_INITIATORLX_H__

#ifdef __EDG__
#define axi_namespace std
#endif

#include "debug.h"

#include "config.h"
#include "globals.h"
#include "address.h"
#include "stats.h"
#include "axi_master_port.h"
#include "axi_slave_if_impl.h"

#include "sim_support.h"
#include "ast_iss_if.h"

//maximum number of word for burst transfer
#define MAX_DIM_BURST_AHB 16

//Preso da ahbbus_adapter, utilizziamo una queue OCCN per le transazioni pendenti
#include <occn.h>

#define AHBBUS_MAX_WORDS_PER_TRANS  32
#undef INI_DEBUG


using namespace axi_namespace;

struct AxiBusTransaction
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

class AXIBus_initiator_lx :
          public ASTISSIf
        , public axi_namespace::AXI_SlaveIFimpl<true>
#ifdef __EDG__
        , public sc_interface
#endif



{
  public:
  SC_HAS_PROCESS(AXIBus_initiator_lx);
  axi_namespace::AXI_MasterPort MasterP;
  
  private:  
  char *type;
  uint mem_id, idx_mem1;
  QueueObject<int> id_fifo;
  Semaphore new_request;

  // parameter
  const char* m_Name;
  bool m_Reset;
  int m_ID;
  int m_Priority;
  int m_DataBusBytes;
  
  // constants
  int m_DataBusWords; // Width of the data bus in units of word.
                      // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                      // A word is 4 bytes for unsigned int
  int m_WstrbWords;
  int m_WstrbBytes;
  int m_BitMask;
  
      
  UInt32 tmp_buffer[AHBBUS_MAX_WORDS_PER_TRANS];    // temporary buffer for software accesses
  bool tmp_be[AHBBUS_MAX_WORDS_PER_TRANS];          // temporary byteenable array for software accesses
  
  vector<AxiBusTransaction *> trans;                // array of pending transactions
    
  public:
  AXIBus_initiator_lx(sc_module_name nm, unsigned int max_pipeline
                      , int Priority // Priority of the master.
                      // Must satisfy: Priority >= 0
                      , int DataBusBytes // Number of byte lanes of the AXI IC.
                      // The same number must be used in the entire AXI system.
                      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
                      // in units of cycles per second. This value is only used
                      // for NegEdge = false, i.e., if the negative clock edge
                      // toggling scheme is not used.
                      ,ASTISSIf::TRACE trace_value=ASTISSIf::TRACE_NO_ACCESSES,
		       ASTISSIf::DEBUG_LX_WRAPPER debug_value=ASTISSIf::DEBUG_NO_ACCESSES
                     ) :

	  ASTISSIf(trace_value, debug_value)
          
        , AXI_SlaveIFimpl<true>(nm, SOURCE, 1, AXI_IM_SP, DataBusBytes, 
                        (DataBusBytes< AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes/ AXI_WORDSIZE_BYTES, ClockFrequency)
        
        , MasterP        (SOURCE)
        , m_Name         (nm)
        , m_Reset        (false)
        , m_ID           (SOURCE)        
        , m_Priority     (Priority)
        , m_DataBusBytes (DataBusBytes)
    {
    
  sc_assert((0 <= SOURCE) && (SOURCE < AXI_MAX_PORT_SIZE));
  sc_assert(Priority >= 0);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  m_WstrbBytes = m_WstrbWords * AXI_WSTRB_BYTES;
  m_BitMask    = DataBusBytes - 1;
  
  SC_THREAD(initiator_behav);
  this->sensitive << this->Aclk.pos();
  SC_METHOD(Areset_SimpleMaster);
  this->sensitive << this->Aresetn;
  this->dont_initialize();
  
  type="AXI_INITIATOR_LX";

//inizializzazione della coda Fifo dal codice del costruttore di ahhbus_adapter
  for (unsigned int i=0;i<max_pipeline;i++)
  {
    AxiBusTransaction* tmp = new AxiBusTransaction;
    tmp->size = 0;
    trans.push_back(tmp);
  }


     TRACEX(ST_INITIATOR_LX, 7,
             "%s:%d sc_name:%s \n",
             type,(int)SOURCE, (const char*)nm);
    
    }


AXIBus_initiator_lx::~AXIBus_initiator_lx()
{
//eliminazione della coda Fifo dal codice del distruttore di ahhbus_adapter

  for (unsigned int i=0;i<trans.size();i++)
    delete trans[i];
}

private:
// ***************************** transactions handling functions ****************************

// ------------------------------------------------------------------------------------------
// Function AXIBus_initiator_lx::GetTransactionId(opcode,size,address)
//
// Return the id of the transaction whose opcode is <opcode> (<opcode> can be either
// OCCN_READ or OCCN_WRITE) and having address <address> and size <size>, or -1 if such
// transaction does not exist.
// ------------------------------------------------------------------------------------------

inline
int AXIBus_initiator_lx::GetTransactionId(Int8 opcode,UInt16 size,UInt32 address)
{
  unsigned int i = trans.size();

  while (i--)
    if (trans[i]->opcode == opcode && trans[i]->size == size && trans[i]->address == address)
      return i;

  return -1;
}


// ------------------------------------------------------------------------------------------
// Function AXIBus_initiator_lx::GetFreeTransactionId()
//
// Return the id of a free position in the array of transactions, or -1 if the array is
// full.
// ------------------------------------------------------------------------------------------

inline
int AXIBus_initiator_lx::GetFreeTransactionId()
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


lx_extmem_result AXIBus_initiator_lx::_HwDataRead(UInt16 size,
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
           << "Error: Too many pipelined transactions in AxiBusAdapter::_HwDataRead()" << endl
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




lx_extmem_result AXIBus_initiator_lx::_HwDataWrite(UInt16 size,
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
           << "Error: Too many pipelined transactions in AxiBusAdapter::_HwDataWrite()" << endl
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

lx_extmem_result AXIBus_initiator_lx::_SpecialOperation(lx_memory_special_op_ty operation,UInt16 size,
                                                  UInt32 address,UInt32 *buffer,
                                                  const lx_extmem_attributes *attributes)
{
  // just return LX_EXTMEM_COMPLETED, since no special operations are implemented
  return LX_EXTMEM_COMPLETED;
}


  // ------------------------------------------------------------------------
  // ------------------------------ BEHAVIOR --------------------------------
  // ------------------------------------------------------------------------
  //ovverriding of the base function

  
void AXIBus_initiator_lx::initiator_behav()
{
    bool wr=false,Verbose=false,bezero,FirstSTATS;
    uint32_t mask;
    uint id,burst,datacount=0;

    Achannel A;
    Wchannel W(m_DataBusBytes, m_DataBusWords);
    Rchannel R(m_DataBusBytes, m_DataBusWords);
    Bchannel B;
    AreadySig Ar;
    WreadySig Wr;
    RreadySig Rr;
    BreadySig Br;
     
    int Aid=SOURCE,Len=0,i=0;
    axi_asize_t Size = ASIZE_32;
    axi_addr_t Addr;
    axi_aburst_t Burst=ABURST_FIXED;
    axi_alock_t Lock;
    
    while(1)
    {
     //waiting for new request
     new_request.wait();
     id = id_fifo.remove();
     //wait inserita per annullare gli effetti del aumento del delta cycle inserita dalla FIFO
     wait();

     if (STATS)
        statobject->requestsAccess(SOURCE);
      
     burst=trans[id]->remaining_words;
  
     if (burst == 1)
        {Burst = ABURST_FIXED;
        Len = 0;}
     else {Burst = ABURST_INCR;
           Len = burst-1;}
     
     ASSERT( (trans[id]->opcode == OCCN_WRITE) || (trans[id]->opcode == OCCN_READ) );
     if (trans[id]->opcode == OCCN_WRITE) wr=true;
     if (trans[id]->opcode == OCCN_READ)  wr=false;
     
     bezero=false;
     if (wr) {
	      datacount=0;
              for (unsigned int i = 0; i < trans[id]->remaining_words; i++)
               { if (trans[id]->be[i] != 0) datacount++;
	         else bezero=true;
	       }
              }
     
	  if (bezero) {
                       Lock = ALOCK_LOCKED; 
                       Burst = ABURST_FIXED;
                       Len = 0;
                       burst=datacount;}
           else Lock = ALOCK_NORMAL;

         
     Addr = trans[id]->address;
     
     if (wr) //write
     {
      
      if (Burst == ABURST_FIXED)   //------------------------Single write transfer
        {
        datacount=0;FirstSTATS=true;
        while(trans[id]->remaining_words)
        {
         if (trans[id]->be[trans[id]->completed_words]!=0)
         {
         if (m_Reset) {
                      Reset();
                      wait(Aresetn.posedge_event());
                      }
         else {
             // Address channel
	     A.Awrite = true; 
             A.Avalid = true; 
	     A.Alen = Len;
	     A.Asize = Size;
	     A.AID = Aid;
	     A.Addr = Addr+4*trans[id]->completed_words;
	     A.Aburst = Burst;
             if (datacount==burst-1) Lock = ALOCK_NORMAL;
	     A.Alock = Lock;

             if (Verbose) {
                cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
                << " AID=" << A.AID
		<< " Awrite=" << A.Awrite << " Alen=" << A.Alen
		<< " Asize=" << A.Asize << " Aburst=" << A.Aburst
		<< " Alock=" << A.Alock << " Aprot=" << A.Aprot
		<< " Acache=" << A.Acache << " Avalid=" << A.Avalid
		<< " at " << sc_time_stamp().to_seconds() << endl;
                }
     
             MasterP.PutChannel(A);
    
             if (!m_Reset) {
                W.Wlast = true; 
		W.Wvalid = true;
                W.Wdata[0] =(*(trans[id]->buffer+trans[id]->completed_words));
		W.WID = A.AID; 
		W.Wstrb[0] = trans[id]->be[trans[id]->completed_words];
		MasterP.PutChannel(W);

                if (Verbose) {
                   cout << m_Name << " ID=" << m_ID << " Addr=" << A.GetAddressN(1)
		   << " WID=" << W.WID
		   << " Wdata=" << axi_hexprint(W.Wdata, W.Wstrb, m_DataBusBytes)
		   << " Wlast=" << W.Wlast
		   << " at " << sc_time_stamp().to_seconds() << endl;
                   }
            
                do {
		 if (m_Reset) Wr.Wready = true;
                 
		 else {
		      wait();
		      this->GetChannel(0, Wr);
		      this->GetChannel(0, Ar);
		      if(Ar.Aready && A.Avalid==true)
                      {
		       if ((STATS) && (FirstSTATS)){FirstSTATS=false;
		          statobject->beginsAccess(1, !wr, burst, SOURCE);}
		      datacount++;
                      A.Avalid = false;
		      MasterP.PutChannel(A);}
		      }
                } while (!Wr.Wready);

                if (m_Reset) break;
             }//!m_Reset
	     
	     W.Wvalid = false; 
             MasterP.PutChannel(W);
      
             if (!m_Reset) {
                // Buffered response channel
                Br.Bready = true; MasterP.PutChannel(Br);
                AXI_WAIT_HS(B, B.Bvalid); // wait for Bvalid
                
                
                if (Verbose) {
                   if (!m_Reset)
                      cout << m_Name << " ID=" << m_ID
                      << " BID=" << B.BID << " Bresp=" << B.Bresp
                      << " at " << sc_time_stamp().to_seconds() << endl;
                }
                
                Br.Bready = false; 
                MasterP.PutChannel(Br);
	        }
	     } // end of not Reset branch
            } //end if be==0    
            trans[id]->completed_words++;
            trans[id]->remaining_words--;
 
            if ((STATS) && (trans[id]->remaining_words==0))
                statobject->endsAccess(!wr, burst, SOURCE);
        
          }//end of remaningword
        } // end of single write
	else  //------------------------- Burst write transfer
        {
                
	if (m_Reset) {
           Reset();
           wait(Aresetn.posedge_event());
           } 
        else {
          // Address channel
          A.Awrite = true; 
          A.Avalid = true; 
          A.Alen = Len; 
          A.Asize = Size; 
          A.AID = Aid; 
          A.Addr = Addr;
          A.Aburst = Burst; 
          A.Alock = Lock;
          MasterP.PutChannel(A);
          
          if (Verbose) {
            cout << "Master" << " ID=" << m_ID << " Addr=" << A.Addr
            << " AID=" << A.AID
            << " Awrite=" << A.Awrite << " Alen=" << A.Alen
            << " Asize=" << A.Asize << " Aburst=" << A.Aburst
            << " Alock=" << A.Alock << " Aprot=" << A.Aprot
            << " Acache=" << A.Acache
            << " at " << sc_time_stamp().to_seconds() << endl;
            }
            
         // wait for Aready high
         do {
            if (m_Reset) Ar.Aready = true;
            else { wait();
                    this->GetChannel(0, Ar);
                  }
        } while (!Ar.Aready);  
        
        if (STATS)
           statobject->beginsAccess(1, !wr, burst, SOURCE);

        A.Avalid = false; MasterP.PutChannel(A);
   
        if (!m_Reset) {
           // Write data channel
           for (i = 0; i < A.Alen+1; i++) {
              if (i == A.Alen) W.Wlast = true; else W.Wlast = false;
              W.Wvalid = true;
	      W.Wdata[0]=(*(trans[id]->buffer+i));
	      W.Wstrb[0]=trans[id]->be[i];
              W.WID = A.AID; 
              MasterP.PutChannel(W);

              if (Verbose) {
                 cout << "Master" << " ID=" << m_ID << " Addr=" << A.GetAddressN(i+1)
                 << " WID=" << W.WID
                 << " Wdata=" << axi_hexprint(W.Wdata, W.Wstrb, m_DataBusBytes)
                 << " Wlast=" << W.Wlast
                 << " at " << sc_time_stamp().to_seconds() << endl;
                 }
      
              do {
                 if (m_Reset) Ar.Aready = true;
                 else { wait();
                         this->GetChannel(0, Wr);
                       }
                     
              } while (!Wr.Wready); 
        
              if (m_Reset) break;
          }/// end FOR
          W.Wvalid = false; 
          MasterP.PutChannel(W);
      }//------->dentro c' è il ciclo FOR
      
      if (!m_Reset) {
         // Buffered response channel
         Br.Bready = true; MasterP.PutChannel(Br); 
         AXI_WAIT_HS(B, B.Bvalid); // wait for Bvalid
      
         if (Verbose) {
            if (!m_Reset)
               cout << "Master" << " ID=" << m_ID
               << " BID=" << B.BID << " Bresp=" << B.Bresp
               << " at " << sc_time_stamp().to_seconds() << endl;
         }
         
         Br.Bready = false; MasterP.PutChannel(Br);
    
         if (STATS)
            statobject->endsAccess(!wr, burst, SOURCE);  
      
         } // end of not Reset branch
      }
    }//end of Burst write
   
   }// end of Write

   else {      // read transactions
     if (Burst == ABURST_FIXED)   //------------------------Single read transfer
        {    
    
        if (m_Reset) {
           Reset();
           wait(Aresetn.posedge_event());
           } 
	else {
           // Address channel
           A.Awrite = false; 
           A.Avalid = true; 
           A.Alen = Len; 
           A.Asize = Size; 
           A.AID = Aid; 
           A.Addr = Addr;
           A.Aburst = Burst; 
           A.Alock = Lock;
           MasterP.PutChannel(A);
           
           if (Verbose) {
              cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
              << " AID=" << A.AID
              << " Awrite=" << A.Awrite << " Alen=" << A.Alen
              << " Asize=" << A.Asize << " Aburst=" << A.Aburst
              << " Alock=" << A.Alock << " Aprot=" << A.Aprot
              << " Acache=" << A.Acache
              << " at " << sc_time_stamp().to_seconds() << endl;
              }
    
           if (!m_Reset) {
              // read data channel
              Rr.Rready = true; 
	      MasterP.PutChannel(Rr);
              do {
                 if (m_Reset) R.Rvalid = true;
                 else {
                       wait();
                       this->GetChannel(0, R);
                       this->GetChannel(0, Ar);
                       if(Ar.Aready && A.Avalid==true){
                         if (STATS)
                            statobject->beginsAccess(1, !wr, burst, SOURCE);
                         A.Avalid = false; 
                         MasterP.PutChannel(A);
			 }
                       }
              } while (!R.Rvalid);

	      mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);
              *(trans[id]->buffer+trans[id]->completed_words)=(mask & R.Rdata[0]);
              
	      if (m_Reset) break;
              if (Verbose) {
                  axi_wstrb_bt m_Wstrb[4];
                  axi_get_wstrb(m_Wstrb, A.GetAddressN(i+1) & m_BitMask,
                  (1 << A.Asize), m_WstrbBytes, m_WstrbWords);
                  cout << m_Name << " ID=" << m_ID
                  << " RID=" << R.RID 
                  << " Rdata=" << axi_hexprint(R.Rdata, m_Wstrb, m_DataBusBytes)
                  << " Rlast=" << R.Rlast << " Rresp=" << R.Rresp
                  << " at " << sc_time_stamp().to_seconds() << endl;
                  }
                  
              Rr.Rready = false; MasterP.PutChannel(Rr);
                              
		trans[id]->completed_words++;
                trans[id]->remaining_words--;
	      
	      if (STATS)
                 statobject->endsAccess(!wr, burst, SOURCE);
              
         } // end of if (!m_Reset) branch---->dentro c'e'il ciclo FOR singolo
        }
       }// end of Single read

      else{//-----------------------Burst Read     
         if (m_Reset) {
            Reset();
            wait(Aresetn.posedge_event());
            }
         else {
           // Address channel
           A.Awrite = false; 
           A.Avalid = true; 
           A.Alen = Len; 
           A.Asize = Size; 
           A.AID = Aid; 
           A.Addr = Addr;
           A.Aburst = Burst; 
           A.Alock = Lock;
           MasterP.PutChannel(A);
           
           if (Verbose) {
             cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
             << " AID=" << A.AID
             << " Awrite=" << A.Awrite << " Alen=" << A.Alen
             << " Asize=" << A.Asize << " Aburst=" << A.Aburst
             << " Alock=" << A.Alock << " Aprot=" << A.Aprot
             << " Acache=" << A.Acache
             << " at " << sc_time_stamp().to_seconds() << endl;
             }
    
           if (!m_Reset) {
            // read data channel
            for (i = 0; i < A.Alen+1; i++) {
                Rr.Rready = true; MasterP.PutChannel(Rr);
                // wait for Rvalid
               do {
                  if (m_Reset) R.Rvalid = true;
                  else {
                       wait();
                       this->GetChannel(0, R);
                       this->GetChannel(0, Ar);
                       if(Ar.Aready && A.Avalid==true){
                          if (STATS)
                             if(i==0)
                               statobject->beginsAccess(1, !wr, burst, SOURCE);
                          A.Avalid = false; 
                          MasterP.PutChannel(A);
			  }
                  }
               } while (!R.Rvalid);
               
	       mask = GetByteEnableMask(trans[id]->be[trans[id]->completed_words]);
               *(trans[id]->buffer+trans[id]->completed_words)=(mask & R.Rdata[0]);
              
               if (m_Reset) break;
               
               if (Verbose) {
                  axi_wstrb_bt m_Wstrb[4];
                  axi_get_wstrb(m_Wstrb, A.GetAddressN(i+1) & m_BitMask,
                  (1 << A.Asize), m_WstrbBytes, m_WstrbWords);
                  cout << m_Name << " ID=" << m_ID
                  << " RID=" << R.RID 
                  << " Rdata=" << axi_hexprint(R.Rdata, m_Wstrb, m_DataBusBytes)
                  << " Rlast=" << R.Rlast << " Rresp=" << R.Rresp
                  << " at " << sc_time_stamp().to_seconds() << endl;
                  }    

		trans[id]->completed_words++;
                trans[id]->remaining_words--; 

           } //end ciclo FOR
      
           Rr.Rready = false; MasterP.PutChannel(Rr);
           
           if (STATS)
              statobject->endsAccess(!wr, burst, SOURCE);
           
           } // end of if (!m_Reset) branch----->dentro c'e'il ciclo FOR
         }
      }//end Burst Read
    }//end Read

    trans[id]->status = LX_EXTMEM_COMPLETED; 
    
    if (STATS)
       statobject->busFreed(SOURCE);
 
 }//end While(1)
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
 void AXIBus_initiator_lx::
Reset()
{
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  RreadySig Rr;
  BreadySig Br;
  A.Avalid = false; MasterP.PutChannel(A);	// set Avalid low
  W.Wvalid = false; MasterP.PutChannel(W);	// set Wvalid low
  Rr.Rready = false; MasterP.PutChannel(Rr);	// Set Rready low
  Br.Bready = false; MasterP.PutChannel(Br);	// Set Bready low
}


// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
void AXIBus_initiator_lx::
Areset_SimpleMaster()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_Reset = true;
  }
}


// ----------------------------------------------------------------------------
// Parameter and direct access methods
// ----------------------------------------------------------------------------
bool AXIBus_initiator_lx::GetMasterParam(int Idx, int& Priority)
{
  Priority = m_Priority;
  return(true);
}

};
#endif
