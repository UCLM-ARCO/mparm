#ifndef __SLAVE_AHB_H
#define __SLAVE_AHB_H

#include <systemc.h>
#include <ccss_systemc.h>

#include "ahb_slave_if.h"
#include "globals.h"
#include "address.h"
#include "ahb_types.h"
#include "ahb_dw_misc.h"
#include "mem_class.h"

using namespace ahb_namespace;

class Slave_ahb// A Simple Slave Module
        : public sc_module
        , public ahb_namespace::ahb_slave_if
{

public:

char* type;
int WaitStates;
int RetryStates;
    
    // parameters

    // This identifier for the slave is used by the Inter
    // Connection Matrix (ICM) to identify the slave.
    //CCSS_PARAMETER(int, SlaveID);

    // Enable monitor tracing.
    CCSS_PARAMETER(bool, DoTrace);

    // ports

    // initialize parameters
    virtual void InitParameters() {
        bool _tmp_DoTrace = false;
        DoTrace.conditional_init(_tmp_DoTrace);
                   }

// constructor
Slave_ahb(sc_module_name name_, // name of the instance
          ahb_namespace::ahb_addr_t base_address,// base address of the slave
	  int ID,
          int WaitStates,  //waitstates
          int RetryStates  //retrystates      
          );

// Register a port, make sure that only one port
// is connected to the interface.
virtual void register_port(sc_port_base&,
                           const char*);

// Register the bus connected to this slave interface.
virtual int register_bus(int,        // bus handle
                     int,        // priority
                     int,        // address bus width in bits
                     int);       // data bus width in bits


// query the status of the memory.
virtual bool response(int,            // caller id
                 ahb_namespace::ahb_hresp&);   // status

// read from the slave
virtual void read(int,                // caller id
             ahb_namespace::ahb_addr_t,         // address
             ahb_namespace::ahb_hsize);               // number of bytes

// Write to the slave
virtual void write(int,               // caller id
              ahb_namespace::ahb_addr_t,        // address
              ahb_namespace::ahb_hsize );             // number of bytes

// This method provides additional control information, e.g.
// the burst mode and the transfer type.
virtual void control_info(int,                // id of the bus
                     ahb_namespace::ahb_hburst,      // mode of the active burst
                     ahb_namespace::ahb_htrans,          // the transfer type
                     ahb_namespace::ahb_hprot,           // access protection information
                     int,                // master id
                     bool);              // master locks the bus

virtual void set_data(int,            // caller id
                 ahb_namespace::ahb_data_t);    // data

// Direct read from the slave
virtual bool direct_read(int,         // caller id
                    ahb_namespace::ahb_addr_t,  // address
                    ahb_namespace::ahb_data_t,  // data
                    int);        // number of bytes

// Direct write to the slave
virtual bool direct_write(int,        // caller id
                     ahb_namespace::ahb_addr_t, // address
                     ahb_namespace::ahb_data_t, // data
                     int);       // number of bytes

// Return the memory map of the slave
virtual const ahb_namespace::ahb_address_map& get_address_map(int);    // master id

// Query the name of the slave
virtual const char* name() const;

protected:

virtual void Write(uint32_t, uint32_t, uint8_t){};
virtual uint32_t Read(uint32_t){return 0;};

protected:

ahb_namespace::ahb_addr_t          _base_address;
uint16_t my;
ahb_namespace::ahb_addr_t          _address;
bool                               _rflag;
int                                _num_bytes;
unsigned int                       _data;
ahb_namespace::ahb_data_t          _bus_data;
int                 _waitstates;
int                 _retrystates;
bool                _idlebusy;
bool                _ready;
bool                _success;
sc_port_base*       _slave_port;
int                 _lock_master_id;
int                 _addr_master_id;
bool                _lock;
int                 split[20];
ahb_namespace::ahb_address_map     _address_map;   // vector of memory regions

}; // end module Slave

#endif
