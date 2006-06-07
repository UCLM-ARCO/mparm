/****************************************************************
******************************************************************
**                                                              **
**       _____________                                          **
**      / ______   __/                                          **
**      \ \     / /                                             **
**       \ \   / /                                              **
**    ____\ \ / /                                               **
**   /______// /                                                **
**                                                              **
**                                                              **
**  STBus Power Enhanced Node                       		**
**                                                              **
**  by                                                          **
**                                                              **
**  Vittorio Zaccaria - Low Power Design Group - AST            **
**                                                              **
**                                                              **
******************************************************************
 ****************************************************************/

#ifndef ST_POWER_NODE_H
#define ST_POWER_NODE_H
using namespace std;

#include "SC_NODE.h"
#include <string>
#include <power_stbus_sysc_lib.h>
#include <SC_POWER_OBJECT.h>

#include "power.h"

//Martino
#ifndef stringify
#define stringify(x,suffix) ((string(x)+suffix).c_str())
#endif

template <	class TDATA, 
		class TADDRESS, 
		class TBE, 
		uint STBUS_SIZE, 
		uint NBR_INITIATOR, 
		uint NBR_TARGET, 
		uint RESOURCE_NUMBER_K, 
		uint RETURN_RESOURCE_NUMBER_K, 
		uint NODE_TYPE>

class POWER_NODE : public STBUS_NODE_BCA_with_snoop<	TDATA, 
						TADDRESS, 
						TBE, 
						STBUS_SIZE, 
						NBR_INITIATOR, 
						NBR_TARGET, 
						RESOURCE_NUMBER_K, 
						RETURN_RESOURCE_NUMBER_K>, POWER_OBJECT
{
	public:
        uint N_MAST,N_SLV,R_NUMBER_K,RETURN_R_NUMBER_K;
	uint clocks;
	uint measured_cycles;

	measurement_range power_cycle;
	measurement_range averaged_power_cycle;
        
	//FIXME
	unsigned int* fw_arbitration; 
        unsigned int* bk_arbitration;
        
	node_power_model_p pm;
	unsigned int requests;
	unsigned int responses;
        
	SC_HAS_PROCESS(POWER_NODE);

	POWER_NODE(	sc_module_name nm, 
			double req_wnd, 
			sc_time_unit req_wnd_unit
			) : 
			STBUS_NODE_BCA_with_snoop<  	TDATA, 
						TADDRESS, 
						TBE, 
						STBUS_SIZE, 
						NBR_INITIATOR, 
						NBR_TARGET, 
						RESOURCE_NUMBER_K, 
						RETURN_RESOURCE_NUMBER_K>(nm, req_wnd, req_wnd_unit), 
			POWER_OBJECT    (	nm,
						0.2,
						1.08,
						sc_time(CLOCKPERIOD * I_DIVIDER[0], SC_NS))
        
        
        {
		
		N_MAST=N_MASTERS;
		N_SLV=N_SLAVES;

                #ifdef stSHAREDst
		R_NUMBER_K=1;
		RETURN_R_NUMBER_K=1;
		fw_arbitration=new unsigned int [2];
		bk_arbitration=new unsigned int [2];
		
		fw_arbitration[0]=2;
		fw_arbitration[1]=0;
		bk_arbitration[0]=2;
		bk_arbitration[1]=0;
		#else
		R_NUMBER_K=N_SLV;
		RETURN_R_NUMBER_K=N_MAST;
		
		fw_arbitration=new unsigned int [N_SLV+1];
		bk_arbitration=new unsigned int [N_MAST+1];
		
		for(uint i=0;i<N_SLV;i++)
		{
		 fw_arbitration[i]=2;
		}
		fw_arbitration[N_MAST]=0;
		
		for(uint i=0;i<N_MAST;i++)
		{
		 bk_arbitration[i]=2;
		}
		bk_arbitration[N_MAST]=0;
		#endif
                		
		SC_METHOD(update_power_profile);
		sensitive_pos << clk;
		setup_name_dependent_parameters(name());
		pow_load_node_power_model(	N_MAST,
						N_SLV,
						R_NUMBER_K,
						RETURN_R_NUMBER_K,
						STBUS_SIZE, 
						NODE_TYPE, &pm, 
						STB_NODE_DATAPATH_DISTRIBUTED, 
						fw_arbitration,
						bk_arbitration
						);
		set_operating_voltage(1.08);
                set_clock_period(sc_time(CLOCKPERIOD * I_DIVIDER[0], SC_NS));
                requests = 0;
		responses = 0;

	}

	void update_power_profile()
	{
			uint dynamic_req = 0; 
			uint dynamic_res = 0; 
			measured_cycles++;
			//Poletti
			for(uint i=0;i<NBR_INITIATOR;i++) dynamic_req += In[i].gnt*In[i].req;
			for(uint i=0;i<NBR_TARGET;i++) dynamic_res += Out[i].r_gnt*Out[i].r_req;
			pow_e_compute_norm_power_for_node(	dynamic_req, 
								dynamic_res, 
								1, 
								pm, 
								&power_cycle, 
								&scaling_parameters);
			update_cumulative_energy(power_cycle);
			averaged_power_cycle = pow_sample_moving_average_power(power_object_name, &power_cycle);
			/*pow_e_write_dynamic_log(		power_object_name, 
								&averaged_power_cycle,
								dynamic_req,
								dynamic_res, 
								&scaling_parameters);	*/
                        requests += dynamic_req;
			responses += dynamic_res;
	if (POWERSTATS)	
	     statobject->inspectSTbusAccess(power_cycle.typ,power_cycle.max,power_cycle.min);  

	}
};

#endif
