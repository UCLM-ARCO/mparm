/***************************************************

 file Node_1.h created by STBusGen

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics

****************************************************/

#include "address.h"

#ifdef WITH_POWER_NODE
#include "SC_POWER_NODE_hacked.h"
#endif


template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, int NBR_INITIATOR, int NBR_TARGET,
	 int RESOURCE_NUMBER_K, int RETURN_RESOURCE_NUMBER_K>

#ifdef WITH_POWER_NODE
class Node_1_base_class_32x32shared 
      : public POWER_NODE <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR,
                           NBR_TARGET, RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K, 3>
      
#else
class Node_1_base_class_32x32shared 
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
	tracker_cell posting_buffer_8[POST_DEPTH+1];
	tracker_cell posting_buffer_9[POST_DEPTH+1];
	tracker_cell posting_buffer_10[POST_DEPTH+1];
	tracker_cell posting_buffer_11[POST_DEPTH+1];
	tracker_cell posting_buffer_12[POST_DEPTH+1];
	tracker_cell posting_buffer_13[POST_DEPTH+1];
	tracker_cell posting_buffer_14[POST_DEPTH+1];
	tracker_cell posting_buffer_15[POST_DEPTH+1];
	tracker_cell posting_buffer_16[POST_DEPTH+1];
	tracker_cell posting_buffer_17[POST_DEPTH+1];
	tracker_cell posting_buffer_18[POST_DEPTH+1];
	tracker_cell posting_buffer_19[POST_DEPTH+1];
	tracker_cell posting_buffer_20[POST_DEPTH+1];
	tracker_cell posting_buffer_21[POST_DEPTH+1];
	tracker_cell posting_buffer_22[POST_DEPTH+1];
	tracker_cell posting_buffer_23[POST_DEPTH+1];
	tracker_cell posting_buffer_24[POST_DEPTH+1];
	tracker_cell posting_buffer_25[POST_DEPTH+1];
	tracker_cell posting_buffer_26[POST_DEPTH+1];
	tracker_cell posting_buffer_27[POST_DEPTH+1];
	tracker_cell posting_buffer_28[POST_DEPTH+1];
	tracker_cell posting_buffer_29[POST_DEPTH+1];
	tracker_cell posting_buffer_30[POST_DEPTH+1];
	tracker_cell posting_buffer_31[POST_DEPTH+1];
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
	   initPriorityReset[8] = 0;
	   initPriorityReset[9] = 0;
	   initPriorityReset[10] = 0;
	   initPriorityReset[11] = 0;
	   initPriorityReset[12] = 0;
	   initPriorityReset[13] = 0;
	   initPriorityReset[14] = 0;
	   initPriorityReset[15] = 0;
	   initPriorityReset[16] = 0;
	   initPriorityReset[17] = 0;
	   initPriorityReset[18] = 0;
	   initPriorityReset[19] = 0;
	   initPriorityReset[20] = 0;
	   initPriorityReset[21] = 0;
	   initPriorityReset[22] = 0;
	   initPriorityReset[23] = 0;
	   initPriorityReset[24] = 0;
	   initPriorityReset[25] = 0;
	   initPriorityReset[26] = 0;
	   initPriorityReset[27] = 0;
	   initPriorityReset[28] = 0;
	   initPriorityReset[29] = 0;
	   initPriorityReset[30] = 0;
	   initPriorityReset[31] = 0;

	   initBandwidthReset[0] = 0;
	   initBandwidthReset[1] = 0;
	   initBandwidthReset[2] = 0;
	   initBandwidthReset[3] = 0;
	   initBandwidthReset[4] = 0;
	   initBandwidthReset[5] = 0;
	   initBandwidthReset[6] = 0;
	   initBandwidthReset[7] = 0;
	   initBandwidthReset[8] = 0;
	   initBandwidthReset[9] = 0;
	   initBandwidthReset[10] = 0;
	   initBandwidthReset[11] = 0;
	   initBandwidthReset[12] = 0;
	   initBandwidthReset[13] = 0;
	   initBandwidthReset[14] = 0;
	   initBandwidthReset[15] = 0;
	   initBandwidthReset[16] = 0;
	   initBandwidthReset[17] = 0;
	   initBandwidthReset[18] = 0;
	   initBandwidthReset[19] = 0;
	   initBandwidthReset[20] = 0;
	   initBandwidthReset[21] = 0;
	   initBandwidthReset[22] = 0;
	   initBandwidthReset[23] = 0;
	   initBandwidthReset[24] = 0;
	   initBandwidthReset[25] = 0;
	   initBandwidthReset[26] = 0;
	   initBandwidthReset[27] = 0;
	   initBandwidthReset[28] = 0;
	   initBandwidthReset[29] = 0;
	   initBandwidthReset[30] = 0;
	   initBandwidthReset[31] = 0;

	   initLatencyReset[0] = 0;
	   initLatencyReset[1] = 0;
	   initLatencyReset[2] = 0;
	   initLatencyReset[3] = 0;
	   initLatencyReset[4] = 0;
	   initLatencyReset[5] = 0;
	   initLatencyReset[6] = 0;
	   initLatencyReset[7] = 0;
	   initLatencyReset[8] = 0;
	   initLatencyReset[9] = 0;
	   initLatencyReset[10] = 0;
	   initLatencyReset[11] = 0;
	   initLatencyReset[12] = 0;
	   initLatencyReset[13] = 0;
	   initLatencyReset[14] = 0;
	   initLatencyReset[15] = 0;
	   initLatencyReset[16] = 0;
	   initLatencyReset[17] = 0;
	   initLatencyReset[18] = 0;
	   initLatencyReset[19] = 0;
	   initLatencyReset[20] = 0;
	   initLatencyReset[21] = 0;
	   initLatencyReset[22] = 0;
	   initLatencyReset[23] = 0;
	   initLatencyReset[24] = 0;
	   initLatencyReset[25] = 0;
	   initLatencyReset[26] = 0;
	   initLatencyReset[27] = 0;
	   initLatencyReset[28] = 0;
	   initLatencyReset[29] = 0;
	   initLatencyReset[30] = 0;
	   initLatencyReset[31] = 0;

	   /*** Reset target registers to the default values ***/
	   targPriorityReset[0] = 0;
	   targPriorityReset[1] = 0;
	   targPriorityReset[2] = 0;
	   targPriorityReset[3] = 0;
	   targPriorityReset[4] = 0;
	   targPriorityReset[5] = 0;
	   targPriorityReset[6] = 0;
	   targPriorityReset[7] = 0;
	   targPriorityReset[8] = 0;
	   targPriorityReset[9] = 0;
	   targPriorityReset[10] = 0;
	   targPriorityReset[11] = 0;
	   targPriorityReset[12] = 0;
	   targPriorityReset[13] = 0;
	   targPriorityReset[14] = 0;
	   targPriorityReset[15] = 0;
	   targPriorityReset[16] = 0;
	   targPriorityReset[17] = 0;
	   targPriorityReset[18] = 0;
	   targPriorityReset[19] = 0;
	   targPriorityReset[20] = 0;
	   targPriorityReset[21] = 0;
	   targPriorityReset[22] = 0;
	   targPriorityReset[23] = 0;
	   targPriorityReset[24] = 0;
	   targPriorityReset[25] = 0;
	   targPriorityReset[26] = 0;
	   targPriorityReset[27] = 0;
	   targPriorityReset[28] = 0;
	   targPriorityReset[29] = 0;
	   targPriorityReset[30] = 0;
	   targPriorityReset[31] = 0;

	   /* tracker fifo initialization */
	   fifo_reset(&tracker_fifo_struct[0], 0);
	   fifo_reset(&tracker_fifo_struct[1], 0);
	   fifo_reset(&tracker_fifo_struct[2], 0);
	   fifo_reset(&tracker_fifo_struct[3], 0);
	   fifo_reset(&tracker_fifo_struct[4], 0);
	   fifo_reset(&tracker_fifo_struct[5], 0);
	   fifo_reset(&tracker_fifo_struct[6], 0);
	   fifo_reset(&tracker_fifo_struct[7], 0);
	   fifo_reset(&tracker_fifo_struct[8], 0);
	   fifo_reset(&tracker_fifo_struct[9], 0);
	   fifo_reset(&tracker_fifo_struct[10], 0);
	   fifo_reset(&tracker_fifo_struct[11], 0);
	   fifo_reset(&tracker_fifo_struct[12], 0);
	   fifo_reset(&tracker_fifo_struct[13], 0);
	   fifo_reset(&tracker_fifo_struct[14], 0);
	   fifo_reset(&tracker_fifo_struct[15], 0);
	   fifo_reset(&tracker_fifo_struct[16], 0);
	   fifo_reset(&tracker_fifo_struct[17], 0);
	   fifo_reset(&tracker_fifo_struct[18], 0);
	   fifo_reset(&tracker_fifo_struct[19], 0);
	   fifo_reset(&tracker_fifo_struct[20], 0);
	   fifo_reset(&tracker_fifo_struct[21], 0);
	   fifo_reset(&tracker_fifo_struct[22], 0);
	   fifo_reset(&tracker_fifo_struct[23], 0);
	   fifo_reset(&tracker_fifo_struct[24], 0);
	   fifo_reset(&tracker_fifo_struct[25], 0);
	   fifo_reset(&tracker_fifo_struct[26], 0);
	   fifo_reset(&tracker_fifo_struct[27], 0);
	   fifo_reset(&tracker_fifo_struct[28], 0);
	   fifo_reset(&tracker_fifo_struct[29], 0);
	   fifo_reset(&tracker_fifo_struct[30], 0);
	   fifo_reset(&tracker_fifo_struct[31], 0);
           for (i = 0; i < NBR_TARGET; i++)
           {
              tracker_state[i] = 0;
              tracker_full_out[i] = 0;
           }
    }

#ifdef WITH_POWER_NODE
    Node_1_base_class_32x32shared (sc_module_name nm, double req_wnd, sc_time_unit req_wnd_unit) 
      : POWER_NODE <TDATA, TADDRESS, TBE, STBUS_SIZE, NBR_INITIATOR, NBR_TARGET, 
                    RESOURCE_NUMBER_K, RETURN_RESOURCE_NUMBER_K, 3> 
                    (nm,req_wnd,req_wnd_unit)
#else
    Node_1_base_class_32x32shared (sc_module_name nm, double req_wnd, sc_time_unit req_wnd_unitm) 
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
	targMemMapRegionsNb[8] = 1;
	targMemMapRegionsNb[9] = 1;
	targMemMapRegionsNb[10] = 1;
	targMemMapRegionsNb[11] = 1;
	targMemMapRegionsNb[12] = 1;
	targMemMapRegionsNb[13] = 1;
	targMemMapRegionsNb[14] = 1;
	targMemMapRegionsNb[15] = 1;
	targMemMapRegionsNb[16] = 1;
	targMemMapRegionsNb[17] = 1;
	targMemMapRegionsNb[18] = 1;
	targMemMapRegionsNb[19] = 1;
	targMemMapRegionsNb[20] = 1;
	targMemMapRegionsNb[21] = 1;
	targMemMapRegionsNb[22] = 1;
	targMemMapRegionsNb[23] = 1;
	targMemMapRegionsNb[24] = 1;
	targMemMapRegionsNb[25] = 1;
	targMemMapRegionsNb[26] = 1;
	targMemMapRegionsNb[27] = 1;
	targMemMapRegionsNb[28] = 1;
	targMemMapRegionsNb[29] = 1;
	targMemMapRegionsNb[30] = 1;
	targMemMapRegionsNb[31] = 1;

#define PRINTTARGET		

	for(uint i=0; i<32; i++)
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
	ord_blk_filter_on[8] = false;
	ord_blk_filter_depth[8] = 0;
	ord_blk_filter_on[9] = false;
	ord_blk_filter_depth[9] = 0;
	ord_blk_filter_on[10] = false;
	ord_blk_filter_depth[10] = 0;
	ord_blk_filter_on[11] = false;
	ord_blk_filter_depth[11] = 0;
	ord_blk_filter_on[12] = false;
	ord_blk_filter_depth[12] = 0;
	ord_blk_filter_on[13] = false;
	ord_blk_filter_depth[13] = 0;
	ord_blk_filter_on[14] = false;
	ord_blk_filter_depth[14] = 0;
	ord_blk_filter_on[15] = false;
	ord_blk_filter_depth[15] = 0;
	ord_blk_filter_on[16] = false;
	ord_blk_filter_depth[16] = 0;
	ord_blk_filter_on[17] = false;
	ord_blk_filter_depth[17] = 0;
	ord_blk_filter_on[18] = false;
	ord_blk_filter_depth[18] = 0;
	ord_blk_filter_on[19] = false;
	ord_blk_filter_depth[19] = 0;
	ord_blk_filter_on[20] = false;
	ord_blk_filter_depth[20] = 0;
	ord_blk_filter_on[21] = false;
	ord_blk_filter_depth[21] = 0;
	ord_blk_filter_on[22] = false;
	ord_blk_filter_depth[22] = 0;
	ord_blk_filter_on[23] = false;
	ord_blk_filter_depth[23] = 0;
	ord_blk_filter_on[24] = false;
	ord_blk_filter_depth[24] = 0;
	ord_blk_filter_on[25] = false;
	ord_blk_filter_depth[25] = 0;
	ord_blk_filter_on[26] = false;
	ord_blk_filter_depth[26] = 0;
	ord_blk_filter_on[27] = false;
	ord_blk_filter_depth[27] = 0;
	ord_blk_filter_on[28] = false;
	ord_blk_filter_depth[28] = 0;
	ord_blk_filter_on[29] = false;
	ord_blk_filter_depth[29] = 0;
	ord_blk_filter_on[30] = false;
	ord_blk_filter_depth[30] = 0;
	ord_blk_filter_on[31] = false;
	ord_blk_filter_depth[31] = 0;

	/* target resource mapping initialization */
	targResourceMapping[0] = 0;
	targResourceMapping[1] = 0;
	targResourceMapping[2] = 0;
	targResourceMapping[3] = 0;
	targResourceMapping[4] = 0;
	targResourceMapping[5] = 0;
	targResourceMapping[6] = 0;
	targResourceMapping[7] = 0;
	targResourceMapping[8] = 0;
	targResourceMapping[9] = 0;
	targResourceMapping[10] = 0;
	targResourceMapping[11] = 0;
	targResourceMapping[12] = 0;
	targResourceMapping[13] = 0;
	targResourceMapping[14] = 0;
	targResourceMapping[15] = 0;
	targResourceMapping[16] = 0;
	targResourceMapping[17] = 0;
	targResourceMapping[18] = 0;
	targResourceMapping[19] = 0;
	targResourceMapping[20] = 0;
	targResourceMapping[21] = 0;
	targResourceMapping[22] = 0;
	targResourceMapping[23] = 0;
	targResourceMapping[24] = 0;
	targResourceMapping[25] = 0;
	targResourceMapping[26] = 0;
	targResourceMapping[27] = 0;
	targResourceMapping[28] = 0;
	targResourceMapping[29] = 0;
	targResourceMapping[30] = 0;
	targResourceMapping[31] = 0;

	/* initiator return resource mapping initialization */
	initReturnResourceMapping[0] = 0;
	initReturnResourceMapping[1] = 0;
	initReturnResourceMapping[2] = 0;
	initReturnResourceMapping[3] = 0;
	initReturnResourceMapping[4] = 0;
	initReturnResourceMapping[5] = 0;
	initReturnResourceMapping[6] = 0;
	initReturnResourceMapping[7] = 0;
	initReturnResourceMapping[8] = 0;
	initReturnResourceMapping[9] = 0;
	initReturnResourceMapping[10] = 0;
	initReturnResourceMapping[11] = 0;
	initReturnResourceMapping[12] = 0;
	initReturnResourceMapping[13] = 0;
	initReturnResourceMapping[14] = 0;
	initReturnResourceMapping[15] = 0;
	initReturnResourceMapping[16] = 0;
	initReturnResourceMapping[17] = 0;
	initReturnResourceMapping[18] = 0;
	initReturnResourceMapping[19] = 0;
	initReturnResourceMapping[20] = 0;
	initReturnResourceMapping[21] = 0;
	initReturnResourceMapping[22] = 0;
	initReturnResourceMapping[23] = 0;
	initReturnResourceMapping[24] = 0;
	initReturnResourceMapping[25] = 0;
	initReturnResourceMapping[26] = 0;
	initReturnResourceMapping[27] = 0;
	initReturnResourceMapping[28] = 0;
	initReturnResourceMapping[29] = 0;
	initReturnResourceMapping[30] = 0;
	initReturnResourceMapping[31] = 0;

	/* resource mappimg number and elements */;
	number_target_per_resource[0] = 32;

	/* return resource mappimg number and elements */;
	number_initiator_per_return_resource[0] = 32;

	/* auxiliary vector initialization */
	p_req_aux1 = ord_blk_output;
	p_req_aux2 = p_req_aux1;
	p_req_aux3 = transaction_management_output;
	p_r_req_aux1 = response_source_decoder_output;
	p_r_req_aux2 = response_transaction_management_output;

	/*** set resource arbitration vector to the default values ***/
	resourceArbitration[0] = 2;

	/*** set resource arbitration vector to the default values ***/
	resourceLd8Interrupt[0] = false;

	/*** set response resource arbitration vector to the default values ***/
	response_resourceArbitration[0] = 2;

	/* tracker array initialization */

	/*** set source range vectors ***/
	init_src_min[0] = 0;
	init_src_max[0] = 0;
	init_src_mask[0] = 0;
	init_src_min[1] = 1;
	init_src_max[1] = 1;
	init_src_mask[1] = 0;
	init_src_min[2] = 2;
	init_src_max[2] = 2;
	init_src_mask[2] = 0;
	init_src_min[3] = 3;
	init_src_max[3] = 3;
	init_src_mask[3] = 0;
	init_src_min[4] = 4;
	init_src_max[4] = 4;
	init_src_mask[4] = 0;
	init_src_min[5] = 5;
	init_src_max[5] = 5;
	init_src_mask[5] = 0;
	init_src_min[6] = 6;
	init_src_max[6] = 6;
	init_src_mask[6] = 0;
	init_src_min[7] = 7;
	init_src_max[7] = 7;
	init_src_mask[7] = 0;
	init_src_min[8] = 8;
	init_src_max[8] = 8;
	init_src_mask[8] = 0;
	init_src_min[9] = 9;
	init_src_max[9] = 9;
	init_src_mask[9] = 0;
	init_src_min[10] = 10;
	init_src_max[10] = 10;
	init_src_mask[10] = 0;
	init_src_min[11] = 11;
	init_src_max[11] = 11;
	init_src_mask[11] = 0;
	init_src_min[12] = 12;
	init_src_max[12] = 12;
	init_src_mask[12] = 0;
	init_src_min[13] = 13;
	init_src_max[13] = 13;
	init_src_mask[13] = 0;
	init_src_min[14] = 14;
	init_src_max[14] = 14;
	init_src_mask[14] = 0;
	init_src_min[15] = 15;
	init_src_max[15] = 15;
	init_src_mask[15] = 0;
	init_src_min[16] = 16;
	init_src_max[16] = 16;
	init_src_mask[16] = 0;
	init_src_min[17] = 17;
	init_src_max[17] = 17;
	init_src_mask[17] = 0;
	init_src_min[18] = 18;
	init_src_max[18] = 18;
	init_src_mask[18] = 0;
	init_src_min[19] = 19;
	init_src_max[19] = 19;
	init_src_mask[19] = 0;
	init_src_min[20] = 20;
	init_src_max[20] = 20;
	init_src_mask[20] = 0;
	init_src_min[21] = 21;
	init_src_max[21] = 21;
	init_src_mask[21] = 0;
	init_src_min[22] = 22;
	init_src_max[22] = 22;
	init_src_mask[22] = 0;
	init_src_min[23] = 23;
	init_src_max[23] = 23;
	init_src_mask[23] = 0;
	init_src_min[24] = 24;
	init_src_max[24] = 24;
	init_src_mask[24] = 0;
	init_src_min[25] = 25;
	init_src_max[25] = 25;
	init_src_mask[25] = 0;
	init_src_min[26] = 26;
	init_src_max[26] = 26;
	init_src_mask[26] = 0;
	init_src_min[27] = 27;
	init_src_max[27] = 27;
	init_src_mask[27] = 0;
	init_src_min[28] = 28;
	init_src_max[28] = 28;
	init_src_mask[28] = 0;
	init_src_min[29] = 29;
	init_src_max[29] = 29;
	init_src_mask[29] = 0;
	init_src_min[30] = 30;
	init_src_max[30] = 30;
	init_src_mask[30] = 0;
	init_src_min[31] = 31;
	init_src_max[31] = 31;
	init_src_mask[31] = 0;

	/*** Preset the bandwidth on flag vector ***/
	initBandwidthLimiterOn[0] = 0;
	initBandwidthLimiterOn[1] = 0;
	initBandwidthLimiterOn[2] = 0;
	initBandwidthLimiterOn[3] = 0;
	initBandwidthLimiterOn[4] = 0;
	initBandwidthLimiterOn[5] = 0;
	initBandwidthLimiterOn[6] = 0;
	initBandwidthLimiterOn[7] = 0;
	initBandwidthLimiterOn[8] = 0;
	initBandwidthLimiterOn[9] = 0;
	initBandwidthLimiterOn[10] = 0;
	initBandwidthLimiterOn[11] = 0;
	initBandwidthLimiterOn[12] = 0;
	initBandwidthLimiterOn[13] = 0;
	initBandwidthLimiterOn[14] = 0;
	initBandwidthLimiterOn[15] = 0;
	initBandwidthLimiterOn[16] = 0;
	initBandwidthLimiterOn[17] = 0;
	initBandwidthLimiterOn[18] = 0;
	initBandwidthLimiterOn[19] = 0;
	initBandwidthLimiterOn[20] = 0;
	initBandwidthLimiterOn[21] = 0;
	initBandwidthLimiterOn[22] = 0;
	initBandwidthLimiterOn[23] = 0;
	initBandwidthLimiterOn[24] = 0;
	initBandwidthLimiterOn[25] = 0;
	initBandwidthLimiterOn[26] = 0;
	initBandwidthLimiterOn[27] = 0;
	initBandwidthLimiterOn[28] = 0;
	initBandwidthLimiterOn[29] = 0;
	initBandwidthLimiterOn[30] = 0;
	initBandwidthLimiterOn[31] = 0;

	/*** Preset Low Priority vector ***/
	initLowPriorityReset[0] = 0;
	initLowPriorityReset[1] = 0;
	initLowPriorityReset[2] = 0;
	initLowPriorityReset[3] = 0;
	initLowPriorityReset[4] = 0;
	initLowPriorityReset[5] = 0;
	initLowPriorityReset[6] = 0;
	initLowPriorityReset[7] = 0;
	initLowPriorityReset[8] = 0;
	initLowPriorityReset[9] = 0;
	initLowPriorityReset[10] = 0;
	initLowPriorityReset[11] = 0;
	initLowPriorityReset[12] = 0;
	initLowPriorityReset[13] = 0;
	initLowPriorityReset[14] = 0;
	initLowPriorityReset[15] = 0;
	initLowPriorityReset[16] = 0;
	initLowPriorityReset[17] = 0;
	initLowPriorityReset[18] = 0;
	initLowPriorityReset[19] = 0;
	initLowPriorityReset[20] = 0;
	initLowPriorityReset[21] = 0;
	initLowPriorityReset[22] = 0;
	initLowPriorityReset[23] = 0;
	initLowPriorityReset[24] = 0;
	initLowPriorityReset[25] = 0;
	initLowPriorityReset[26] = 0;
	initLowPriorityReset[27] = 0;
	initLowPriorityReset[28] = 0;
	initLowPriorityReset[29] = 0;
	initLowPriorityReset[30] = 0;
	initLowPriorityReset[31] = 0;

	/*** Preset the Frame Size vector ***/
	initFrameSizeReset[0] = 0;
	initFrameSizeReset[1] = 0;
	initFrameSizeReset[2] = 0;
	initFrameSizeReset[3] = 0;
	initFrameSizeReset[4] = 0;
	initFrameSizeReset[5] = 0;
	initFrameSizeReset[6] = 0;
	initFrameSizeReset[7] = 0;
	initFrameSizeReset[8] = 0;
	initFrameSizeReset[9] = 0;
	initFrameSizeReset[10] = 0;
	initFrameSizeReset[11] = 0;
	initFrameSizeReset[12] = 0;
	initFrameSizeReset[13] = 0;
	initFrameSizeReset[14] = 0;
	initFrameSizeReset[15] = 0;
	initFrameSizeReset[16] = 0;
	initFrameSizeReset[17] = 0;
	initFrameSizeReset[18] = 0;
	initFrameSizeReset[19] = 0;
	initFrameSizeReset[20] = 0;
	initFrameSizeReset[21] = 0;
	initFrameSizeReset[22] = 0;
	initFrameSizeReset[23] = 0;
	initFrameSizeReset[24] = 0;
	initFrameSizeReset[25] = 0;
	initFrameSizeReset[26] = 0;
	initFrameSizeReset[27] = 0;
	initFrameSizeReset[28] = 0;
	initFrameSizeReset[29] = 0;
	initFrameSizeReset[30] = 0;
	initFrameSizeReset[31] = 0;

	/*** Preset the Words Limit vector ***/
	initWordsLimitReset[0] = 0;
	initWordsLimitReset[1] = 0;
	initWordsLimitReset[2] = 0;
	initWordsLimitReset[3] = 0;
	initWordsLimitReset[4] = 0;
	initWordsLimitReset[5] = 0;
	initWordsLimitReset[6] = 0;
	initWordsLimitReset[7] = 0;
	initWordsLimitReset[8] = 0;
	initWordsLimitReset[9] = 0;
	initWordsLimitReset[10] = 0;
	initWordsLimitReset[11] = 0;
	initWordsLimitReset[12] = 0;
	initWordsLimitReset[13] = 0;
	initWordsLimitReset[14] = 0;
	initWordsLimitReset[15] = 0;
	initWordsLimitReset[16] = 0;
	initWordsLimitReset[17] = 0;
	initWordsLimitReset[18] = 0;
	initWordsLimitReset[19] = 0;
	initWordsLimitReset[20] = 0;
	initWordsLimitReset[21] = 0;
	initWordsLimitReset[22] = 0;
	initWordsLimitReset[23] = 0;
	initWordsLimitReset[24] = 0;
	initWordsLimitReset[25] = 0;
	initWordsLimitReset[26] = 0;
	initWordsLimitReset[27] = 0;
	initWordsLimitReset[28] = 0;
	initWordsLimitReset[29] = 0;
	initWordsLimitReset[30] = 0;
	initWordsLimitReset[31] = 0;

	//In our realise of Node doesn't exist...Poletti: boh!?!?

	/*** Preset the Moving Window vector ***/
	initBandwidthLimiterMovingWindowOn[0] = 0;
	initBandwidthLimiterMovingWindowOn[1] = 0;
	initBandwidthLimiterMovingWindowOn[2] = 0;
	initBandwidthLimiterMovingWindowOn[3] = 0;
	initBandwidthLimiterMovingWindowOn[4] = 0;
	initBandwidthLimiterMovingWindowOn[5] = 0;
	initBandwidthLimiterMovingWindowOn[6] = 0;
	initBandwidthLimiterMovingWindowOn[7] = 0;
	initBandwidthLimiterMovingWindowOn[8] = 0;
	initBandwidthLimiterMovingWindowOn[9] = 0;
	initBandwidthLimiterMovingWindowOn[10] = 0;
	initBandwidthLimiterMovingWindowOn[11] = 0;
	initBandwidthLimiterMovingWindowOn[12] = 0;
	initBandwidthLimiterMovingWindowOn[13] = 0;
	initBandwidthLimiterMovingWindowOn[14] = 0;
	initBandwidthLimiterMovingWindowOn[15] = 0;
	initBandwidthLimiterMovingWindowOn[16] = 0;
	initBandwidthLimiterMovingWindowOn[17] = 0;
	initBandwidthLimiterMovingWindowOn[18] = 0;
	initBandwidthLimiterMovingWindowOn[19] = 0;
	initBandwidthLimiterMovingWindowOn[20] = 0;
	initBandwidthLimiterMovingWindowOn[21] = 0;
	initBandwidthLimiterMovingWindowOn[22] = 0;
	initBandwidthLimiterMovingWindowOn[23] = 0;
	initBandwidthLimiterMovingWindowOn[24] = 0;
	initBandwidthLimiterMovingWindowOn[25] = 0;
	initBandwidthLimiterMovingWindowOn[26] = 0;
	initBandwidthLimiterMovingWindowOn[27] = 0;
	initBandwidthLimiterMovingWindowOn[28] = 0;
	initBandwidthLimiterMovingWindowOn[29] = 0;
	initBandwidthLimiterMovingWindowOn[30] = 0;
	initBandwidthLimiterMovingWindowOn[31] = 0;


	/*** set priority type vector ***/
	initPriorityType[0] = 1;
	initPriorityType[1] = 1;
	initPriorityType[2] = 1;
	initPriorityType[3] = 1;
	initPriorityType[4] = 1;
	initPriorityType[5] = 1;
	initPriorityType[6] = 1;
	initPriorityType[7] = 1;
	initPriorityType[8] = 1;
	initPriorityType[9] = 1;
	initPriorityType[10] = 1;
	initPriorityType[11] = 1;
	initPriorityType[12] = 1;
	initPriorityType[13] = 1;
	initPriorityType[14] = 1;
	initPriorityType[15] = 1;
	initPriorityType[16] = 1;
	initPriorityType[17] = 1;
	initPriorityType[18] = 1;
	initPriorityType[19] = 1;
	initPriorityType[20] = 1;
	initPriorityType[21] = 1;
	initPriorityType[22] = 1;
	initPriorityType[23] = 1;
	initPriorityType[24] = 1;
	initPriorityType[25] = 1;
	initPriorityType[26] = 1;
	initPriorityType[27] = 1;
	initPriorityType[28] = 1;
	initPriorityType[29] = 1;
	initPriorityType[30] = 1;
	initPriorityType[31] = 1;

	/*** Preset init interrupt rule reset ***/
	init_interrupt_rule_reset[0] = 0;
	init_interrupt_rule_reset[1] = 0;
	init_interrupt_rule_reset[2] = 0;
	init_interrupt_rule_reset[3] = 0;
	init_interrupt_rule_reset[4] = 0;
	init_interrupt_rule_reset[5] = 0;
	init_interrupt_rule_reset[6] = 0;
	init_interrupt_rule_reset[7] = 0;
	init_interrupt_rule_reset[8] = 0;
	init_interrupt_rule_reset[9] = 0;
	init_interrupt_rule_reset[10] = 0;
	init_interrupt_rule_reset[11] = 0;
	init_interrupt_rule_reset[12] = 0;
	init_interrupt_rule_reset[13] = 0;
	init_interrupt_rule_reset[14] = 0;
	init_interrupt_rule_reset[15] = 0;
	init_interrupt_rule_reset[16] = 0;
	init_interrupt_rule_reset[17] = 0;
	init_interrupt_rule_reset[18] = 0;
	init_interrupt_rule_reset[19] = 0;
	init_interrupt_rule_reset[20] = 0;
	init_interrupt_rule_reset[21] = 0;
	init_interrupt_rule_reset[22] = 0;
	init_interrupt_rule_reset[23] = 0;
	init_interrupt_rule_reset[24] = 0;
	init_interrupt_rule_reset[25] = 0;
	init_interrupt_rule_reset[26] = 0;
	init_interrupt_rule_reset[27] = 0;
	init_interrupt_rule_reset[28] = 0;
	init_interrupt_rule_reset[29] = 0;
	init_interrupt_rule_reset[30] = 0;
	init_interrupt_rule_reset[31] = 0;

	/*** Preset init interrupt rule reset ***/
	init_max_opcode_size[0] = 5;
	init_max_opcode_size[1] = 5;
	init_max_opcode_size[2] = 5;
	init_max_opcode_size[3] = 5;
	init_max_opcode_size[4] = 5;
	init_max_opcode_size[5] = 5;
	init_max_opcode_size[6] = 5;
	init_max_opcode_size[7] = 5;
	init_max_opcode_size[8] = 5;
	init_max_opcode_size[9] = 5;
	init_max_opcode_size[10] = 5;
	init_max_opcode_size[11] = 5;
	init_max_opcode_size[12] = 5;
	init_max_opcode_size[13] = 5;
	init_max_opcode_size[14] = 5;
	init_max_opcode_size[15] = 5;
	init_max_opcode_size[16] = 5;
	init_max_opcode_size[17] = 5;
	init_max_opcode_size[18] = 5;
	init_max_opcode_size[19] = 5;
	init_max_opcode_size[20] = 5;
	init_max_opcode_size[21] = 5;
	init_max_opcode_size[22] = 5;
	init_max_opcode_size[23] = 5;
	init_max_opcode_size[24] = 5;
	init_max_opcode_size[25] = 5;
	init_max_opcode_size[26] = 5;
	init_max_opcode_size[27] = 5;
	init_max_opcode_size[28] = 5;
	init_max_opcode_size[29] = 5;
	init_max_opcode_size[30] = 5;
	init_max_opcode_size[31] = 5;
    }
};
