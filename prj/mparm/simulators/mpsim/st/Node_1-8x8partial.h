/***************************************************

 file Node_1.h created by STBusGen

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics

****************************************************/


#ifdef WITH_POWER_NODE
#include "SC_POWER_NODE_hacked.h"
#endif

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, int NBR_INITIATOR, int NBR_TARGET,
	 int RESOURCE_NUMBER_K, int RETURN_RESOURCE_NUMBER_K>

#ifdef WITH_POWER_NODE
class Node_1_base_class_8x8partial 
      : public POWER_NODE <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR,
                           NBR_TARGET, RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K, 3>
#else
class Node_1_base_class_8x8partial 
      : public STBUS_NODE_BCA_with_snoop <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR, 
                                          NBR_TARGET, RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K>
#endif
{
    public:

  #define POST_DEPTH              256/STBUS_SIZE

	tracker_cell posting_buffer_0[POST_DEPTH+1];
	tracker_cell posting_buffer_1[POST_DEPTH+1];
	tracker_cell posting_buffer_2[POST_DEPTH+1];
	tracker_cell posting_buffer_3[POST_DEPTH+1];
	tracker_cell posting_buffer_4[POST_DEPTH+1];
	tracker_cell posting_buffer_5[POST_DEPTH+1];
	tracker_cell posting_buffer_6[POST_DEPTH+1];
	tracker_cell posting_buffer_7[POST_DEPTH+1];
    void Register_Reset()
    {

	   /*** Reset initiator registers to the default values ***/
	   initPriorityReset[0] = 0;
	   initPriorityReset[1] = 0;
	   initPriorityReset[2] = 0;
	   initPriorityReset[3] = 0;
	   initPriorityReset[4] = 0;
	   initPriorityReset[5] = 0;
	   initPriorityReset[6] = 0;
	   initPriorityReset[7] = 0;

	   initBandwidthReset[0] = 0;
	   initBandwidthReset[1] = 0;
	   initBandwidthReset[2] = 0;
	   initBandwidthReset[3] = 0;
	   initBandwidthReset[4] = 0;
	   initBandwidthReset[5] = 0;
	   initBandwidthReset[6] = 0;
	   initBandwidthReset[7] = 0;

	   initLatencyReset[0] = 0;
	   initLatencyReset[1] = 0;
	   initLatencyReset[2] = 0;
	   initLatencyReset[3] = 0;
	   initLatencyReset[4] = 0;
	   initLatencyReset[5] = 0;
	   initLatencyReset[6] = 0;
	   initLatencyReset[7] = 0;
	   
	   /*** Reset target registers to the default values ***/
	   targPriorityReset[0] = 0;
	   targPriorityReset[1] = 0;
	   targPriorityReset[2] = 0;
	   targPriorityReset[3] = 0;
	   targPriorityReset[4] = 0;
	   targPriorityReset[5] = 0;
	   targPriorityReset[6] = 0;
	   targPriorityReset[7] = 0;

	   /* tracker fifo initialization */
	   fifo_reset(&tracker_fifo_struct[0], 0);
	   fifo_reset(&tracker_fifo_struct[1], 0);
	   fifo_reset(&tracker_fifo_struct[2], 0);
	   fifo_reset(&tracker_fifo_struct[3], 0);
	   fifo_reset(&tracker_fifo_struct[4], 0);
	   fifo_reset(&tracker_fifo_struct[5], 0);
	   fifo_reset(&tracker_fifo_struct[6], 0);
	   fifo_reset(&tracker_fifo_struct[7], 0);
           for (i = 0; i < NBR_TARGET; i++)
           {
              tracker_state[i] = 0;
              tracker_full_out[i] = 0;
           }
    }
    
#ifdef WITH_POWER_NODE
    Node_1_base_class_8x8partial (sc_module_name nm, double req_wnd, sc_time_unit req_wnd_unit) 
      : POWER_NODE <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR, NBR_TARGET, 
                    RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K, 3> 
                    (nm,req_wnd,req_wnd_unit)
#else
    Node_1_base_class_8x8partial (sc_module_name nm, double req_wnd, sc_time_unit req_wnd_unitm) 
    : STBUS_NODE_BCA_with_snoop <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR, NBR_TARGET, 
                                 RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K>
                                (nm,req_wnd,req_wnd_unit)
#endif

{
        stbusType = 3;
        initRetime = false;
        initReturnRetime = false;
        targReturnRetime = false;
        targRetime = false;
        programmingOn = false;
        postedValidOn = false;
        message_timeout = 0;
        max_nbr_regions = 1;
	targMemMapRegions = new targ_mem_region<TADDRESS>[NBR_TARGET*max_nbr_regions];
	/*** check pointer ***/
	Assert_Pointers((void *)targMemMapRegions);
	targMemMapRegionsNb[0] = 1;
	targMemMapRegionsNb[1] = 1;
	targMemMapRegionsNb[2] = 1;
	targMemMapRegionsNb[3] = 1;
	targMemMapRegionsNb[4] = 1;
	targMemMapRegionsNb[5] = 1;
	targMemMapRegionsNb[6] = 1;
	targMemMapRegionsNb[7] = 1;

#define PRINTTARGET		

	for(uint i=0; i<8; i++)
	{
	 if(i<N_SLAVES)
	 {
	  (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_min = addresser->SLV_TABLE[i].firstaddr;
	  (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_max = addresser->SLV_TABLE[i].lastaddr;
#ifdef PRINTTARGET	   
	   printf("target_private:%d start_add:%x, stop_add:%x\n",i,
	   (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_min ,
	   (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_max
	   );
#endif	 
	 }
	 else
	 {
	  (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_min =0xFFFFFFFF; 
	  (targMemMapRegions + i*max_nbr_regions + 0)->targ_mem_map_max = 0xFFFFFFFF;
	 }
	}
	
	/*** ordering blocks initialization ***/
	ord_blk_filter_on[0] = false;
	ord_blk_filter_depth[0] = 0;
	ord_blk_filter_on[1] = false;
	ord_blk_filter_depth[1] = 0;
	ord_blk_filter_on[2] = false;
	ord_blk_filter_depth[2] = 0;
	ord_blk_filter_on[3] = false;
	ord_blk_filter_depth[3] = 0;
	ord_blk_filter_on[4] = false;
	ord_blk_filter_depth[4] = 0;
	ord_blk_filter_on[5] = false;
	ord_blk_filter_depth[5] = 0;
	ord_blk_filter_on[6] = false;
	ord_blk_filter_depth[6] = 0;
	ord_blk_filter_on[7] = false;
	ord_blk_filter_depth[7] = 0;
	
	/* target resource mapping initialization */
	targResourceMapping[0] = 0;
	targResourceMapping[1] = 0;
	targResourceMapping[2] = 1;
	targResourceMapping[3] = 1;
	targResourceMapping[4] = 2;
	targResourceMapping[5] = 2;
	targResourceMapping[6] = 2;
	targResourceMapping[7] = 2;
	
	/* initiator return resource mapping initialization */
	initReturnResourceMapping[0] = 0;
	initReturnResourceMapping[1] = 0;
	initReturnResourceMapping[2] = 1;
	initReturnResourceMapping[3] = 1;
	initReturnResourceMapping[4] = 2;
	initReturnResourceMapping[5] = 2;
	initReturnResourceMapping[6] = 3;
	initReturnResourceMapping[7] = 3;
	
	/* resource mappimg number and elements */;
	number_target_per_resource[0] = 2;
	number_target_per_resource[1] = 2;
	number_target_per_resource[2] = 4;
	
	/* return resource mappimg number and elements */;
	number_initiator_per_return_resource[0] = 2;
	number_initiator_per_return_resource[1] = 2;

	
	/* auxiliary vector initialization */
	p_req_aux1 = ord_blk_output;
	p_req_aux2 = p_req_aux1;
	p_req_aux3 = transaction_management_output;
	p_r_req_aux1 = response_source_decoder_output;
	p_r_req_aux2 = response_transaction_management_output;
	
	/*** set resource arbitration vector to the default values ***/
	//Picciano: resourceArbitration set to 6 instead of 2
	resourceArbitration[0] = 6;
	resourceArbitration[1] = 6;
	resourceArbitration[2] = 6;
	
	/*** set resource arbitration vector to the default values ***/
	resourceLd8Interrupt[0] = false;
	resourceLd8Interrupt[1] = false;
	resourceLd8Interrupt[2] = false;

	/*** set response resource arbitration vector to the default values ***/
	//Picciano: response_resourceArbitration set to 6 instead of 2
	response_resourceArbitration[0] = 6;
	response_resourceArbitration[1] = 6;
	response_resourceArbitration[2] = 6;
	response_resourceArbitration[3] = 6;
	response_resourceArbitration[4] = 6;
	response_resourceArbitration[5] = 6;
	response_resourceArbitration[6] = 6;
	response_resourceArbitration[7] = 6;

	
	/* tracker array initialization */

	/*** set source range vectors ***/
	init_src_min[0] = 0;
	init_src_max[0] = 0;
	init_src_mask[0] = 1;
	init_src_min[1] = 1;
	init_src_max[1] = 1;
	init_src_mask[1] = 1;
	init_src_min[2] = 2;
	init_src_max[2] = 2;
	init_src_mask[2] = 1;
	init_src_min[3] = 3;
	init_src_max[3] = 3;
	init_src_mask[3] = 1;
	init_src_min[4] = 4;
	init_src_max[4] = 4;
	init_src_mask[4] = 1;
	init_src_min[5] = 5;
	init_src_max[5] = 5;
	init_src_mask[5] = 1;
	init_src_min[6] = 6;
	init_src_max[6] = 6;
	init_src_mask[6] = 1;
	init_src_min[7] = 7;
	init_src_max[7] = 7;
	init_src_mask[7] = 1;

	/*** Preset the bandwidth on flag vector ***/
	   initBandwidthLimiterOn[0] = 0;
	   initBandwidthLimiterOn[1] = 0;
	   initBandwidthLimiterOn[2] = 0;
	   initBandwidthLimiterOn[3] = 0;
	   initBandwidthLimiterOn[4] = 0;
	   initBandwidthLimiterOn[5] = 0;
	   initBandwidthLimiterOn[6] = 0;
	   initBandwidthLimiterOn[7] = 0;

	/*** Preset Low Priority vector ***/
	   initLowPriorityReset[0] = 0;
	   initLowPriorityReset[1] = 0;
	   initLowPriorityReset[2] = 0;
	   initLowPriorityReset[3] = 0;
	   initLowPriorityReset[4] = 0;
	   initLowPriorityReset[5] = 0;
	   initLowPriorityReset[6] = 0;
	   initLowPriorityReset[7] = 0;

	/*** Preset the Frame Size vector ***/
	   initFrameSizeReset[0] = 0;
	   initFrameSizeReset[1] = 0;
	   initFrameSizeReset[2] = 0;
	   initFrameSizeReset[3] = 0;
	   initFrameSizeReset[4] = 0;
	   initFrameSizeReset[5] = 0;
	   initFrameSizeReset[6] = 0;
	   initFrameSizeReset[7] = 0;

	/*** Preset the Words Limit vector ***/
	   initWordsLimitReset[0] = 0;
	   initWordsLimitReset[1] = 0;
	   initWordsLimitReset[2] = 0;
	   initWordsLimitReset[3] = 0;
	   initWordsLimitReset[4] = 0;
	   initWordsLimitReset[5] = 0;
	   initWordsLimitReset[6] = 0;
	   initWordsLimitReset[7] = 0;
	   
   	/*** set priority type vector ***/
	initPriorityType[0] = 0;
	initPriorityType[1] = 0;
	initPriorityType[2] = 0;
	initPriorityType[3] = 0;
	initPriorityType[4] = 0;
	initPriorityType[5] = 0;
	initPriorityType[6] = 0;
	initPriorityType[7] = 0;

	/*** Preset init interrupt rule reset ***/
	init_interrupt_rule_reset[0] = 0;
	init_interrupt_rule_reset[1] = 0;
	init_interrupt_rule_reset[2] = 0;
	init_interrupt_rule_reset[3] = 0;
	init_interrupt_rule_reset[4] = 0;
	init_interrupt_rule_reset[5] = 0;
	init_interrupt_rule_reset[6] = 0;
	init_interrupt_rule_reset[7] = 0;

	/*** Preset init interrupt rule reset ***/
	init_max_opcode_size[0] = 5;
	init_max_opcode_size[1] = 5;
	init_max_opcode_size[2] = 5;
	init_max_opcode_size[3] = 5;
	init_max_opcode_size[4] = 5;
	init_max_opcode_size[5] = 5;
	init_max_opcode_size[6] = 5;
	init_max_opcode_size[7] = 5;
    }
};




