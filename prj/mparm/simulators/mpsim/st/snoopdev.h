#ifndef _snoopdev_h_included
#define _snoopdev_h_included
                                    #define INITIAL_DEBUGGING
                                    /* Togliere dopo i test iniziali */


struct snoopdev;    // Forward declaration

#include <armproc.h>

#include "address.h"
#include "systemc.h"
#include "config.h"
#include "cache.h"
#include "debug.h"
#include "stbus_adds.h"


#define SNOOP_TRICK


#ifdef SNOOP_TRICK
#define SN_STATIC static
#else
#define SN_STATIC
#endif

  /* Max nuber of simultaneously transactions. */
#define MAX_TID 16

  /* Snoop ports connected to the bus.
     Shared-bus: 1 direct port ed 1 response port. */
#define N_SNOOP_PORTS_DIRECT   1
#define N_SNOOP_PORTS_RESPONSE 1


SC_MODULE(snoopdev)
{
  public:
    sc_in_clk clock;
    stbus_T2T3_port_direct<uint, uint, uchar> 
        SnoopPort_d[N_SNOOP_PORTS_DIRECT];
    stbus_T2T3_port_response<uint, uint, uchar>
        SnoopPort_r[N_SNOOP_PORTS_RESPONSE];

    enum op_type {SNOOP_NOP=0, SNOOP_READ, SNOOP_WRITE};

    typedef struct {
      enum op_type curr_op;
      uint32_t curr_addr;
      uint32_t curr_data;
      uchar    curr_be;
      int      curr_id;  /* core's id */
    } bus_op;

    SN_STATIC
    bus_op bus_operation[N_SNOOP_PORTS_DIRECT+N_SNOOP_PORTS_RESPONSE];


      /* to keep track of outstanding transactions */
    typedef struct {
      int core_id;
      uint32_t addr;
      uint32_t data;
      int opc;
      uchar be;
    } op_info;

    SN_STATIC op_info *op_vector;


      /* A class to keep records of outstanding cache operations
         which must be performed by the snoop device.
         It implements a circular buffer.                        */
    class cache_op_buffer {
      public:

      typedef struct {
        uint32_t addr;
        uint32_t data;
        uchar    be;
      } cache_op;


      cache_op_buffer(int buffer_size) {
        size = buffer_size;
        buffer = new cache_op[size];
        in_buffer = 0;
        first = 0;
      };

      int is_empty() {return (in_buffer==0);};
      int is_full() {return (in_buffer==size);};

      int insert(uint32_t addr, uint32_t data, uchar be) {
        if (in_buffer<size) {
          buffer[first].addr = addr;
          buffer[first].data = data;
          buffer[first].be = be;
          first = (first + 1) % size;
          in_buffer++;
          return 1;
        } else
          return 0; /* buffer full */
      };

      int insert(cache_op& op) {
        return insert(op.addr, op.data, op.be);
      };

      int insert(cache_op *op) {
        return insert(op->addr, op->data, op->be);
      };

      cache_op *get() {
        cache_op *op;
        int f;
        if (in_buffer) {
          f = first-in_buffer;
          if (f<0) f+=size;
          op = buffer+f;
          in_buffer--;
          return op;
        } else
          return NULL; /* buffer empty */
      };

      int get(uint32_t *addr, uint32_t *data, uchar *be) {
        cache_op *op;
        op = get();
        if (op) {
          *addr = op->addr;
          *data = op->data;
          *be = op->be;
        } else
          return 0;
      };

      cache_op *buffer;
      int size;
      int in_buffer;
      int first;
    };

    cache_op_buffer *op_buffer;

      // Used in perform_op to keep status, during a multi-cycle operation
    cache_op_buffer::cache_op *op;
    int STAT, W;
    int CACHE_WS;  // The cache delay

#ifdef SNOOP_TRICK
    static long decoded;
#endif


    SC_CTOR(snoopdev) {
      the_ARM = NULL;
      op_vector = new op_info[MAX_TID*N_CORES]; 
      for (int k=0; k<N_SNOOP_PORTS_DIRECT+N_SNOOP_PORTS_RESPONSE; k++) {
        bus_operation[k].curr_id = -1;
        bus_operation[k].curr_op = SNOOP_NOP;
      }

      for (int k=0; k<MAX_TID*N_CORES; k++) {
        op_vector[k].core_id = -1;
        op_vector[k].addr = 0;
      }

      op_buffer = new cache_op_buffer(N_CORES);

      if (LOCK_ICACHE) 
          CACHE_WS = (ICACHE_WS>DCACHE_WS?ICACHE_WS:DCACHE_WS);
      else 
          CACHE_WS = DCACHE_WS;

      STAT = '0';

      SC_METHOD(Cycle);
      sensitive_pos << clock;
    };


    ~snoopdev(){ delete[] op_vector; delete[] op_buffer;};


#ifdef INITIAL_DEBUGGING
    void to_trace(sc_trace_file *tf) {
      char buffer[40];
      for (int k=0; k<N_SNOOP_PORTS_DIRECT+N_SNOOP_PORTS_RESPONSE; k++) {
        sprintf(buffer, "snoopdev_%d_curr_op_%d", the_ARM->ID, k);
        sc_trace(tf, bus_operation[k].curr_op, buffer);
        sprintf(buffer, "snoopdev_%d_curr_addr_%d", the_ARM->ID, k);
        sc_trace(tf, bus_operation[k].curr_addr, buffer);
        sprintf(buffer, "snoopdev_%d_curr_data_%d", the_ARM->ID, k);
        sc_trace(tf, bus_operation[k].curr_data, buffer);
        sprintf(buffer, "snoopdev_%d_curr_id_%d", the_ARM->ID, k);
        sc_trace(tf, bus_operation[k].curr_id, buffer);
      }
    };
#endif

    virtual void Cycle();
    void decode();
    void perform_op();


    /* Auxiliar member functions */

    void execute();

    void Decode_Direct_Port(int port);
    void Decode_Response_Port(int port);
    void Examine_Direct_Bus_Operation(int port);
    void Examine_Response_Bus_Operation(int port);

    void WriteOnDirectPort(int port);
    void ReadOnDirectPort(int port);
    void ReadOnResponsePort(int port, int core, int tid);
    void WriteOnResponsePort(int port, int core, int tid);

    void Set_Bus_Operation(int id,int core, enum op_type op, 
                           uint32_t addr, uint32_t data, 
                           uchar be);

    void Record_Bus_Operation(int core, int tid, 
                              uint32_t addr, uint32_t data, 
                              int opc, uchar be);

    void Use_Recorded_Data(int id, int core, int tid, enum op_type op);

    void CacheUpdate(uint32_t addr, uint32_t data, uchar be);
    void CacheInvalidate(uint32_t addr);

    inline bool InShared(uint32_t addr) {
        return addresser->PhysicalInSharedSpace(addr);
    };


    CArmProc *the_ARM;
};


#ifdef INITIAL_DEBUGGING
extern snoopdev *snoop[32];  /* 32: max # of cores */
#endif

#endif     /*  #ifndef _snoopdev_h_included  */

