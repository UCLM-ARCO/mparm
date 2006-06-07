
#include "Slave_ahb.h"

// ----------------------------------------------------------------------------
// Constructor : Slave_ahb::Slave_ahb
// ----------------------------------------------------------------------------

Slave_ahb::Slave_ahb(sc_module_name name_,
                     ahb_addr_t base_address,
                     int ID,
                     int WaitStates,
                     int RetryStates
		     )
: sc_module     (name_)
, WaitStates    (WaitStates)
, _base_address (base_address)
, my            (ID)
, _waitstates   (0)
, _retrystates  (0)
, _idlebusy     (false)
, _ready        (true)
, _slave_port   (0)
, _lock_master_id (-1)
, _addr_master_id (ahb_namespace::DUMMY_MASTER_ID)
, _lock           (false)
{

}
// ----------------------------------------------------------------------------
// Method: Slave_ahb::register_port
// ----------------------------------------------------------------------------

void
Slave_ahb::register_port(sc_port_base& port_,
                         const char* if_typename_)
{     
sc_string nm(if_typename_);

if( nm == typeid( ahb_slave_if ).name() ) {       
         // an out or inout port; only one can be connected
                if(_slave_port != 0 ) {
                        cout << "ERROR: AHB slave interface was already bound. <" << name() << ">" << endl;
                        sc_stop();
                }
                _slave_port = &port_;
        } /* else {
                // this code line never is reached if the class is
                // derived from only one interface
        } */
}

// ----------------------------------------------------------------------------
//  Interface Method: Slave_ahb::register_bus
// ----------------------------------------------------------------------------

int
Slave_ahb::register_bus(int,
                        int,
                        int,
                        int data_width)
{
if(data_width != 32) {
          cerr << " ERROR: wider data bus configurations are not supported. <" << name() << ">" << endl;
          sc_stop();
        }
return my;
}

// ----------------------------------------------------------------------------
//  Interface Method : Slave_ahb::set_data
// ----------------------------------------------------------------------------

void
Slave_ahb::set_data(int,
                    ahb_data_t data)
{
if (_idlebusy) {
        return;
        }

if(_rflag) {
        _success = direct_read(0, _address, &_data, _num_bytes);
        *data = 0xdeadbeef;
        _bus_data = data;
        } else {
                _success = direct_write(0, _address, data, _num_bytes);
                }
}

// ----------------------------------------------------------------------------
//  Interface Method: Slave0_ahb::response
// ----------------------------------------------------------------------------

bool
Slave_ahb::response(int,
                    ahb_hresp& _status)
{
if(_idlebusy) {
        _status = OKAY;
        _idlebusy = false;
        return true;
        }

if(_success) {
        if(--_waitstates > 0){
                        _status = OKAY;
                        _ready = false;
                        return false;
                        }
        if(--_retrystates > 0) {
                        _waitstates = WaitStates+1;
                        _status = RETRY;
                        _ready = true;
                        return false;
                        }

        _waitstates = WaitStates+1;
        _retrystates = RetryStates+1;
        if(_bus_data)
        *_bus_data = _data;

        _status = OKAY;
        _ready = true;
        return true;
        
        } else {
                _waitstates = 1;
                _retrystates = 1;
                _ready = true;
                _status = ERROR;
                }
return false;
}

// ----------------------------------------------------------------------------
//  Interface Method: Slave0_ahb::read
// ----------------------------------------------------------------------------

void
Slave_ahb::read(int,
                ahb_addr_t address,
                ahb_hsize hsize_)
{

if(_ready) {
        _address         = address;
        _num_bytes       = 1<<hsize_;
        _rflag           = true;

        if(_waitstates == 0)
                _waitstates = WaitStates+1;

        if(_retrystates == 0) {
                _retrystates = RetryStates+1;
                }
        }
}

// ----------------------------------------------------------------------------
//  Interface Method : Slave0_ahb::write
// ----------------------------------------------------------------------------

void
Slave_ahb::write(int,
                 ahb_addr_t address,
                 ahb_hsize hsize_)
{
if(_ready) {
        _address         = address;
        _num_bytes       = 1<<hsize_;
        _rflag           = false;

        if(_waitstates == 0)
                _waitstates = WaitStates+1;

        if(_retrystates == 0) {
                _retrystates = RetryStates+1;
                }
        }
}

// ----------------------------------------------------------------------------
// Interface Method : Slave_ahb::control_info
// ----------------------------------------------------------------------------

void
Slave_ahb::control_info(int,
                        ahb_hburst,
                        ahb_htrans transf_type_,
                        ahb_hprot,
                        int master_id_,
                        bool lock_)
{
 _addr_master_id = master_id_;
 _lock           = lock_;

if(_ready) {
        // explicit busy or idle command, response must be OKAY
        switch(transf_type_) {
                        case BUSY: _waitstates--;
                        case IDLE: _idlebusy = true;
                                   break;
                        default  : break;
                        }
           }
}

// ----------------------------------------------------------------------------
//  Interface Method : Slave_ahb::direct_read
// ----------------------------------------------------------------------------

bool     
Slave_ahb::direct_read(int,
                       ahb_addr_t address,
                       ahb_data_t data,
                               int num_bytes)
{

ahb_addr_t index  = (address-_base_address);
int offset = (address-_base_address)%4;

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

// ----------------------------------------------------------------------------
//  Interface Method : Slave0_ahb::direct_write
// ----------------------------------------------------------------------------

bool
Slave_ahb::direct_write(int,
                        ahb_addr_t address,
                        ahb_data_t data,
                        int num_bytes)
{
uint8_t bw;
ahb_addr_t index = (address-_base_address);
int offset = (address-_base_address)%4;

// check the right address data alignment
switch(_num_bytes) {
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

switch (num_bytes)
      {
        case 1 :  bw=MEM_BYTE;
                  break;
        case 2 :  bw=MEM_HWORD;
                  break;
        case 4 :  bw=MEM_WORD;
                  break;
        default : printf("Fatal error: Slave0 %hu detected a malformed data size \n",
                           my);
                  exit(0);
      }

this->Write((uint32_t)index, (uint32_t)*data, bw);
return true;

}

// ----------------------------------------------------------------------------
//  Interface Method : Slave_ahb::get_address_map
// ----------------------------------------------------------------------------

const ahb_address_map&
Slave_ahb::get_address_map(int)
{
return _address_map;
}

// ----------------------------------------------------------------------------
//  Interface Method : Slave_ahb::name
// ----------------------------------------------------------------------------

const char*
Slave_ahb::name() const
{
return sc_module::name();
}
