#include "Master_axi.h"

using namespace axi_namespace;

Master_axi::
Master_axi(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority // Priority of the master.
               // Must satisfy: Priority >= 0
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system.
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      )
      : AXI_SlaveIFimpl<true>(Name, ID, 1, AXI_IM_SP, DataBusBytes, 
                        (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes / AXI_WORDSIZE_BYTES, ClockFrequency)
      , MasterP        (ID)
      , m_Name         (Name)
      , m_ID           (ID)
      , m_Priority     (Priority)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
{
  
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert(Priority >= 0);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  m_WstrbBytes = m_WstrbWords * AXI_WSTRB_BYTES;
  m_BitMask    = DataBusBytes - 1;
  SC_THREAD(Send);
  this->sensitive << this->Aclk.pos();
  SC_METHOD(Areset_SimpleMaster);
  this->sensitive << this->Aresetn;
  this->dont_initialize();

}

void Master_axi::Send()
{
  bool Verbose=false;
  uint32_t din, dout;
  uint burst=0,n;
  bool wr;
  PINOUT mast_pinout;
  unsigned int buffer[16];
  bool firstwait;

  int i=0;
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  RreadySig Rr;
  BreadySig Br;

  axi_data_bt Data[1024];
  int Len=0;
  axi_asize_t Size;
  axi_wstrb_bt Wstrb;
  int Aid;
  axi_addr_t Addr;
  axi_aburst_t Burst=ABURST_FIXED;
  axi_alock_t Lock;


Aid = m_ID;
my = m_ID;
Wstrb = 0x1;
Lock = ALOCK_NORMAL;
ready_to_wrapper.write(false);

while(true){

do{
     wait();
      }while(request_from_wrapper == false);
      
      // What's up?
      if (STATS)
      statobject->requestsAccess(my);
 
      mast_pinout = pinout.read();
      // Word, half word or byte?
      switch (mast_pinout.bw)
      {
        case 0 :  Size = ASIZE_32;
                  break;
        case 1 :  Size = ASIZE_8;
                  break;
        case 2 :  Size = ASIZE_16;
                  break;
        default : printf("Fatal error: Master detected a malformed data size at time %10.1f\n",
                           sc_simulation_time());
                  exit(0);
      }

      burst = mast_pinout.burst;
      wr = mast_pinout.rw;
      Addr = (unsigned int) addresser->Logical2Physical(mast_pinout.address, m_ID);
      dout = mast_pinout.data;

      if (burst == 1){
        Burst = ABURST_FIXED;
        Len = 0;}
      if (burst == 4){
        Burst = ABURST_INCR;
        Len = 3;}
      if (burst == 8){
        Burst = ABURST_INCR;
        Len = 7;}
      if (burst == 16){
        Burst = ABURST_INCR;
        Len = 15;}

    
if (wr) //write
{
  if (Burst == ABURST_FIXED)   //------------------------Single write transfer
    {
      
Data[0]=dout;
  
if (m_Reset) {
                Reset();
                wait(Aresetn.posedge_event());
               } 
  else {
    // Address channel
    A.Awrite = true; A.Avalid = true; 
    A.Alen = Len;
    A.Asize = Size;
    A.AID = Aid;
    A.Addr = Addr;
    A.Aburst = Burst;
    A.Alock = Lock;
 if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
         << " AID=" << A.AID
         << " Awrite=" << A.Awrite << " Alen=" << A.Alen
         << " Asize=" << A.Asize << " Aburst=" << A.Aburst
         << " Alock=" << A.Alock << " Aprot=" << A.Aprot
         << " Acache=" << A.Acache
         << " at " << sc_time_stamp().to_seconds() << endl;
             }

    MasterP.PutChannel(A);
    
    if (!m_Reset) {
        W.Wlast = true; 
        W.Wvalid = true;
        W.Wdata[0] = Data[0];
        W.WID = A.AID; 
        W.Wstrb[0] = Wstrb;
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
      if(Ar.Aready && A.Avalid==true){
      if (STATS)
     statobject->beginsAccess(1, !wr, burst, my);
    A.Avalid = false; 
    MasterP.PutChannel(A);}
    }
  } while (!Wr.Wready);

        if (m_Reset) break;
                 }

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
      
   ready_to_wrapper.write(true);
        
        if (STATS)
        statobject->endsAccess(!wr, burst, my);
    
        wait();

        ready_to_wrapper.write(false);
        
                } // end of not Reset branch
  } // end of single write
  }  
else  //------------------------- Burst write transfer
    {
buffer[0]=dout;

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
    ready_to_wrapper.write(true);
    n=1;
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
    firstwait=true;
    do {
    if (m_Reset) Ar.Aready = true;
    else {
    wait();

      if(n<burst){
                  mast_pinout = pinout.read();
                  dout = mast_pinout.data;
        if(firstwait=false){
                  buffer[n]=dout;
                  n=n+1;
                        }
        if(n==burst-1){
        ready_to_wrapper.write(false);
                        }
                 }
        
        firstwait=false;
        this->GetChannel(0, Ar);
         }
       } while (!Ar.Aready);
  
  if (STATS)
  statobject->beginsAccess(1, !wr, burst, my);

    A.Avalid = false; MasterP.PutChannel(A);
   
    if (!m_Reset) {
      // Write data channel
      firstwait=0;
      for (i = 0; i < A.Alen+1; i++) {
        if (i == A.Alen) W.Wlast = true; else W.Wlast = false;
        W.Wvalid = true;
        Data[i]=buffer[i];
        W.Wdata[0] = Data[i];
        W.WID = A.AID; W.Wstrb[0] = Wstrb;
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
         else {
                wait();
                if(n<burst){
                            mast_pinout = pinout.read();
                            dout = mast_pinout.data;
                            buffer[n]=dout;
                            n=n+1;
                if(n==burst-1){
                ready_to_wrapper.write(false);
                              }
                           }
                this->GetChannel(0, Wr);
              }
              } while (!Wr.Wready); 
        
        if (m_Reset) break;
      }/// end FOR
    W.Wvalid = false; MasterP.PutChannel(W);
    }//------->dentro c'�il ciclo FOR

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
    ready_to_wrapper.write(true);
    
    if (STATS)
  statobject->endsAccess(!wr, burst, my);  
    
    wait();
    
    ready_to_wrapper.write(false);
      
    } // end of not Reset branch
  } 
  }//end of Burst write
   
   
   }// end of Write


  else {  // read transactions
  if (Burst == ABURST_FIXED)   //------------------------Single read transfer
    {
    

  if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
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
        Rr.Rready = true; MasterP.PutChannel(Rr);
 do {
    if (m_Reset) R.Rvalid = true;
    else {
      wait();
      this->GetChannel(0, R);
      this->GetChannel(0, Ar);
      if(Ar.Aready && A.Avalid==true){
      if (STATS)
     statobject->beginsAccess(1, !wr, burst, my);
    A.Avalid = false; 
    MasterP.PutChannel(A);}
    }
  } while (!R.Rvalid);

Data[0]=R.Rdata[0];

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
        din = Data[0];
        mast_pinout.data = din;

          pinout.write(mast_pinout);
          ready_to_wrapper.write(true);
          if (STATS)
            statobject->endsAccess(!wr, burst, my);
          wait();
 
          ready_to_wrapper.write(false);
    } // end of if (!m_Reset) branch---->dentro c'�il ciclo FOR singolo
  }
  }// end of Single read

else{//-----------------------Burst Read
   if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
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
      ready_to_wrapper.write(false);
      this->GetChannel(0, R);
      this->GetChannel(0, Ar);
      if(Ar.Aready && A.Avalid==true){
      if (STATS)
      if(i==0)
     statobject->beginsAccess(1, !wr, burst, my);
    A.Avalid = false; 
    MasterP.PutChannel(A);}
    }
  } while (!R.Rvalid);


        Data[i]=R.Rdata[0];
        din=Data[i];
        
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

        mast_pinout.data = din;
        pinout.write(mast_pinout);
        if(i<A.Alen)
        ready_to_wrapper.write(true);
      } //end ciclo FOR
      
      Rr.Rready = false; MasterP.PutChannel(Rr);
      ready_to_wrapper.write(true);
      if (STATS)
      statobject->endsAccess(!wr, burst, my);
      
      wait();
      ready_to_wrapper.write(false);
    } // end of if (!m_Reset) branch----->dentro c'�il ciclo FOR
  }
 }//end Burst Read
}//end Read


        if (STATS)
        statobject->busFreed(my);
}//end While(1)
}
// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
 void Master_axi::
Reset()
{
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  RreadySig Rr;
  BreadySig Br;
  A.Avalid = false; MasterP.PutChannel(A); // set Avalid low
  W.Wvalid = false; MasterP.PutChannel(W); // set Wvalid low
  Rr.Rready = false; MasterP.PutChannel(Rr); // Set Rready low
  Br.Bready = false; MasterP.PutChannel(Br); // Set Bready low
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
void Master_axi::
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
bool Master_axi::GetMasterParam(int Idx, int& Priority)
{
  Priority = m_Priority;
  return(true);
}
