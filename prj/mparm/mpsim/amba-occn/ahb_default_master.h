#ifndef AHB_DEFAULT_MASTER_H
#define AHB_DEFAULT_MASTER_H

#include "occn.h"
#include "arm_bus.h"


SC_MODULE(AhbDefMaster) 
{       
    MasterPort<Pdu<AhbMasterCtrl,N_int32>,Pdu<AhbSlaveCtrl,N_int32> > m_if;
    
    void idle_master()
    {
        Pdu<AhbMasterCtrl,N_int32> *pk = new Pdu<AhbMasterCtrl,N_int32>;

        do {
         occn_hdr(*pk,htrans)=AhbArb::IDLE;         
         occn_hdr(*pk,hbusreq)=true;
         occn_hdr(*pk,hlock)=false;
         m_if.send(pk);
         //cout << "Default Master get bus at: " << sc_time_stamp() << endl;	 
        } while(1);
    }
    
    SC_CTOR(AhbDefMaster)
    {
      SC_THREAD(idle_master);
    }
};

#endif
