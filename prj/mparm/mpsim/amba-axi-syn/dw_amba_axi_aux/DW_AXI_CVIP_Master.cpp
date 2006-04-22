// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 10/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 master example.
//    - Example master using the generic AXI_CVIP_Master class
//      that implements the VIP AXI master command IF
// ============================================================================

#include "DW_AXI_CVIP_Master.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_CVIP_Master<NegEdge, Verbose>::
DW_AXI_CVIP_Master(sc_module_name Name
      , int ID
      , int Priority
      , axi_addr_t Addr1
      , axi_addr_t Addr2
      , axi_addr_t Addr3
      , axi_addr_t Addr4
      , int DataBusBytes
      , double ClockFrequency
      , int MaxXactBuffers
      , int MaxIntlBuffers
      , int AID_Width
      , int MaxRequests
      , int WriteIntlDepth
      , bool RreadyDefault
      , bool BreadyDefault)
      : AXI_CVIP_Master<NegEdge, Verbose>(
          Name, ID, DataBusBytes,
          (DataBusBytes < AXI_WORDSIZE_BYTES) ?
          1 : DataBusBytes / AXI_WORDSIZE_BYTES,
          ClockFrequency,
          MaxXactBuffers, MaxIntlBuffers,
          AID_Width, MaxRequests, WriteIntlDepth,
          RreadyDefault, BreadyDefault,
          0, // RreadyDelayP
          0, // BreadyDelayP
          0, // AvalidWvalidDelayP
          0, // NextAvalidDelayP
          0, // NextWvalidDelayP
          0, // RvalidRreadyDelayP
          0 // BvalidBreadyDelayP
          )
      , m_ID           (ID)
      , m_Priority     (Priority)
      , m_Addr1        (Addr1)
      , m_Addr2        (Addr2)
      , m_Addr3        (Addr3)
      , m_Addr4        (Addr4)
      , m_DataBusBytes (DataBusBytes)
      , m_IntlDepth    (WriteIntlDepth)
      , m_Reset        (false)
{
  sc_assert((0 <= ID) && (ID < AXI_MAX_PORT_SIZE));
  sc_assert(Priority >= 0);
  sc_assert(WriteIntlDepth >= 1);

  m_Bh = new int[MaxXactBuffers + MaxIntlBuffers];
  m_Ch = new int[MaxRequests];
  m_Rh = new int[MaxXactBuffers + MaxIntlBuffers*128];

  SC_THREAD(Master);
  this->sensitive << this->Aclk.pos();
  this->dont_initialize();
  SC_METHOD(AresetMs);
  this->sensitive << this->Aresetn;
  this->dont_initialize();

  Initialize();

  // create the buffers that will be sent to the slave
  sc_assert(MaxXactBuffers >= 6);
}
template <bool NegEdge, bool Verbose> DW_AXI_CVIP_Master<NegEdge, Verbose>::
~DW_AXI_CVIP_Master()
{
  delete [] m_Bh; delete [] m_Ch; delete [] m_Rh;
}

template <bool NegEdge, bool Verbose> void DW_AXI_CVIP_Master<NegEdge, Verbose>::
Master()
{
while (true) {
  if (!m_Reset) {
    if (m_CreateBuffer) {
      CreateBuffer();
      m_CreateBuffer = false;
    }
    // put buffer in queue
    send_xact(0, m_Bh[0], m_Ch[0]);
    send_xact(0, m_Bh[1], m_Ch[1]);
    send_xact(0, m_Bh[2], m_Ch[2]);
    send_xact(0, m_Bh[3], m_Ch[3]);
    send_xact(0, m_Bh[4], m_Ch[4]);
    send_xact(0, m_Bh[5], m_Ch[5]);
    for (int i = 0; i < 10; i++)
      send_xact(0, m_Bh[6], m_Ch[6 + i]);

    // get results, delete response buffer
    get_result(0, m_Ch[4], m_Rh[4]);
    delete_buffer(m_Rh[4]);
    get_result(0, m_Ch[3], m_Rh[3]);
    delete_buffer(m_Rh[3]);
    // interleave buffer
    for (int i = 0; i < 5; i++) {
      get_interleave_result(0, m_Ch[5], i, m_Rh[5 + i]);
      delete_buffer(m_Rh[5 + i]);
    }
    // get buffer handle, to delete the intl buffer
    get_result(0, m_Ch[5], m_Rh[11]);
    delete_buffer(m_Rh[11]);

    get_result(0, m_Ch[2], m_Rh[2]);
    delete_buffer(m_Rh[2]);
    get_result(0, m_Ch[1], m_Rh[1]);
    delete_buffer(m_Rh[1]);
    get_result(0, m_Ch[0], m_Rh[0]);
    delete_buffer(m_Rh[0]);
    for (int i = 0; i < 10; i++) {
      get_result(0, m_Ch[6 + i], m_Rh[20+i]);
      delete_buffer(m_Rh[20+i]);
    }
  } // end of (!m_Reset) branch
  else {
    wait(m_ResetEvent);
    cout << "Reset" << endl;
  }
}
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_CVIP_Master<NegEdge, Verbose>::
Initialize()
{
  m_CreateBuffer = true;
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_CVIP_Master<NegEdge, Verbose>::
AresetMs()
{
  if (Aresetn) {
    m_Reset = false;
    m_ResetEvent.notify();
    if (Verbose) cout << name() << ": Reset End" << endl;
  }
  else {
    Initialize();
    m_Reset = true;
    if (Verbose) cout << name() << ": Reset Start" << endl;
  }
}

// ----------------------------------------------------------------------------
// Parameter and direct access methods
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_CVIP_Master<NegEdge, Verbose>::
GetMasterParam(int Idx, int& Priority)
{
  Priority = m_Priority;
  return(true);
}

// ----------------------------------------------------------------------------
// End of elaboration method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_CVIP_Master<NegEdge, Verbose>::
end_of_elaboration()
{
  AXI_AddressMap M;
  int it;
  sc_module::end_of_elaboration();
  MasterP.GetSlaveParam(it, M);
  if (it < m_IntlDepth) {
    m_IntlDepth = it;
    sc_assert(m_IntlDepth >= 1);
    AXI_CVIP_Master<NegEdge, Verbose>::m_WriteIntlDepth = m_IntlDepth;
  }
}

// ----------------------------------------------------------------------------
// Create buffers
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose>
void DW_AXI_CVIP_Master<NegEdge, Verbose>::CreateBuffer()
{
  // create 6 XACT buffer, we create 1 XACT write buffer and use copy_buffer
  // to create a total of 4 write buffer
  // we also create 2 read buffer
  new_buffer(DW_VIP_AXI_XACT, m_Bh[0]);
  for (int i = 3; i < 5; i++) {
    new_buffer(DW_VIP_AXI_XACT, m_Bh[i]);
  }

  // fill XACT buffer with data

  // set buffer type: read, write, or expected read
  set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_XACT_TYPE, 0, DW_VIP_AXI_XACT_WRITE);
  set_buffer_attr_int(m_Bh[3], DW_VIP_AXI_XACT_TYPE, 0,
                      DW_VIP_AXI_XACT_READ_EXPECT);
  set_buffer_attr_int(m_Bh[4], DW_VIP_AXI_XACT_TYPE, 0,
                      DW_VIP_AXI_XACT_READ_EXPECT);

  // set Achannel data
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_AWRITE, 0, "0b001");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ALEN, 0, "0x007");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ASIZE, 0, "0x00");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ABURST, 0, "0b001");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ALOCK, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ACACHE, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_APROT, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_AID, 0, "0x001");
  set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_ADDR, 0, m_Addr1);
  for (int i = 0; i < 16; i++) {
    set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_BUFFER_DATA, i, i);
    set_buffer_attr_bitvec(m_Bh[0], DW_VIP_AXI_WSTRB_BEAT, i, "0x001");
  }
  // change a few default values
  set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_AVALID_WVALID_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_BVALID_BREADY_DELAY, 0, 2);
  set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_BREADY_DELAY, 0, 1);
  for (int i = 0; i < 15; i++) {
    if (i < 4)
      set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 0);
    else
      set_buffer_attr_int(m_Bh[0], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 2);
  }
  // create 3 more write buffer
  copy_buffer(m_Bh[0], m_Bh[1]);
  copy_buffer(m_Bh[0], m_Bh[2]);
  copy_buffer(m_Bh[0], m_Bh[6]);

  // change a few data in the copied write buffer
  // write buffer 1
  set_buffer_attr_bitvec(m_Bh[1], DW_VIP_AXI_ALEN, 0, "0x00F");
  set_buffer_attr_bitvec(m_Bh[1], DW_VIP_AXI_ASIZE, 0, "0x001");
  set_buffer_attr_bitvec(m_Bh[1], DW_VIP_AXI_AID, 0, "0x002");
  set_buffer_attr_bitvec(m_Bh[1], DW_VIP_AXI_ADDR, 0, m_Addr2);
  for (int i = 0; i < 16; i++) {
    set_buffer_attr_bitvec(m_Bh[1], DW_VIP_AXI_WSTRB_BEAT, i, "0x003");
  }
  set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_AVALID_WVALID_DELAY, 0, 1);
  set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 2);
  set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_BVALID_BREADY_DELAY, 0, 1);
  set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_BREADY_DELAY, 0, 2);
  for (int i = 0; i < 15; i++) {
    if (i < 4)
      set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 1);
    else
      set_buffer_attr_int(m_Bh[1], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 0);
  }

  // write buffer 2
  set_buffer_attr_bitvec(m_Bh[2], DW_VIP_AXI_ALEN, 0, "0x03");
  set_buffer_attr_bitvec(m_Bh[2], DW_VIP_AXI_ASIZE, 0, "0x02");
  set_buffer_attr_bitvec(m_Bh[2], DW_VIP_AXI_AID, 0, "0x03");
  set_buffer_attr_bitvec(m_Bh[2], DW_VIP_AXI_ADDR, 0, m_Addr3);
  for (int i = 0; i < 16; i++) {
    set_buffer_attr_bitvec(m_Bh[2], DW_VIP_AXI_WSTRB_BEAT, i, "0x00F");
  }
  set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_AVALID_WVALID_DELAY, 0, -2);
  set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 1);
  set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_BVALID_BREADY_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_BREADY_DELAY, 0, 0);
  for (int i = 0; i < 15; i++) {
    if (i < 4)
      set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 3);
    else
      set_buffer_attr_int(m_Bh[2], DW_VIP_AXI_NEXT_WVALID_DELAY, i, 2);
  }

  // write buffer 6
  set_buffer_attr_bitvec(m_Bh[6], DW_VIP_AXI_ALEN, 0, "0x00");
  set_buffer_attr_bitvec(m_Bh[6], DW_VIP_AXI_ASIZE, 0, "0x00");
  set_buffer_attr_bitvec(m_Bh[6], DW_VIP_AXI_AID, 0, "0x0A");
  set_buffer_attr_bitvec(m_Bh[6], DW_VIP_AXI_ADDR, 0, m_Addr4);
  for (int i = 0; i < 16; i++) {
    set_buffer_attr_bitvec(m_Bh[6], DW_VIP_AXI_WSTRB_BEAT, i, "0x003");
  }
  set_buffer_attr_int(m_Bh[6], DW_VIP_AXI_AVALID_WVALID_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[6], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[6], DW_VIP_AXI_BVALID_BREADY_DELAY, 0, 10);
  set_buffer_attr_int(m_Bh[6], DW_VIP_AXI_BREADY_DELAY, 0, 1);
  set_buffer_attr_int(m_Bh[6], DW_VIP_AXI_NEXT_WVALID_DELAY, 0, 0);

  // read buffer 3
  // set Achannel data
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_AWRITE, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ALEN, 0, "0x0F");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ASIZE, 0, "0x01");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ABURST, 0, "0b01");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ALOCK, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ACACHE, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_APROT, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_AID, 0, "0x11");
  set_buffer_attr_bitvec(m_Bh[3], DW_VIP_AXI_ADDR, 0, m_Addr2);

  // change a few default values
  set_buffer_attr_int(m_Bh[3], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 1);
  set_buffer_attr_int(m_Bh[3], DW_VIP_AXI_RREADY_DELAY, 0, 2);
  for (int i = 0; i < 16; i++) {
    if (i < 4)
      set_buffer_attr_int(m_Bh[3], DW_VIP_AXI_RVALID_RREADY_DELAY, i, 2);
    else
      set_buffer_attr_int(m_Bh[3], DW_VIP_AXI_RVALID_RREADY_DELAY, i, 0);
  }

  // read buffer 4 (expected read)
  // set Achannel data
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_AWRITE, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ALEN, 0, "0x07");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ASIZE, 0, "0x00");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ABURST, 0, "0b01");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ALOCK, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ACACHE, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_APROT, 0, "0b00");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_AID, 0, "0x12");
  set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_ADDR, 0, m_Addr1);
  for (int i = 0; i < 16; i++) {
    // set expected read data
    set_buffer_attr_bitvec(m_Bh[4], DW_VIP_AXI_BUFFER_DATA, i, i);
  }

  // change a few default values
  set_buffer_attr_int(m_Bh[4], DW_VIP_AXI_NEXT_AVALID_DELAY, 0, 0);
  set_buffer_attr_int(m_Bh[4], DW_VIP_AXI_RREADY_DELAY, 0, 0);
  for (int i = 0; i < 16; i++) {
    if (i < 4)
      set_buffer_attr_int(m_Bh[4], DW_VIP_AXI_RVALID_RREADY_DELAY, i, 1);
    else
      set_buffer_attr_int(m_Bh[4], DW_VIP_AXI_RVALID_RREADY_DELAY, i, 2);
  }

  // create one interleave buffer (using the XACT buffer from above
  new_buffer(DW_VIP_AXI_INTERLEAVE, m_Bh[5]);
  // Fill the interleave buffer with data.
  // Each buffer handle represents one beat.
  // The number of beats for each buffer handle must be consistent
  // with the Alen value.
  // Each interleave buffer position may appear only ones.
  if (m_IntlDepth == 1) {
    // no write interleaving allowed by attached slave)
    set_buffer_interleave(m_Bh[5], 0, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 1, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 3, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 4, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 5, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 6, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 7, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 8, m_Bh[0]);

    set_buffer_interleave(m_Bh[5], 9, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 10, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 11, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 13, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 14, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 15, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 16, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 17, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 18, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 19, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 20, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 21, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 22, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 23, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 24, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 25, m_Bh[1]);

    set_buffer_interleave(m_Bh[5], 26, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 27, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 28, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 29, m_Bh[2]);


    set_buffer_interleave(m_Bh[5], 2, m_Bh[3]);
    set_buffer_interleave(m_Bh[5], 12, m_Bh[4]);
  }
  else {
    set_buffer_interleave(m_Bh[5], 0, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 4, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 8, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 11, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 12, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 21, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 22, m_Bh[0]);
    set_buffer_interleave(m_Bh[5], 23, m_Bh[0]);

    set_buffer_interleave(m_Bh[5], 1, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 5, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 9, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 13, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 14, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 15, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 16, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 18, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 19, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 20, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 24, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 25, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 26, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 27, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 28, m_Bh[1]);
    set_buffer_interleave(m_Bh[5], 29, m_Bh[1]);

    set_buffer_interleave(m_Bh[5], 2, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 6, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 10, m_Bh[2]);
    set_buffer_interleave(m_Bh[5], 17, m_Bh[2]);


    set_buffer_interleave(m_Bh[5], 3, m_Bh[3]);
    set_buffer_interleave(m_Bh[5], 7, m_Bh[4]);
  }
}

// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_CVIP_Master<true, true >;
template class DW_AXI_CVIP_Master<false, true >;
template class DW_AXI_CVIP_Master<true, false >;
template class DW_AXI_CVIP_Master<false, false >;
