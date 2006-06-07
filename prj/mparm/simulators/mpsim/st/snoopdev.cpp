#ifndef NOSNOOP


#include <systemc.h>
#include <unistd.h>
#include "common.h"
#include "stbus_opcodes.h"
#include "snoopdev.h"


#ifdef SNOOP_TRICK
long snoopdev::decoded = -1;
snoopdev::op_info *snoopdev::op_vector;
snoopdev::bus_op
    snoopdev::bus_operation[N_SNOOP_PORTS_DIRECT+N_SNOOP_PORTS_RESPONSE];

#define SN_bus_operation  snoopdev::bus_operation
#define SN_decoded        snoopdev::decoded
#define SN_op_vector      snoopdev::op_vector

#else

#define SN_bus_operation  bus_operation
#define SN_decoded        decoded
#define SN_op_vector      op_vector

#endif


void snoopdev::Cycle()
{
#ifdef SNOOP_TRICK
    /* Trick - Just to avoid to call decode() for all the snoop devices.
       We assume that all the devices are connected to the same snoop port. */

  long curr_time = (long)(sc_simulation_time());

  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::Cycle()  - enter time: %10.1f  ID:%d\n",
         sc_simulation_time(),
         the_ARM->ID);
  ASSERT(decoded<=curr_time);
  if (decoded<curr_time) {
    decode();
    decoded=curr_time;
  }
#else
  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::Cycle()  - enter time: %10.1f  ID:%d\n",
         sc_simulation_time(),
         the_ARM->ID);
  decode();
#endif

  perform_op();
}


void snoopdev::decode()
{
  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::decode()  - enter\n");
  for (int k=0; k<N_SNOOP_PORTS_DIRECT; k++)
      Decode_Direct_Port(k);
  for (int k=0; k<N_SNOOP_PORTS_RESPONSE; k++)
      Decode_Response_Port(k);
}


void snoopdev::perform_op()
{
  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::perform_op()  - enter\n");

#if 0
    /* Examining operation on the direct-busses */
  for (int k=0; k<N_SNOOP_PORTS_DIRECT ; k++) {
    Examine_Direct_Bus_Operation(k);
  }
#endif

    /* Examining operation on the respons-busses */
  for (int k=N_SNOOP_PORTS_DIRECT; 
       k<N_SNOOP_PORTS_DIRECT+N_SNOOP_PORTS_RESPONSE ; 
       k++) {
    Examine_Response_Bus_Operation(k);
  }

  execute();
}

/* Examine the pending operation buffer and perfom cache access if needed */
inline
void snoopdev::execute()
{
  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::execute()  - enter\n");

  // Something to do?
  switch (STAT) {
    // '0' : initial state
    // 'A' : waiting state (cache is currently used by the core)
    // 'B' : cache access state

    case '0':
        TRACEX(SNOOPDEV_TRACEX, 6, "case 0\n");
        the_ARM->CL1=0;  // Unlock the cache (previously locked?)
        if (!op_buffer->is_empty()) {
            /* Something to do: lock the cache and perform the job */
            op = op_buffer->get();
        } else
            break;

        the_ARM->CL1=1;
        while ( (the_ARM->CL2) && (!the_ARM->CLA) )
            sleep(20);  // give the other task a chance to advance.
                        // This can happen only for concurrent tasks

    case 'A':
        if (the_ARM->CL2) {
            STAT = 'A';
            break;
        } else STAT='B';

    case 'B':
            // Cache Access
        TRACEX(SNOOPDEV_TRACEX, 6, "case B\n");
        W=0;
        if (W==CACHE_WS) {
          STAT='0';
          if (SNOOP_POLICY == SNOOP_INVALIDATE) {
            CacheInvalidate(op->addr);
          } else if (SNOOP_POLICY == SNOOP_UPDATE) {
            CacheUpdate(op->addr, op->data, op->be);
          }
          break;
        }
        STAT='B';
        W++;
        break;

    default:
        ASSERT(0); // This cannot happen
  }

}


inline
void snoopdev::Decode_Direct_Port(int port)
{
  TRACEX(SNOOPDEV_TRACEX, 8, 
         "snoopdev::Decode_Direct_Port(%d)  - enter\n", port);

  SN_bus_operation[port].curr_op = SNOOP_NOP;
  if (!(SnoopPort_d[port].req)) return;

  if ( (SnoopPort_d[port].opc&0xF) == STORE_M ) {
      /* Someone is going to write. */
    WriteOnDirectPort(port);
  }

  if ( (SnoopPort_d[port].opc&0xF) == LOAD_M ) {
      /* Someone is starting a read. */
    ReadOnDirectPort(port);
  }
}


inline
void snoopdev::Decode_Response_Port(int port)
{
  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::Decode_Response_Port(%d)  - enter\n", port);

  SN_bus_operation[N_SNOOP_PORTS_DIRECT+port].curr_op = SNOOP_NOP;
  if ( !(SnoopPort_r[port].r_req) ) return;

   /* Response to an operation previously started.
      To know which operation is, use the signal r_tid&0x0f */

  int tid = SnoopPort_r[port].r_tid & 0x0F;
  int core = SnoopPort_r[port].r_src;
    // r_opc does not carry information

  if (SN_op_vector[MAX_TID*core+tid].core_id == -1) return;

  if ( SN_op_vector[MAX_TID*core+tid].opc == LOAD_M ) {
    ReadOnResponsePort(port, core, tid);
  } else if (SN_op_vector[MAX_TID*core+tid].opc == STORE_M) {
    WriteOnResponsePort(port, core, tid);
  }
}


inline
void snoopdev::Examine_Direct_Bus_Operation(int port)
{
}


inline
void snoopdev::Examine_Response_Bus_Operation(int port)
{
  uint32_t addr;

  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::Examine_Response_Bus_Operation(%d)  - enter\n", port);

  if (SN_bus_operation[port].curr_id==(int)the_ARM->ID) return;
  if (SN_bus_operation[port].curr_op==SNOOP_READ) {
    /* READ
       address:  bus_operation[port].curr_addr
       data:     bus_operation[port].curr_data
       be:       bus_operation[port].curr_be   */
  } else if (SN_bus_operation[port].curr_op==SNOOP_WRITE) {
      addr = addresser->Physical2Logical(SN_bus_operation[port].curr_addr,
                                         the_ARM->ID);
      if (SNOOP_POLICY == SNOOP_UPDATE) {
        int res = op_buffer->insert(SN_bus_operation[port].curr_addr,
                                    SN_bus_operation[port].curr_data,
                                    SN_bus_operation[port].curr_be);
        ASSERT(res); /* buffer MUST be big enough */
      } else if (SNOOP_POLICY == SNOOP_INVALIDATE) {
        int res;
        res = op_buffer->insert(SN_bus_operation[port].curr_addr, 0, 0);
        ASSERT(res); /* buffer MUST be big enough */
      }
  }
}


inline
void snoopdev::WriteOnDirectPort(int port)
{
  int core = SnoopPort_d[port].src;
  int tid = SnoopPort_d[port].tid & 0x0F;
  uint32_t addr;

  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::WriteOnDirectPort(%d)  - enter\n", port);

  addr = SnoopPort_d[port].add;

    /* Take into account only shared-memory addresses */
  if ( !InShared(addr) ) return;

  if ( (SNOOP_POLICY == SNOOP_INVALIDATE) || (SNOOP_POLICY == SNOOP_UPDATE) ) {
      Record_Bus_Operation(core, tid, addr, SnoopPort_d[port].data,
                           STORE_M, SnoopPort_d[port].be);
  }
}


inline
void snoopdev::ReadOnDirectPort(int port)
{
  int tid = SnoopPort_d[port].tid & 0x0F;
  int core = SnoopPort_d[port].src;
  uint32_t addr;

  TRACEX(SNOOPDEV_TRACEX, 8, "snoopdev::ReadOnDirectPort(%d)  - enter\n", port);

  addr = SnoopPort_d[port].add;

    /* Take into account only shared-memory addresses */
  if ( !InShared(addr) ) return;

    /* Record values for further comparison */

    /* This tid is not in use, is it? */
  ASSERT(SN_op_vector[MAX_TID*core+tid].core_id == -1);
  Record_Bus_Operation(core, tid, addr, 0, LOAD_M, SnoopPort_d[port].be);
}


inline
void snoopdev::ReadOnResponsePort(int port, int core, int tid)
{
  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::ReadOnResponsePort(%d,%d,%d)  - enter\n", port, core, tid);

    /* PARANOID CHECK! */
  ASSERT(SN_op_vector[MAX_TID*core+tid].core_id == SnoopPort_r[port].r_src);

  Use_Recorded_Data(N_SNOOP_PORTS_DIRECT+port, core, tid, SNOOP_READ);

    /* We did not record data, they are incoming NOW! */
  SN_bus_operation[N_SNOOP_PORTS_DIRECT+port].curr_data =
    SnoopPort_r[port].r_data;

  if (SnoopPort_r[port].r_eop) {
    /* End of transaction: make tid available for a new transfer. */
    TRACEX(SNOOPDEV_TRACEX,
           4,
           "Read(end) - Processor: %d Tid: %d\n", core, tid);
    SN_op_vector[MAX_TID*core+tid].core_id = -1;
    SN_op_vector[MAX_TID*core+tid].addr = 0;
  }
}


inline
void snoopdev::WriteOnResponsePort(int port, int core, int tid)
{
  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::WriteOnResponsePort(%d)  - enter\n", port, core, tid);

  if ( (SNOOP_POLICY == SNOOP_INVALIDATE) || (SNOOP_POLICY == SNOOP_UPDATE) ) {
      Use_Recorded_Data(N_SNOOP_PORTS_DIRECT+port, core, tid, SNOOP_WRITE);
      if (SnoopPort_r[port].r_eop) {
          /* End of transaction: tid is avail. for a new transfer. */
          TRACEX(SNOOPDEV_TRACEX,
                 4,
                 "Write(end) - Processor: %d Tid: %d\n", core, tid);
          SN_op_vector[MAX_TID*core+tid].core_id = -1;
          SN_op_vector[MAX_TID*core+tid].addr = 0;
      }
  }
}


inline
void snoopdev::Set_Bus_Operation(int id, int core, enum snoopdev::op_type op,
                                 uint32_t addr, uint32_t data,
                                 uchar be)
{
    TRACEX(SNOOPDEV_TRACEX, 8,
           "snoopdev::Set_Bus_Operation(%d,%d,%d,0x%x,0x%x,%d)  - enter\n", 
           id, core, (int)op, addr, data, be);

    SN_bus_operation[id].curr_id = core;
    SN_bus_operation[id].curr_op = op;
    SN_bus_operation[id].curr_addr = addr;
    SN_bus_operation[id].curr_data = data;
    SN_bus_operation[id].curr_be = be;
}


inline
void snoopdev::Record_Bus_Operation(int core, int tid,
                                    uint32_t addr, uint32_t data,
                                    int opc, uchar be)
{
    TRACEX(SNOOPDEV_TRACEX, 8,
           "snoopdev::Record_Bus_Operation(%d,%d,0x%x,0x%x,%d,%d)  - enter\n",
           core, tid, addr, data, opc, be);

    SN_op_vector[MAX_TID*core+tid].addr = addr;
    SN_op_vector[MAX_TID*core+tid].data = data;
    SN_op_vector[MAX_TID*core+tid].core_id = core;
    SN_op_vector[MAX_TID*core+tid].opc = opc;
    SN_op_vector[MAX_TID*core+tid].be = be;
}


inline
void snoopdev::Use_Recorded_Data(int id, int core, int tid, 
                                 enum snoopdev::op_type op)
{
    TRACEX(SNOOPDEV_TRACEX, 8,
           "snoopdev::Use_Recorded_Data(%d,%d,%d,%d)  - enter\n",
           id, core, tid, (int)op);

    Set_Bus_Operation(id,
                      SN_op_vector[MAX_TID*core+tid].core_id,
                      op,
                      SN_op_vector[MAX_TID*core+tid].addr,
                      SN_op_vector[MAX_TID*core+tid].data,
                      SN_op_vector[MAX_TID*core+tid].be);
}


inline
void snoopdev::CacheUpdate(uint32_t addr, uint32_t data, uchar be)
{
  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::CacheUpdate(0x%x,0x%x,%d)  - enter\n",
         addr, data, be);

  if (SHARED_CACHE) {
    //printf("core %hu - snoopdev - updating %d with 0x%x\n", the_ARM->ID, addr>>2, data);
    the_ARM->m_pICache->Update(addr>>2, data, be);
  } else {
    //printf("core %hu - snoopdev - updating %d with 0x%x\n", the_ARM->ID, addr>>2, data);
    if (LOCK_ICACHE) the_ARM->m_pICache->Update(addr>>2, data, be);
    the_ARM->m_pDCache->Update(addr>>2, data, be);
  }
}


inline
void snoopdev::CacheInvalidate(uint32_t addr)
{
  TRACEX(SNOOPDEV_TRACEX, 8,
         "snoopdev::CacheInvalidate(0x%x)  - enter\n",
         addr);

  if (SHARED_CACHE) {
    //printf("core %hu - snoopdev - setting INVALID_BIT for 0x%x\n", the_ARM->ID, addr>>2);
    the_ARM->m_pICache->SetBit(addr>>2, INVALID_BIT);
  } else {
    //printf("core %hu - snoopdev - setting INVALID_BIT for 0x%x\n", the_ARM->ID, addr>>2);
    if (LOCK_ICACHE) the_ARM->m_pICache->SetBit(addr>>2, INVALID_BIT);
    the_ARM->m_pDCache->SetBit(addr>>2, INVALID_BIT);
  }
}


#ifdef INITIAL_DEBUGGING
snoopdev *snoop[32];  /* 32: max # of cores */
#endif


#endif  /* #ifndef NOSNOOP */

