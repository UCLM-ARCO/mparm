
#include "Slave_axi.h"

using namespace axi_namespace;


Slave_axi::
Slave_axi(sc_module_name Name
      , int ID
      , axi_addr_t StartAddress
      , axi_addr_t EndAddress
      , bool DefaultSlave
      , bool MonitorOn
      , int DataBusBytes
      , double ClockFrequency
      , int WaitState
      )
      : AXI_MasterIFimpl<true>(Name, ID, 1, AXI_IM_SP, DataBusBytes,
                         (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                         1 : DataBusBytes / AXI_WORDSIZE_BYTES, ClockFrequency)
      , SlaveP(ID)
      , m_Name         (Name)
      , m_ID (ID)
      , m_StartAddress (StartAddress)
      , m_EndAddress   (EndAddress)
      , m_DefaultSlave (DefaultSlave)
      , m_MonitorOn    (MonitorOn)
      , m_DataBusBytes (DataBusBytes)
      , m_Reset        (false)
      , dev_wait_state (WaitState)
{

my=m_ID;

  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert((EndAddress + 1 - StartAddress) >= 0x1000);

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_WstrbWords = ((DataBusBytes - 1) >> 5) + 1;
  m_WstrbBytes = m_WstrbWords * AXI_WSTRB_BYTES;
  m_BitMask    = DataBusBytes - 1;

 
  SC_THREAD(SeqSlave);
  this->sensitive << this->Aclk.pos();
  SC_METHOD(Areset_SimpleSlave);
  this->sensitive << this->Aresetn;
  this->dont_initialize();
}

void Slave_axi::SeqSlave()
{  
  bool Verbose=false;
  Achannel A;
  Wchannel W(m_DataBusBytes, m_DataBusWords);
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  RreadySig Rr;
  BreadySig Br;
  uint w=0;

  while(true) {
  if (m_Reset) {
    Reset();
    wait(Aresetn.posedge_event());
  } else {
    Ar.Aready = true; SlaveP.PutChannel(Ar);// set Aready high
    AXI_WAIT_HS(A, A.Avalid); // wait for valid address
    if (!m_Reset) {
      Ar.Aready = false; SlaveP.PutChannel(Ar); // set Aready low
      GetChannel(0, A); // sample the address channel
      int N = 0;           // initialize beat counter
if (Verbose) {
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.Addr
         << " AID=" << A.AID
         << " Awrite=" << A.Awrite << " Alen=" << A.Alen
         << " Asize=" << A.Asize << " Aburst=" << A.Aburst
         << " Alock=" << A.Alock << " Aprot=" << A.Aprot
         << " Acache=" << A.Acache
         << " at " << sc_time_stamp().to_seconds() << endl;
}
      if (A.Awrite) { // write request
        axi_resp_t Eflag = RESP_OKAY;
        do {
  
          Wr.Wready = true; SlaveP.PutChannel(Wr);// set Wready high
          AXI_WAIT_HS(W, W.Wvalid); // wait for Wvalid = high
          if (m_Reset) break;
          Wr.Wready = false; SlaveP.PutChannel(Wr); // set Wready low
          GetChannel(0, W); // sample the write channel
          if (dev_wait_state){
                        for(w=0;w<dev_wait_state;w++){
                                                       wait();}}
                                                       
          if(CURRENT_ISS ==LX){                                             
          //Set data if writestrobe
          if (W.Wstrb[0] != 0xf)
            {direct_read_axi(0,N, A.Addr, &TempWSdata, A.Asize);
             mask = AXI_ByteStrobeMask[W.Wstrb[0]];
             W.Wdata[0]= ((mask & W.Wdata[0])|(~mask & TempWSdata));}
          }
          
          if (!(direct_write_axi(0,N, A.Addr, &W.Wdata[0], A.Asize))) // write data into memory
          Eflag = RESP_OKAY;
if (Verbose) {
    cout << "Slave" << " ID=" << m_ID << " Addr=" << A.GetAddressN(N+1)
         << " WID=" << W.WID
         << " Wdata=" << axi_hexprint(W.Wdata, W.Wstrb, m_DataBusBytes)
         << " Wlast=" << W.Wlast
         << " at " << sc_time_stamp().to_seconds() << endl;
}
          if (N == A.Alen) { // last transfer in a transaction
            B.Bvalid = true; B.BID = W.WID;
            if (A.Alen > 0) B.Bresp = Eflag;
            SlaveP.PutChannel(B); // write to response channel
if (Verbose) {
    cout << "Slave" << " ID=" << m_ID
         << " BID=" << B.BID << " Bresp=" << B.Bresp << " Bvalid=" << B.Bvalid
         << " at " << sc_time_stamp().to_seconds() << endl;
}
            AXI_WAIT_HS(Br, Br.Bready); // wait for Bready = high
            if (m_Reset) break;
            B.Bvalid = false; SlaveP.PutChannel(B); // Set Bvalid low
          } // end of if last branch
   
        } while (N++ < A.Alen);
      } // end of write branch
      else { // read request
 
        do {
        R.Rvalid = false; SlaveP.PutChannel(R); // Set Rvalid low
        if (dev_wait_state){
                  for(w=0;w<dev_wait_state;w++){
                                              wait();}}
          R.Rvalid = true; 
          R.Rlast = (N == A.Alen) ? true : false;
          R.RID = A.AID;
          direct_read_axi(0,N, A.Addr, &R.Rdata[0], A.Asize);
          R.Rresp=RESP_OKAY;
          SlaveP.PutChannel(R);
if (Verbose) {
    axi_wstrb_bt m_Wstrb[4];
    axi_get_wstrb(m_Wstrb, A.GetAddressN(N+1) & m_BitMask,
                 (1 << A.Asize), m_WstrbBytes, m_WstrbWords);
    cout << m_Name << " ID=" << m_ID << " Addr=" << A.GetAddressN(N+1)
         << " RID=" << R.RID 
         << " Rdata=" << axi_hexprint(R.Rdata, m_Wstrb, m_DataBusBytes)
         << " Rlast=" << R.Rlast << " Rresp=" << R.Rresp
         << " at " << sc_time_stamp().to_seconds() << endl;
}

          AXI_WAIT_HS(Rr, Rr.Rready); // wait for Rready = high
          if (m_Reset) break;
        } while (N++ < A.Alen);
        R.Rvalid = false; SlaveP.PutChannel(R); // Set Rvalid low
      } // end of read request
    } // end of if (!m_Reset) branch
  } // end of not Reset branch
  } // end of thread loop
}


 
// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
void Slave_axi::
Reset()
{
  Rchannel R(m_DataBusBytes, m_DataBusWords);
  Bchannel B;
  AreadySig Ar;
  WreadySig Wr;
  Ar.Aready = false; SlaveP.PutChannel(Ar); // set Aready low
  Wr.Wready = false; SlaveP.PutChannel(Wr); // set Wready low
  B.Bvalid = false; SlaveP.PutChannel(B); // Set Bvalid low
  R.Rvalid = false; SlaveP.PutChannel(R); // Set Rvalid low
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
void Slave_axi::
Areset_SimpleSlave()
{
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    m_Reset = true;
  }
}
// ----------------------------------------------------------------------------
// Direct access methods
// ----------------------------------------------------------------------------
bool Slave_axi::GetSlaveParam(int Idx, int& ItrlDepth, AXI_AddressMap& M)
{
  AXI_AddressRegion* mr = new AXI_AddressRegion(m_ID, m_StartAddress,
                                                      m_EndAddress);
  M.PushBack(*mr); delete(mr);
  ItrlDepth = 1;
  if (m_DefaultSlave) M.PutDefaultSlave(m_ID);
  return(true);
}

bool Slave_axi::direct_write_axi(int,
                                int N,
                                unsigned int address,
                                unsigned int* data,
                                int _num_bytes)
{
int num;
uint8_t bw;
      switch (_num_bytes)
      {
        case 0 :  bw=MEM_BYTE;
                  num=0x1;
                  break;
        case 1 :  bw=MEM_HWORD;
                  num=0x2;
                  break;
        case 2 :  bw=MEM_WORD;
                  num=0x4;
                  break;
        default : printf("Fatal error: Slave1 %hu detected a malformed data size \n",m_ID);
                  exit(0);
      }

        unsigned int index = (address-m_StartAddress)+(N*num);

        int offset = (address-m_StartAddress)%4;

        // check the right address data alignment
        switch(_num_bytes) {
                case 4:
                        if(offset) {
                                return false;
                        }
                        break;
                case 3:
                        cerr << " WARNING: read/write of 3 bytes ignored. <" << name() << ">" << endl;
                        return false;
                case 2:
                        if(offset&0x1) {
                                return false;
                        }
                        break;
                default: break;
        }

this->Write((uint32_t)index, (uint32_t)*data, bw);

return true;
}

bool Slave_axi::direct_read_axi(int,
                               int N,
                               unsigned int address,
                               unsigned int* data,
                               int num_bytes)
{
int num;
        switch (num_bytes)
      {
        case 0 :  num=0x1;
                  break;
        case 1 :  num=0x2;
                  break;
        case 2 :  num=0x4;
                  break;
        default : printf("Fatal error: Slave1 %hu detected a malformed data size \n",m_ID);
                  exit(0);
      }

        unsigned int index  = (address-m_StartAddress)+(N*num);
        int offset = (address-m_StartAddress)%4;

// check the right address data alignment
        switch(num_bytes) {
                case 4: if(offset) {
                                return false;
                                }
                        break;
                case 3: cerr << " WARNING: read/write of 3 bytes ignored. <" << name() << ">" << endl;
                        return false;
                case 2: if(offset&0x1) {
                                return false;
                                }
                        break;
                default: break;
                }

*data = this->Read((uint32_t)index);
return true;
}






