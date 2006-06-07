
// ****************************************************************************
//       _____________
//      / ______   __/
//      \ \     / /
//       \ \   / /
//    ____\ \ / /
//   /      // / R 
// 
// ****************************************************************************
// SC_NODE.h
// ****************************************************************************
//
// Description : BCA SystemC STBus Node
// Created by  : Carlo SPITALE - IPMV - FMVG - CMG-Design
// On          : Wednesday 14/02/2001
//
// Updated by  : Nizar ROMDHANE - OCCS - CMG-Design
// Since       : Monday 03/03/2003
//
// ****************************************************************************
//
//  $Log: SC_NODE.h,v $
//  Revision 1.3  2006/01/12 16:47:08  poletti
//  Added a partial cross bar instatiation
//  Fixed a bug in the FREQUENCY DEVICE instatiation
//
//  Revision 1.9  2004/12/02 13:30:31  romdanen
//  Merged branch release-2-5-dev to Trunk
//
//  Revision 1.8.2.1  2004/11/30 10:54:16  romdanen
//  Fixed again src propagation (no more propagated in case of trackers)
//
//  Revision 1.8  2004/10/12 14:17:10  romdanen
//  Merged release-2-4-dev to trunc
//
//  Revision 1.7.2.2  2004/09/27 10:18:59  romdanen
//  Bandwidth Limiter bug: Set to 0 FSMs variables
//
//  Revision 1.7.2.1  2004/09/24 09:54:53  romdanen
//  Fixed a bug in Bandwidth Limiter programming
//
//  Revision 1.7  2004/07/27 13:34:23  romdanen
//  Added Trackers and Filters tracing
//
//  Revision 1.6  2004/07/14 11:29:39  romdanen
//  Fixed Bugs in Bandwidth Limiter, Message Based Arbitration Scheme & Porgramming Port
//
//  Revision 1.5  2004/03/18 11:18:47  romdanen
//  Fixed adresse decoding bug in case of multi regions
//
//  Revision 1.4  2004/01/22 10:25:02  ipmodels
//  Added message_timeout_enabled parameter to enable or disable the message timeout mechanism
//
//  Revision 1.3  2004/01/21 09:22:16  ipmodels
//  Merged Branch release-2-0-dev to trunk
//
//  Revision 1.2.2.4  2004/01/20 16:17:12  ipmodels
//  Updated Header
//
//  Revision 1.2.2.3  2003/11/14 18:55:19  ipmodels
//  Message Based Alog load8 grouping feature excluded from 128 & 256 bits
//
//  Revision 1.2.2.2  2003/11/06 12:25:10  ipmodels
//  Message Based Bug fix when timeout is set to 0 - Dependancy stages modified (gnt & r_gnt basis)
//
//  Revision 1.2.2.1  2003/10/28 11:35:49  ipmodels
//  Fix Bug in load operation bigger than Bus size
//
//  Revision 1.2  2003/10/03 12:53:31  ipmodels
//  Aligned on RTL v17.1 : Bandwidth Limiter, Message Based & WritePosting Updates
//
// ****************************************************************************
                                                                                                                                                  
#ifndef ST_NODE_H
#define ST_NODE_H

#define STATIC 0
#define PRIORITY 1
#define RESPONSE_DYNAMIC_PRIORITY 0
#define RESPONSE_VARIABLE_PRIORITY 1
#define LRU      2
#define LATENCY  3
#define BANDWITH 4
#define STB      5
#define MSG_BASED 6

#define REGISTER_PRI 1
#define EXT_PRI 0

#define MAX_INIT_PRI 31
#define MAX_INIT_LRU 31

/*** shared macros ***/
#define tracker_full(i)  fifo_full(&tracker_fifo_struct[i])
#define tracker_empty(i) fifo_empty(&tracker_fifo_struct[i])
#define tracker_first(i) (tracker_array[i] + fifo_first(&tracker_fifo_struct[i]))
#define tracker_last(i)  (tracker_array[i] + fifo_last(&tracker_fifo_struct[i]))
#define tracker_get(i)   *(tracker_array[i] + fifo_get(&tracker_fifo_struct[i]))
#define tracker_put(i)   (tracker_array[i] + fifo_in_put(&tracker_fifo_struct[i]))

#define postfifo_empty(i) fifo_empty(&posting_fifo_struct[i])
#define postfifo_full(i)  fifo_full(&posting_fifo_struct[i])
#define postfifo_get(i)   *(posting_array[i] + fifo_get(&posting_fifo_struct[i]))
#define postfifo_first(i) (posting_array[i] + fifo_first(&posting_fifo_struct[i]))
#define postfifo_put(i)   (posting_array[i] + fifo_in_put(&posting_fifo_struct[i]))

/*** shared structures ***/

/* generic fifo structure */
typedef struct
{
  uchar in, out, count, fifosize;
  void * item;
} fifo;

typedef struct
{
  uint src, tid, pri, lck;
  uint num_cell;
} tracker_cell;

/*** functions ***/

/* fifo methods */

uchar fifo_next(fifo * pf, uchar actual) 
{
  if(actual >= pf->fifosize - 1) return 0;
  return ++actual;
}

uchar fifo_prev(fifo * pf, uchar actual) 
{
  if(actual == 0) return (pf->fifosize - 1);
  return --actual;
}

uchar fifo_empty(fifo * pf) 
{
  return (pf->count == 0);
}

uchar fifo_full(fifo * pf) 
{
  return (pf->count >= pf->fifosize);
}

uchar fifo_get(fifo * pf) 
{
  if(fifo_empty(pf)) return pf->out;
  pf->out = fifo_next(pf, pf->out);
  pf->count--;
  return fifo_prev(pf, pf->out);
}

uchar fifo_first(fifo * pf) 
{
  return pf->out;
}

uchar fifo_last(fifo * pf) 
{
  return pf->in;
}

uchar fifo_in_put(fifo * pf) 
{
  if(!fifo_full(pf))
    {
      pf->in = fifo_next(pf, pf->in);
      pf->count++;
    }
  return pf->in;
}

void fifo_reset(fifo * pf, uchar size) 
{
  pf->out = 0;
  pf->in = size - 1;
  pf->count = 0;
  pf->fifosize = size;
}

uint fifo_num(fifo * pf) {
  return pf->count;
}

uint fifo_free_num(fifo * pf) 
{
  return(pf->fifosize - pf->count);
}

/*** set bandwidth constant vectors ***/
//uint bandwidthFrameSizeValues[8] = { 0, 133, 266, 400, 533, 666, 800, 933 };
uint bandwidthFrameSizeValues[256];

//uint bandwidthWordsLimitValues[8] = { 8*8, 16*8, 24*8, 32*8, 40*8, 48*8, 56*8, 64*8 };
uint bandwidthWordsLimitValues[32];


template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, int NBR_INITIATOR, int NBR_TARGET, int RESOURCE_NUMBER_K, int RETURN_RESOURCE_NUMBER_K>
SC_MODULE(STBUS_NODE_BCA_with_snoop)
{
  sc_in_clk clk;
  sc_in<bool> rst;

  stbus_T2T3_port<TDATA, TADDRESS, TBE > In[NBR_INITIATOR];
  stbus_T2T3_port<TDATA, TADDRESS, TBE > Out[NBR_TARGET];
#ifndef NOSNOOP
        bool SnoopingActive;
        stbus_T2T3_port_direct<TDATA, TADDRESS, TBE >   SnoopPort_d[RESOURCE_NUMBER_K];
        stbus_T2T3_port_response<TDATA, TADDRESS, TBE > SnoopPort_r[RETURN_RESOURCE_NUMBER_K];
#endif
  stbus_T1_port<uint, uint, uchar > *p_Prog;

  /*************************************/
  /* Node internal data and structures */
  /*************************************/
    
  /*** asynch switch flag ***/
  bool asynch_flag;
  
  /*** request catching window param ***/
  double req_wnd;
  sc_time_unit req_wnd_unit;
  
  /*** stbus type (2, 3) ***/
  uchar stbusType;
  
  /*** initRetime flag***/
  bool initRetime;
  
  /*** initReturnRetime flag***/
  bool initReturnRetime;
  
  /*** targRetime flag***/
  bool targRetime;
  
  /*** targReturnRetime flag***/
  bool targReturnRetime;
  
  /*** posting flag***/
  bool postedValidOn;
  
  /*** programming flag***/
  bool programmingOn;
  
  /*** max number of target regions ***/
  uchar max_nbr_regions;
  
  /*** message timeout ***/
  bool message_timeout_enabled;
  uint message_timeout;
  
  /*** T3 post fifo depth ***/
  uchar post_depth;
  
  /*** memory map arrays ****/
  int targMemMapRegionsNb[NBR_TARGET];
  targ_mem_region<TADDRESS> *targMemMapRegions;
  
  /*** address decoder structures ***/
  /* address decoder vector state */
  uchar address_decoder_state[NBR_INITIATOR];
  /* address decoder output */
  int hot_vector_req[NBR_INITIATOR];
  
  /*** ordering block structures ***/
  /* request counters */
  uint ord_blk_counter[NBR_INITIATOR];
  /* init request packet vector */
  uchar ord_blk_detect_init_req[NBR_INITIATOR];
  /* max request depth */
  uint ord_blk_filter_depth[NBR_INITIATOR];
  /* filter on vector */
  bool ord_blk_filter_on[NBR_INITIATOR];
  /* request register vector*/
  int ord_blk_reg[NBR_INITIATOR];
  /* output vector */
  int ord_blk_output[NBR_INITIATOR];
  
  /* auxiliary request vector pointer */
  int * p_req_aux1;
  int * p_req_aux2;
  int * p_req_aux3;
  int * p_r_req_aux1;
  int * p_r_req_aux2;
  
  /* real request generator structures */
  int real_req[NBR_INITIATOR];
  
  /*** message request mask unit structures ***/
  int message_counter[RESOURCE_NUMBER_K];
  int msg_based_req_mask[NBR_INITIATOR];
  uchar msg_based_req_mask_state[RESOURCE_NUMBER_K];
  int msg_based_current_init[RESOURCE_NUMBER_K];
  int msg_to_be_interrupted[RESOURCE_NUMBER_K];
  
  /* target resource mapping vector : targetResourceMapping[target index] = resource index */
  int targResourceMapping[NBR_TARGET];
  
  /* initiator return resource mapping vector : initReturnResourceMapping[initiator index] = return resource index */
  int initReturnResourceMapping[NBR_INITIATOR];
  
  /* lock management block structures */
  int lock_management_output[NBR_TARGET];
  uchar lock_management_state[NBR_INITIATOR];
  
  /* number of targets per resource vector */;
  int number_target_per_resource[RESOURCE_NUMBER_K];
  
  /* number of initiators per return resource vector */;
  int number_initiator_per_return_resource[RETURN_RESOURCE_NUMBER_K];
  
  /* initiator registers */
  uint initPriorityReset[NBR_INITIATOR];
  uint initPriorityType[NBR_INITIATOR];
  uint initBandwidthReset[NBR_INITIATOR];
  uint initLatencyReset[NBR_INITIATOR];
  /* priority output vector */
  uint initPriorityOut[NBR_INITIATOR];
  /* priority output vector changed flag */
  uchar initPriorityOut_changed;
  
  /*** initiator flags ***/
  uchar init_latency_counter_en_bit[NBR_INITIATOR];
  uchar init_latency_counter_pri_mask[NBR_INITIATOR];
  uint init_interrupt_rule_reset[NBR_INITIATOR];
  uint init_max_opcode_size[NBR_INITIATOR];
  
  /* target registers */
  uint targPriorityReset[NBR_TARGET];
  
  /*** bandwidth limiter register ***/
  uint bandwidthLimiterPriorityOutput[NBR_INITIATOR];
  uint initLowPriorityReset[NBR_INITIATOR];
  uint initFrameSizeReset[NBR_INITIATOR];
  uint initFrameSizeValue[NBR_INITIATOR];
  uint initWordsLimitReset[NBR_INITIATOR];
  uint initWordsLimitValue[NBR_INITIATOR];
  uint bandwidthFrameSizeCounter[NBR_INITIATOR];
  uint bandwidthWordsLimitCounter[NBR_INITIATOR];
  uchar bandwidthPriorityOut_changed;
  bool initBandwidthLimiterOn[NBR_INITIATOR];
  //Added by Nizar
  // Tells if the counter of the frame window is decremented immediately 
  // or after that the request has been granted
  bool initBandwidthLimiterMovingWindowOn[NBR_INITIATOR];
  bool initBandwidthLimiterMovingWindowState[NBR_INITIATOR];
  bool initBandwidthLimiterSwitchLowToHighState[NBR_INITIATOR];
  bool initBandwidthLimiterSwitchHighToLowStateDone[NBR_INITIATOR];
  
  /* resource arbitration vector */
  uchar resourceArbitration[RESOURCE_NUMBER_K];
  bool resourceLd8Interrupt[RESOURCE_NUMBER_K];

  /* return resource arbitration vector */
  uchar response_resourceArbitration[RETURN_RESOURCE_NUMBER_K];
  
  /* control unit structures */
  uchar LRU_initiator_packet_state[NBR_INITIATOR];
  uchar Latency_initiator_packet_state[NBR_INITIATOR];
  uint  Latency_initiator_sort_vect[NBR_INITIATOR];
  uint  Latency_initiator_value_vect[NBR_INITIATOR];
  bool  Latency_resource_changed_counts[RESOURCE_NUMBER_K];
  uchar bandwidth_vect_req_mask[NBR_INITIATOR*RESOURCE_NUMBER_K];
  uchar control_unit_LRU_counters[NBR_INITIATOR*RESOURCE_NUMBER_K];
  uint  control_unit_Latency_counters[NBR_INITIATOR*RESOURCE_NUMBER_K];
  uint  control_unit_Bandwidth_counters[NBR_INITIATOR*RESOURCE_NUMBER_K];
  
  /* priority management unit structures */
  uchar response_LRU_target_packet_state[NBR_TARGET];
  uchar response_LRU_counters[NBR_TARGET*RETURN_RESOURCE_NUMBER_K];
  uchar priority_management_unit_state[RETURN_RESOURCE_NUMBER_K];
  
  /* arbitration unit priorities structures */
  uchar *arbitration_unit_priority;
  uchar *retimed_Latency_arbitration_unit_priority;
  int arbitration_unit_result[RESOURCE_NUMBER_K];
  //Added By Nizar
  uchar arbitration_unit_interrupt_load8_state[RESOURCE_NUMBER_K];
  //End of Add

  /* transaction management unit structures */
  uchar transaction_management_state[RESOURCE_NUMBER_K];
  int transaction_management_output[RESOURCE_NUMBER_K];
  
  /*** tracker structures ***/
  fifo tracker_fifo_struct[NBR_TARGET];
  tracker_cell *tracker_array[NBR_TARGET];
  uchar tracker_state[NBR_TARGET];
  uchar tracker_full_out[NBR_TARGET];
  
  /*** posting structures ***/
  fifo posting_fifo_struct[NBR_INITIATOR];
  tracker_cell *posting_array[NBR_INITIATOR];
  uchar posting_fifo_state[NBR_INITIATOR];
  uchar posting_send_state[NBR_INITIATOR];
  uchar posting_send_out[NBR_INITIATOR];
  
  /* Response Source Decoder structures */
  uchar response_source_decoder_state[NBR_TARGET];
  int response_source_decoder_output[NBR_TARGET];
  
  /* real response request generator structures */
  int real_r_req[NBR_TARGET];
  
  /* response lock management block structures */
  int r_lock_management_output[NBR_INITIATOR];
  uchar r_lock_management_state[NBR_TARGET];
  
  /* response arbitration unit priorities structures */
  uchar *response_arbitration_unit_priority;
  /* response arbitration unit results structures */
  int response_arbitration_unit_result[RETURN_RESOURCE_NUMBER_K];
  
  /* response transaction management unit structures */
  uchar response_transaction_management_state[RETURN_RESOURCE_NUMBER_K];
  int response_transaction_management_output[RETURN_RESOURCE_NUMBER_K];
  int response_message_counter[RETURN_RESOURCE_NUMBER_K];
  
  /* initiator source ranges */
  uint init_src_min[NBR_INITIATOR];
  uint init_src_max[NBR_INITIATOR];
  uchar init_src_mask[NBR_INITIATOR];
  
  /*** init retiming stage structures ***/
  int init_retiming_output[NBR_INITIATOR];
  uchar init_retiming_state[NBR_INITIATOR];
  
  /*** init retiming stage structures ***/
  int target_return_retiming_output[NBR_TARGET];
  uchar target_return_retiming_state[NBR_TARGET];
  
  /*** target retiming stage structures ***/
  int target_retiming_output[RESOURCE_NUMBER_K];
  uchar target_retiming_state[RESOURCE_NUMBER_K];
  
  /*** init retiming stage structures ***/
  int init_return_retiming_output[RETURN_RESOURCE_NUMBER_K];
  uchar init_return_retiming_state[RETURN_RESOURCE_NUMBER_K];
  
  /*** programming port state ***/
  uchar pp_state;

  int i,j,k, clk_counter;
  
  bool rst_flag;

  // Debug variables
  double sim_time;

  virtual void Register_Reset()
    {
      ;
    }
  
  void Reset_Signals()
    {
      /*** reset gnt[i] ***/
      for(uint i=0; i<NBR_INITIATOR; i++)
	In[i].gnt.write(false);
      
      /*** reset r_gnt[i] ***/
      for(uint i=0; i<NBR_TARGET; i++)
	Out[i].r_gnt.write(false);
    }
  
  void Node_Reset()
    {
      /*** Reset register contents ***/
      Register_Reset();
      
      /*** Reset Address Decoder ***/
      for (i=0; i<NBR_INITIATOR; i++)
	{
	  address_decoder_state[i] = 0;
	  hot_vector_req[i] = -1;
	}
      
      /*** Reset Ordering Block structures ***/
      for (i=0; i<NBR_INITIATOR; i++)
	{
	  ord_blk_counter[i] = 0;
	  ord_blk_detect_init_req[i] = 0;
	  ord_blk_reg[i] = -1;
	  ord_blk_output[i] = -1;
	}
      
      /*** Reset real request vectors ***/
      for (i=0; i<NBR_INITIATOR; i++)
	real_req[i] = -1;
      
      /*** Reset lock management output vector ***/
      /* Reset outstanding packet vector (0 = beginning of  packet)*/
      for (i=0; i<NBR_INITIATOR; i++)
	lock_management_state[i] = 0;
      for (i=0; i<NBR_TARGET; i++)
	lock_management_output[i] = -1;
      
      /*** Reset transaction management structures ***/
      for (i=0; i<RESOURCE_NUMBER_K; i++)
	{
	  transaction_management_output[i] = -1;
	  transaction_management_state[i] = 0;
	  message_counter[i] = 0;
	  msg_to_be_interrupted[i] = 0;
	}
      
      /* control unit initialization */
      /*** Reset counters & mask vectors ***/
      for (i = 0; i < NBR_INITIATOR*RESOURCE_NUMBER_K; i++)
	{
	  control_unit_LRU_counters[i] = 0;
	  bandwidth_vect_req_mask[i] = 1;
	  //              control_unit_Latency_counters[i] = initLatencyReset[i%NBR_INITIATOR];
	  control_unit_Latency_counters[i] = 0;
	  control_unit_Bandwidth_counters[i] = 0;
	  retimed_Latency_arbitration_unit_priority[i] = 0;
	}
      /*** Reset states ***/
      for (i = 0; i < NBR_INITIATOR; i++)
	{
	  LRU_initiator_packet_state[i] = 0;
	  Latency_initiator_packet_state[i] = 0;
	  Latency_initiator_sort_vect[i] = i;
	  Latency_initiator_value_vect[i] = 0;
	}
      
      /*** Reset internal Latency flag vector ***/
      for (i = 0; i < RESOURCE_NUMBER_K; i++)
	Latency_resource_changed_counts[i] = true;
      
      /* priority management unit initialization */
      /*** Reset LRU counters ***/
      for (i = 0; i < NBR_TARGET*RETURN_RESOURCE_NUMBER_K; i++)
	response_LRU_counters[i] = 0;
      /*** Reset states ***/
      for (i = 0; i < NBR_TARGET; i++)
	response_LRU_target_packet_state[i] = 0;
      
      /* arbitration unit priorities initialization */
      /*** Reset initiators priorities ***/
      for (i = 0; i < RESOURCE_NUMBER_K; i++)
	for (j = 0; j < NBR_INITIATOR; j++)
	  *(arbitration_unit_priority + i*NBR_INITIATOR + j) = 0;
      
      /* arbitration unit output initialization */
      for (i = 0; i < RESOURCE_NUMBER_K; i++)
	arbitration_unit_result[i] = -1;
 
      //Added By Nizar
      for (i = 0; i < RESOURCE_NUMBER_K; i++)
	arbitration_unit_interrupt_load8_state[i] = 0;

      /*** Reset Response Source Decoder structures ***/
      for (i=0; i<NBR_TARGET; i++)
	{
	  response_source_decoder_state[i] = 0;
	  response_source_decoder_output[i] = -1;
	}
      
      /* Reset response lock management output vector */
      /*** Reset outstanding packet vector (0 = beginning of  packet) ***/
      for (i=0; i<NBR_TARGET; i++)
	r_lock_management_state[i] = 0;
      for (i=0; i<NBR_INITIATOR; i++)
	r_lock_management_output[i] = -1;
      
      /*** Reset real response request vectors ***/
      for (i=0; i<NBR_TARGET; i++)
	real_r_req[i] = -1;
      
      /* response arbitration unit priorities initialization */
      /*** Reset target priorities ***/
      for (i = 0; i < RETURN_RESOURCE_NUMBER_K; i++)
	if(response_resourceArbitration[i] == RESPONSE_DYNAMIC_PRIORITY)
	  for (j = 0; j < NBR_TARGET; j++)
	    {
	      *(response_arbitration_unit_priority + i*NBR_TARGET + j) = targPriorityReset[j];
	    }
	else
	  for (j = 0; j < NBR_TARGET; j++)
	    {
	      *(response_arbitration_unit_priority + i*NBR_TARGET + j) = 0;
	    }
      
      /* response arbitration unit output initialization */
      for (i = 0; i < RETURN_RESOURCE_NUMBER_K; i++)
	response_arbitration_unit_result[i] = -1;
      
      /*** Reset response transaction management structures ***/
      for (i=0; i<RETURN_RESOURCE_NUMBER_K; i++)
	{
	  response_transaction_management_output[i] = -1;
	  response_transaction_management_state[i] = 0;
	  response_message_counter[i] = 0;
	}
      
      // Added By Nizar
      /** Set Frame size Values */
      for (i = 0; i < 256 ; i++)
	bandwidthFrameSizeValues[i] = 8*i;
      
      // Added By Nizar
      /** Set Words Limit Vlues */
      for (i = 0; i < 32 ; i++)
	bandwidthWordsLimitValues[i] = 8*(2*(i+1));
      
      /* Reset Bandwidth Limiter counters */
      for (i=0; i<NBR_INITIATOR; i++)
	{
	  bandwidthFrameSizeCounter[i] = bandwidthFrameSizeValues[initFrameSizeReset[i]]-1;
	  bandwidthWordsLimitCounter[i] = 0;
	  bandwidthLimiterPriorityOutput[i] = 0;
	  initFrameSizeValue[i] = bandwidthFrameSizeValues[initFrameSizeReset[i]];
	  initWordsLimitValue[i] = bandwidthWordsLimitValues[initWordsLimitReset[i]];
	}
      
      /* Reset STB pri mask vector */
      for (i=0; i<NBR_INITIATOR; i++)
	init_latency_counter_pri_mask[i] = 0;
      
      /* Reset init priority output vector */
      for (i=0; i<NBR_INITIATOR; i++)
	initPriorityOut[i] = 0;

      /*** set initPriorityOut_changed flag ***/
      initPriorityOut_changed = 1;
      
      /*** Reset Message Request Mask Unit structures ***/
      for (i = 0; i < RESOURCE_NUMBER_K; i++)
	msg_based_req_mask_state[i] = 0;
      for (i = 0; i < NBR_INITIATOR; i++)
	msg_based_req_mask[i] = 1;
      
      /*** reset rst flag ***/
      rst_flag = false;
    }
  
  
  
  /*** Check pointer function ***/
  void Assert_Pointers(void *p)
    {
      if(p == NULL)
	{
	  cout << "\n!!!!! ERROR At T=" << sc_simulation_time() <<"ns => Node\"" << name() << "\": Not available memory !!!!!\n" << endl;
	  exit(1);
	}
    }
  
  /************************************************************************
   *                                                                       *
   * Name:            SetPriorityVector()                                  *
   *                                                                       *
   * Comment:         It select the init pririty according to              *
   *                  the init priority flag                               *
   *                                                                       *
   * Returned value:  void                                                 *
   *                                                                       *
   *                                                                       *
   ************************************************************************/
  void SetPriorityVector()
    {
      int i;
      uint vpri;
      /*** reset flag ***/
      for(i=0; i<NBR_INITIATOR; i++)
	{
	  if( (initPriorityType[i] == REGISTER_PRI) || ((initPriorityType[i] == EXT_PRI)&&(In[i].req)) )
	    {
	      vpri = (initPriorityType[i] == REGISTER_PRI)? initPriorityReset[i] : (In[i].pri & 0x0f);
	      if(!(initPriorityOut_changed))
		{
		  if(initPriorityOut[i] != vpri)
		    {
		      initPriorityOut_changed = 1;
		      initPriorityOut[i] = vpri;
		    }
		}
	      else
		initPriorityOut[i] = vpri;
	    }
	}
    }
  
  /************************************************************************
   *                                                                       *
   * Name:            BandwidthLimiterAsync()                              *
   *                                                                       *
   * Comment:         It model the bandwidth limiter block (asynch block)  *
   *                                                                       *
   * Returned value:  void                                                 *
   *                                                                       *
   *                                                                       *
   ************************************************************************/
  //void BandwidthLimiterAsync(STbus *iInit_ICN, int nbr_initiator, uint *initPriorityOut, uchar *p_initPriorityOut_changed,
  //			   uint *bandwidthLimiterPriorityOutput, uchar *p_bandwidthPriorityOut_changed, uchar *initBandwidthLimiterOn,
  //			   uint *bandwidthWordsLimitCounter, uint *initWordsLimitReset, uint *initLowPriorityReset, const uint *bandwidthWordsLimitValues)
  void BandwidthLimiterAsync()
    {
      int i;
      /*** reset flag ***/
      bandwidthPriorityOut_changed = initPriorityOut_changed;
      for(i=0; i<NBR_INITIATOR; i++)
	{
	  /*    if( (!initBandwidthLimiterOn[i]) && (initPriorityOut_changed) )*/
	  if(initBandwidthLimiterOn[i])
	    {
	      if (initFrameSizeReset[i])
		{
		  if(bandwidthWordsLimitCounter[i] < bandwidthWordsLimitValues[initWordsLimitReset[i]])
		    {
		      if(bandwidthLimiterPriorityOutput[i] != initPriorityOut[i])
			{
			  bandwidthPriorityOut_changed = 1;
			  bandwidthLimiterPriorityOutput[i] = initPriorityOut[i];
			}
		    }
		  else
		    {
		      if(initBandwidthLimiterSwitchHighToLowStateDone[i])
			{
			  if(bandwidthLimiterPriorityOutput[i] != initLowPriorityReset[i])
			    {
			      bandwidthLimiterPriorityOutput[i] = initLowPriorityReset[i];
			      bandwidthPriorityOut_changed = 1;
			    }
			}
		      else
			initBandwidthLimiterSwitchHighToLowStateDone[i] = 1;
		    }
		}
	      else
		{
		  if(bandwidthLimiterPriorityOutput[i] != initPriorityOut[i])
		    {
		      bandwidthPriorityOut_changed = 1;
		      bandwidthLimiterPriorityOutput[i] = initPriorityOut[i];
		    }
		}
	    }
	}
      /*** reset init priority changed flag ***/
      //initPriorityOut_changed = 0;
    }
  
  /************************************************************************
   *                                                                       *
   * Name:            BandwidthLimiterSync()                               *
   *                                                                       *
   * Comment:         It model the bandwidth limiter block (synch block)   *
   *                                                                       *
   * Returned value:  void                                                 *
   *                                                                       *
   *                                                                       *
   ************************************************************************/
  //void BandwidthLimiterSync(STbus *iInit_ICN, int nbr_initiator, uchar *initBandwidthLimiterOn, uint *bandwidthFrameSizeCounter, uint *bandwidthWordsLimitCounter,
  //			  uint *initFrameSizeReset, const uint *bandwidthFrameSizeValues, uint stbus_size)
  void BandwidthLimiterSync()
    {
      int i;
      for(i=0; i<NBR_INITIATOR; i++)
	{
	  if((initBandwidthLimiterOn[i])&&(initFrameSizeReset[i]))
	    {
	      if(bandwidthFrameSizeCounter[i] == 0)
		{
		  bandwidthFrameSizeCounter[i] = bandwidthFrameSizeValues[initFrameSizeReset[i]]-1;
		  bandwidthWordsLimitCounter[i] = 0;
		  initBandwidthLimiterMovingWindowState[i] = 0;
		  initBandwidthLimiterSwitchLowToHighState[i] = 1;
		  initBandwidthLimiterSwitchHighToLowStateDone[i] = 0;
		}
	      else
		initBandwidthLimiterSwitchLowToHighState[i] = 0;
	      
	      //Added by Nizar
	      //bandwidthFrameSizeCounter[i]--;
	      if(((!initBandwidthLimiterMovingWindowOn[i])||((initBandwidthLimiterMovingWindowOn[i])&&(initBandwidthLimiterMovingWindowState[i])))&&(!initBandwidthLimiterSwitchLowToHighState[i]))
		bandwidthFrameSizeCounter[i]--;
	      
	      if((In[i].req && In[i].gnt)&&(!initBandwidthLimiterSwitchLowToHighState[i]))
		{
		  //Added by Nizar
		  if((initBandwidthLimiterMovingWindowOn[i])&&(!initBandwidthLimiterMovingWindowState[i]))
		    {
			bandwidthFrameSizeCounter[i]--;
			initBandwidthLimiterMovingWindowState[i] = 1;
		    }
		  uint nbytes_sent= 1<<((In[i].opc&0xf0)>>4), stbus_size = STBUS_SIZE>>3;
		  uchar opc = In[i].opc & 0x0f;
		  if((stbusType==3)&&((opc==LOAD_M)||((opc==RMW_M)&&(In[i].lck))))
		  	bandwidthWordsLimitCounter[i] += nbytes_sent;
		  else
		  	bandwidthWordsLimitCounter[i] += (nbytes_sent<=stbus_size) ? nbytes_sent : stbus_size;
		}
	    }
	}
    }
  
  /*********************************************************************
   *                                                                    *
   * Name:            ProcessRequest()                                  *
   *                                                                    *
   * Comment:         It collects transactions info                     *
   *                  at the beginning of a packet;                     *
   *                  set the priority table to the                     *
   *                  current bus values if the                         *
   *                  corresponding resource is                         *
   *                  VARIABLE_PRIORITY programmed;                     *
   *                                                                    *
   *                                                                    *
   *                                                                    *
   **********************************************************************/
  void SetArbitrationVector()
    {
      /*** process pending request ***/
      for(int res=0; res<RESOURCE_NUMBER_K; res++)
	/*** if resource i is programmed as variable priority ***/
	if( (resourceArbitration[res] == PRIORITY) || (resourceArbitration[res] == BANDWITH) || (resourceArbitration[res] == MSG_BASED) ||
	    (resourceArbitration[res] == STB) )
	  for(int initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	    {
	      if(!initBandwidthLimiterOn[initiator_index])
		{
		  if(initPriorityOut_changed)
		    *(arbitration_unit_priority + res*NBR_INITIATOR + initiator_index) = initPriorityOut[initiator_index];
		}
	      else
		{
		  if(bandwidthPriorityOut_changed)
		    *(arbitration_unit_priority + res*NBR_INITIATOR + initiator_index) = bandwidthLimiterPriorityOutput[initiator_index];
		}
	      /*** STB pri mask handling ***/
	      if( (resourceArbitration[res] == STB) && (init_latency_counter_en_bit[initiator_index] == 1) )
		{
		  if(init_latency_counter_pri_mask[initiator_index] != 0 )
		    /*** set bit 5 pri ***/
		    *(arbitration_unit_priority + res*NBR_INITIATOR + initiator_index) |= init_latency_counter_pri_mask[initiator_index];
		  else
		    /*** reset bit 5 pri ***/
		    *(arbitration_unit_priority + res*NBR_INITIATOR + initiator_index) &= 0xdf;
		}
	    }
      /*** reset init priority changed flag ***/
      initPriorityOut_changed = 0;
    }
  
  
  
  /********************************************************************
   *                                                                   *
   * Name:            Address_Decoding()                               *
   *                                                                   *
   * Comment:         It decodes the requested address                 *
   *                  into a target index, according to                *
   *                  the target memory map regions                    *
   *                                                                   *
   **                                                                   *
   ********************************************************************/
  //int Address_Decoding(uint add, targ_mem_region *targMemMapRegions, int * targMemMapRegionsNb, int NBR_TARGET, int max_nbr_regions)
  int Address_Decoding(TADDRESS add)
    {
      int i, j;
      for(i=0; i<NBR_TARGET; i++)
	//for(j=0; j<max_nbr_regions; j++)
	for(j=0; j<targMemMapRegionsNb[i]; j++)
	  {
	    if((add >= ((targMemMapRegions + i*max_nbr_regions + j)->targ_mem_map_min))&&(add <= ((targMemMapRegions + i*max_nbr_regions + j)->targ_mem_map_max)))
	      return i;
	  }
      return -1;
    }
  
  
/************************************************************************
*                                                                       *
* Name:            Address_Decoder_FSM()                                *
*                                                                       *
* Comment:         It models the Address Decoder functionality          *
*                                                                       *
* Returned value:  void                                                 *
*                                                                       *
*                                                                       *
************************************************************************/
/*void Address_Decoder(STbus *In, uchar *address_decoder_state, int *hot_vector_req, targ_mem_region *targMemMapRegions,
			 int * targMemMapRegionsNb, int NBR_TARGET, int max_nbr_regions, uchar *arbitration_unit_priority, uchar *resourceArbitration,
			 int RESOURCE_NUMBER_K, int NBR_INITIATOR, const char *node_name)*/

void Address_Decoder()
{
    for(int initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(address_decoder_state[initiator_index] == 0)
	{
	    if(In[initiator_index].req == 1)
	    {
		hot_vector_req[initiator_index] = Address_Decoding(In[initiator_index].add);
		if(hot_vector_req[initiator_index] == -1)
		    cout << "\n!!!!! WARNING At T=" << sc_time_stamp() <<"ns => Node\"" << name() << "\": address request(0x" << In[initiator_index].add << ") on port " << initiator_index <<" doesn't match any target address range !!!!!\n" << endl;
		//		if(bandwidthPriorityOut_changed)
		//		  /*** process pending request ***/
		//		  ProcessRequest(initiator_index);
		address_decoder_state[initiator_index] = 1;
	    }
	    else
		hot_vector_req[initiator_index] = -1;
	}
}


/*void Address_Decoder_GNT_FeedBack(int NBR_INITIATOR, STbus *In, uchar *address_decoder_state, int postedValidOn, fifo *posting_fifo_struct,
				  tracker_cell **posting_array, uchar *posting_fifo_state, int stbusType, int STBUS_SIZE) */
void Address_Decoder_GNT_FeedBack()
{
    for(int initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(address_decoder_state[initiator_index] == 1)
	{
	    if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1))
	    {
		/*** if write posting is enabled ***/
		if(postedValidOn && (!postfifo_full(initiator_index)) &&
		   (((stbusType == 3)&&(In[initiator_index].eop == 1)&&((In[initiator_index].tid &0x20) != 0)) ||
		    ((stbusType == 2)&&(posting_fifo_state[initiator_index] == 0))))
		{
		    tracker_cell *pcell = postfifo_put(initiator_index);
		    pcell->src = In[initiator_index].src;
		    pcell->tid = In[initiator_index].tid;
		    pcell->pri = In[initiator_index].pri;
		    pcell->lck = In[initiator_index].lck;
		    posting_fifo_state[initiator_index] = 1;
		    if(stbusType == 2)
		    {
			pcell->num_cell = (1 << ((In[initiator_index].opc & 0x70) >> 4))/(STBUS_SIZE/8);
			if(pcell->num_cell == 0)
			    pcell->num_cell = 1;
//			printf("posted dummy response: num cell %d\n", (pcell->num_cell));
		    }
		}
		if(In[initiator_index].eop == 1)
		{
		    address_decoder_state[initiator_index] = 0;
		    posting_fifo_state[initiator_index] = 0;
		}
	    }
	}
}

/*************************************************************************
*                                                                        *
* Name:            Ordering_Block_FSM()                                  *
*                                                                        *
* Comment:         It models the Ordering Block functionality            *
*                                                                        *
* Returned value:  void                                                  *
*                                                                        *
*                                                                        *
**************************************************************************/
//void Ordering_Block(int NBR_INITIATOR, STbus *In, uchar *ord_blk_detect_init_req, uint *ord_blk_counter,
//			uint *ord_blk_filter_depth, int *hot_vector_req, int *ord_blk_reg, int *ord_blk_output)
void Ordering_Block()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(ord_blk_filter_depth[initiator_index] > 0)
	{
	    if(ord_blk_detect_init_req[initiator_index] == 0)
	    {
		/*** reset output req ***/
		ord_blk_output[initiator_index] = -1;
		if((In[initiator_index].req == 1)&&(ord_blk_counter[initiator_index] < ord_blk_filter_depth[initiator_index])&&
		   (hot_vector_req[initiator_index] != -1))
		{
		    /*** if counter == 0 or input req == output req ***/
		    if((ord_blk_counter[initiator_index] == 0)||(hot_vector_req[initiator_index] == ord_blk_reg[initiator_index])||(ord_blk_filter_on[initiator_index] == false))
			{
			    if(ord_blk_counter[initiator_index] == 0)
				/*** copy req on the output ***/
				ord_blk_reg[initiator_index] = hot_vector_req[initiator_index];
			    /*** copy req on the output ***/
			    ord_blk_output[initiator_index] = hot_vector_req[initiator_index];
			    /*** incr filter counter ***/
			    ord_blk_counter[initiator_index]++;
			    /*** change init packet detector state ***/
			    ord_blk_detect_init_req[initiator_index] = 1;
			}
		}
	    }
	}
	else
	    ord_blk_output[initiator_index] = hot_vector_req[initiator_index];
}

//void Ordering_Block_GNT_FeedBack(int NBR_INITIATOR, uint *ord_blk_filter_depth, STbus *In, uchar *ord_blk_detect_init_req)
void Ordering_Block_GNT_FeedBack()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(ord_blk_filter_depth[initiator_index] > 0)
	    /*** if end of request packet ***/
	    if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1)&&(In[initiator_index].eop == 1)&&
	       (ord_blk_detect_init_req[initiator_index] == 1))
		/*** return to "detect init packet" state ***/
		ord_blk_detect_init_req[initiator_index] = 0;
}

//void Ordering_Block_R_GNT_FeedBack(int NBR_INITIATOR, uint *ord_blk_filter_depth, STbus *In, uint *ord_blk_counter,int *ord_blk_reg)
void Ordering_Block_R_GNT_FeedBack()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(ord_blk_filter_depth[initiator_index] > 0)
	    /*** if end of response and counter > 0 ***/
	    if((In[initiator_index].r_req == 1)&&(In[initiator_index].r_gnt == 1)&&(In[initiator_index].r_eop == 1)&&(ord_blk_counter[initiator_index] > 0))
	    {
		/*** dec counter ***/
		ord_blk_counter[initiator_index]--;
		/*** if counter = 0 -> reset output ***/
		if(ord_blk_counter[initiator_index] == 0)
		    ord_blk_reg[initiator_index] = -1;
	    }
}


/****************************************************************************
*                                                                           *
* Name:            Target_Dependency_Stage()                                *
*                                                                           *
* Comment:         It models the Target Dependency Stage functionality      *
*                                                                           *
* Returned value:  void                                                     *
*                                                                           *
*                                                                           *
*****************************************************************************/
//int Target_Dependency_Stage(int target_index, STbus *Out, int *targResourceMapping, int *number_target_per_resource, fifo *tracker_fifo_struct,
//			    uchar *tracker_full_out)
int Target_Dependency_Stage(int target_index)
{
    int val;

    if(target_index != -1)
    {
      //	if(number_target_per_resource[(targResourceMapping[target_index])] == 1)
      //	    val =  1;
      //	else
      val = Out[target_index].gnt;
    }
    else
	return 0;

    if(tracker_fifo_struct[target_index].fifosize == 0)
	    return val;
    else
    {
      if(tracker_full_out[target_index] == 0)
	return val;
      else
	return 0;
    }
}

/**********************************************************************************
*                                                                                 *
* Name:            Real_Request_Generator()                                       *
*                                                                                 *
* Comment:         It models the Real Request Gen functionality                   *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Real_Request_Generator()
{
    int i;
    for(i=0; i<NBR_INITIATOR; i++)
    {
	if(Target_Dependency_Stage(p_req_aux2[i]) == 1)
	{
	   if((In[i].req == 1) && ((lock_management_output[p_req_aux2[i]] == -1) || (lock_management_output[p_req_aux2[i]] == i)))
	       real_req[i] = p_req_aux2[i];
	   else
	   {
	       real_req[i] = -1;
//	       printf("%d: lock_management_output = %d, p_req_aux2 = %d\n", i, lock_management_output[p_req_aux2[i]], p_req_aux2[i]);
	   }
	}
	else
	{
	    real_req[i] = -1;
//	    printf("Non funziona !\n");
	}
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Inc_LRU_Counters()                                             *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Inc_LRU_Counters(int initiator_index, int NBR_INITIATOR, uchar *p_resource_LRU_counters)
void Inc_LRU_Counters(int initiator_index, uchar *p_resource_LRU_counters)
{
    int i;
    for(i=0; i<NBR_INITIATOR; i++)
    {
	if((i != initiator_index)&&(p_resource_LRU_counters[i] < 31))
	    p_resource_LRU_counters[i] = p_resource_LRU_counters[i] + 1;
//	printf("pri[%d] = %d  ", i, p_resource_LRU_counters[i]);
    }
//    printf("\n\n");
}

/**********************************************************************************
*                                                                                 *
* Name:            LRU_Algorithm()                                                *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void LRU_Algorithm(int resource_index, int initiator_index, int NBR_INITIATOR, uchar *LRU_initiator_packet_state, STbus *In, int *vect_req,
//		   uchar *control_unit_LRU_counters)
void LRU_Algorithm(int resource_index, int initiator_index, int *vect_req)
{
    /*** NOTE: initiator_index == transaction_management_output[resource_index] -> internal grant = 1 & direct connection with resource ***/
    if(initiator_index != -1)
	if(vect_req[initiator_index] != -1)
	{
	    int base_index = resource_index*NBR_INITIATOR;
	    switch(LRU_initiator_packet_state[initiator_index])
	    {
	       case 0:
		   /*** reset initiator counter ***/
		   control_unit_LRU_counters[base_index + initiator_index] = 0;

		   Inc_LRU_Counters(initiator_index, (control_unit_LRU_counters + base_index));
		   if(In[initiator_index].eop == 0)
		       LRU_initiator_packet_state[initiator_index] = 1;
		   break;

	       case 1:
		   if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1)&&(In[initiator_index].eop == 1))
		       LRU_initiator_packet_state[initiator_index] = 0;
	    }
	}
}



/**********************************************************************************
*                                                                                 *
* Name:            Latency_Algorithm()                                            *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Latency_Algorithm(int resource_index, int *vect_req)
{
    int initiator_index, base_index = resource_index*NBR_INITIATOR;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	if(vect_req[initiator_index] != -1)
	    if(targResourceMapping[vect_req[initiator_index]] == resource_index)
	{
	    switch(Latency_initiator_packet_state[initiator_index])
	    {
	        case 0:
		    /*** set latency counter ***/
		    control_unit_Latency_counters[base_index + initiator_index] = initLatencyReset[initiator_index];
		    /*** if initiator not granted ***/
		    if(transaction_management_output[resource_index] != initiator_index)
		    {
			if(control_unit_Latency_counters[base_index + initiator_index] > 0)
			{
			    control_unit_Latency_counters[base_index + initiator_index] -= 1;
			    /*** set flag to 1 (count changed) ***/
			    Latency_resource_changed_counts[resource_index] = 1;
			}
			/*** STB pri mask setting ***/
			if( (resourceArbitration[resource_index] == STB) && (init_latency_counter_en_bit[initiator_index] == 1) &&
			    (control_unit_Latency_counters[base_index + initiator_index] == 0) )
			  init_latency_counter_pri_mask[initiator_index] = 0x20;
			Latency_initiator_packet_state[initiator_index] = 1;
		    }
		    break;

	        case 1:
			if(transaction_management_output[resource_index] != initiator_index)
			{
			    if(control_unit_Latency_counters[base_index + initiator_index] > 0)
				control_unit_Latency_counters[base_index + initiator_index] -= 1;
			    /*** set flag to 1 (count changed) ***/
			    Latency_resource_changed_counts[resource_index] = 1;
			    /*** STB pri mask setting ***/
			    if( (resourceArbitration[resource_index] == STB) && (init_latency_counter_en_bit[initiator_index] == 1) &&
				(control_unit_Latency_counters[base_index + initiator_index] == 0) )
			      init_latency_counter_pri_mask[initiator_index] = 0x20;
			}
			else
			{
			    /*** set latency counter ***/
			    control_unit_Latency_counters[base_index + initiator_index] = initLatencyReset[initiator_index];
			    Latency_initiator_packet_state[initiator_index] = 0;
			    /*** set flag to 1 (count changed) ***/
			    Latency_resource_changed_counts[resource_index] = 1;
			    Latency_initiator_packet_state[initiator_index] = 0;
			    /*** STB pri mask reset ***/
			    if( (resourceArbitration[resource_index] == STB) && (init_latency_counter_en_bit[initiator_index] == 1) )
			      init_latency_counter_pri_mask[initiator_index] = 0;
			}
		    break;
	    }
	}
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Latency_Get_Min()                                              *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
int Latency_Get_Min(int resource_index, int *vect_req)
{
    int initiator_index, base_index = resource_index*NBR_INITIATOR, i = -1, j, min_counter = 0;
    uint min_latency;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	latency_vect_priority_mask[initiator_index] = 0;
	if(vect_req[initiator_index] != -1)
	{
	    if(targResourceMapping[vect_req[initiator_index]] == resource_index)
	    {
		if((i == -1)||(control_unit_Latency_counters[base_index + initiator_index] < min_latency))
		{
		    if(i != -1)
			for(j=0; j<=i; j++)
			    latency_vect_priority_mask[j] = 0;
		    i = initiator_index;
		    min_latency = control_unit_Latency_counters[base_index + initiator_index];
		    min_counter = 1;
		    latency_vect_priority_mask[i] = 1;
		}
		else
		if(control_unit_Latency_counters[base_index + initiator_index] == min_latency)
		{
		    i = initiator_index;
		    latency_vect_priority_mask[initiator_index] = 1;
		    min_counter++;
		}
	    }
	}
    }
    return min_counter;
}

void quickswap(uint *A, uint *B)
{
    uint Temp;
    Temp = *A;
    *A = *B;
    *B = Temp;

}

void QuickSort(int lower, int upper)
{
    int m = lower, i;
    if(lower>=upper)
        return;
    quickswap(&Latency_initiator_sort_vect[lower], &Latency_initiator_sort_vect[(lower+upper)/2]);
    for(i=lower+1; i<=upper; i++)
    {
        if(Latency_initiator_value_vect[Latency_initiator_sort_vect[i]] <= Latency_initiator_value_vect[Latency_initiator_sort_vect[lower]])
            quickswap(&Latency_initiator_sort_vect[++m], &Latency_initiator_sort_vect[i]);
    }
    quickswap(&Latency_initiator_sort_vect[lower], &Latency_initiator_sort_vect[m]);
    QuickSort(lower, m-1);
    QuickSort(m+1, upper);
}


//void NormalizeLRUValues(int start_index, int end_index, int nbr_initiator, uint *Latency_initiator_sort_vect, uint *Latency_initiator_value_vect)
void NormalizeLRUValues(int start_index, int end_index)
{
    while(start_index <= end_index)
    {
	if(start_index == end_index)
	{
	    Latency_initiator_value_vect[Latency_initiator_sort_vect[start_index]] = NBR_INITIATOR - start_index - 1;
	    start_index++;
	}
	else
	{
	    if(Latency_initiator_value_vect[Latency_initiator_sort_vect[start_index+1]] != Latency_initiator_value_vect[Latency_initiator_sort_vect[start_index]])
	    {
		Latency_initiator_value_vect[Latency_initiator_sort_vect[start_index]] = NBR_INITIATOR - start_index - 1;
		start_index++;
	    }
	    else
	    {
		int j = start_index + 1;
		while((j < end_index)&&(Latency_initiator_value_vect[Latency_initiator_sort_vect[j+1]] == Latency_initiator_value_vect[Latency_initiator_sort_vect[j]]))
		    j++;
		for(;start_index<=j; start_index++)
		    Latency_initiator_value_vect[Latency_initiator_sort_vect[start_index]] = NBR_INITIATOR - j - 1;
	    }
	}
    }
}

//void LookForPrioritySubRanges(int start_index, int end_index, int nbr_initiator, uint *Latency_initiator_sort_vect, uint *Latency_initiator_value_vect,
//			      uchar *control_unit_LRU_counters)
void LookForPrioritySubRanges(int start_index, int end_index, uchar *resource_LRU_counters)
{
    int i=start_index, j, k;
    /*** look for equal subrange ***/
    while((i<(end_index-1))&&(Latency_initiator_value_vect[Latency_initiator_sort_vect[i+1]] != Latency_initiator_value_vect[Latency_initiator_sort_vect[i]]))
	i++;
    j = i;

    if(i<(end_index-1))
	for(k=start_index; k<i; k++)
	    Latency_initiator_value_vect[Latency_initiator_sort_vect[k]] = NBR_INITIATOR - k - 1;
    else
	for(k=start_index; k<end_index; k++)
	    Latency_initiator_value_vect[Latency_initiator_sort_vect[k]] = NBR_INITIATOR - k - 1;

	while((j<(end_index-1))&&(Latency_initiator_value_vect[Latency_initiator_sort_vect[j+1]] == Latency_initiator_value_vect[Latency_initiator_sort_vect[i]]))
	{
	    j++;
	}
	if(j>i)
	{
	    int l;
	    for(l=i; l<=j; l++)
		Latency_initiator_value_vect[Latency_initiator_sort_vect[l]] = MAX_INIT_LRU - resource_LRU_counters[Latency_initiator_sort_vect[l]];
	    /*** re-sort sub range ***/
	    QuickSort(i, j);
	    NormalizeLRUValues(i, j);
	}
}


//int LookForLatencySubRanges(int start_index, int NBR_INITIATOR, uint *Latency_initiator_sort_vect, uint *Latency_initiator_value_vect, uint *initPriorityReset,
//			    uchar *control_unit_LRU_counters)
int LookForLatencySubRanges(int start_index, uchar *resource_LRU_counters)
{
    int i=start_index, j, k;
    /*** look for equal subrange ***/
    while((i<(NBR_INITIATOR-1))&&(Latency_initiator_value_vect[Latency_initiator_sort_vect[i+1]] != Latency_initiator_value_vect[Latency_initiator_sort_vect[i]]))
	i++;
    j = i;

    if(i<(NBR_INITIATOR-1))
	for(k=start_index; k<i; k++)
	    Latency_initiator_value_vect[Latency_initiator_sort_vect[k]] = NBR_INITIATOR - k - 1;
    else
	for(k=start_index; k<NBR_INITIATOR; k++)
	    Latency_initiator_value_vect[Latency_initiator_sort_vect[k]] = NBR_INITIATOR - k - 1;

	while((j<(NBR_INITIATOR-1))&&(Latency_initiator_value_vect[Latency_initiator_sort_vect[j+1]] == Latency_initiator_value_vect[Latency_initiator_sort_vect[i]]))
	{
	    j++;
	}
	if(j>i)
	{
	    int l;
	    for(l=i; l<=j; l++)
		Latency_initiator_value_vect[Latency_initiator_sort_vect[l]] = MAX_INIT_PRI - initPriorityReset[Latency_initiator_sort_vect[l]];
	    /*** re-sort sub range ***/
//	    qsort((void *)(Latency_initiator_sort_vect + i), (j-i+1), sizeof (uint), Latency_compare);
	    QuickSort(i, j);
//            LookForPrioritySubRanges(i, j+1, NBR_INITIATOR, Latency_initiator_sort_vect, Latency_initiator_value_vect, resource_LRU_counters);
            LookForPrioritySubRanges(i, j+1, resource_LRU_counters);
	}
	j++;
    return j;
}


/**********************************************************************************
*                                                                                 *
* Name:            Latency_Sort_Counters()                                        *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void NewLatency_Get_Min(int resource_index, int nbr_initiator, uint *control_unit_Latency_counters, uint *Latency_initiator_sort_vect,
//			uint *Latency_initiator_value_vect, uchar *Latency_resource_changed_counts, uint *initPriorityReset, uchar *control_unit_LRU_counters)
void NewLatency_Get_Min(int resource_index, int base_index)
{
    if(Latency_resource_changed_counts[resource_index])
    {
	int initiator_index, i;

	/*** reset counter changed flag ***/
	Latency_resource_changed_counts[resource_index] = false;
	/*** load latency counters into the work vector ***/
	for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	    Latency_initiator_value_vect[initiator_index] = control_unit_Latency_counters[base_index + initiator_index];
//	qsort((void *)Latency_initiator_sort_vect, NBR_INITIATOR, sizeof (uint), Latency_compare);
	QuickSort(0, NBR_INITIATOR - 1);
	/*** search for subranges with equal latency values ***/
	i=0;
	do
//	    i=LookForLatencySubRanges(i, nbr_initiator, Latency_initiator_sort_vect, Latency_initiator_value_vect, initPriorityReset, (control_unit_LRU_counters + base_index));
	    i=LookForLatencySubRanges(i, (control_unit_LRU_counters + base_index));
	while(i < NBR_INITIATOR);
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Latency_Set_Pri()                                              *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Latency_Set_Pri(int resource_index, int NBR_INITIATOR, uint *initPriorityReset, uchar *arbitration_unit_priority, uchar *control_unit_LRU_counters,
//		     uchar *vect_mask, int latency_min_count)
void Latency_Set_Pri(int resource_index, int latency_min_count)
{
    int initiator_index, base_index = resource_index*NBR_INITIATOR, i = -1, j, max_pri_counter = 0;
    uint max_pri = 0;

    /*** only one min counter ***/
    if(latency_min_count == 0)
    {
	for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	    *(arbitration_unit_priority + base_index + initiator_index) = *(control_unit_LRU_counters + base_index + initiator_index);
	return;
    }
    else
    if(latency_min_count == 1)
    {
	for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	    *(arbitration_unit_priority + base_index + initiator_index) = (latency_vect_priority_mask[initiator_index] == 1)? 31 : *(control_unit_LRU_counters + base_index + initiator_index);
    }
    else
    {
	for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	{
	    if(latency_vect_priority_mask[initiator_index] == 1)
	    {
		if((i == -1)||(initPriorityReset[initiator_index]>max_pri))
		{
//		    if(i != -1)
//			for(j=0; j<=i; j++)
//			    latency_vect_priority_mask[j] = 0;
		    i = initiator_index;
//		    latency_vect_priority_mask[i] = 1;
		    max_pri_counter = 1;
		    max_pri = initPriorityReset[initiator_index];
//		    printf("set max_pri_counter to 1; max_pri= %d\n", max_pri);
		}
		else
		if(initPriorityReset[initiator_index] == max_pri)
		{
		    i = initiator_index;
		    max_pri_counter++;
//		    printf("max_pri_counter++; d\n", max_pri_counter);
		}
//		else
//		if(initPriorityReset[initiator_index] < max_pri)
//		    latency_vect_priority_mask[initiator_index] = 0;
	    }
	}
//	printf("max_pri_counter : %d\n", max_pri_counter);
	if(max_pri_counter == 1)
	    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	    {
	      *(arbitration_unit_priority + base_index + initiator_index) = (latency_vect_priority_mask[initiator_index] == 1) ? initPriorityReset[initiator_index]
		: (*(control_unit_LRU_counters + base_index + initiator_index));
//	      printf("pri[%d] = %d\n", (*(arbitration_unit_priority + base_index + initiator_index)));
	    }
	else
	    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
		*(arbitration_unit_priority + base_index + initiator_index) = (latency_vect_priority_mask[initiator_index] == 1) ?
		    (*(control_unit_LRU_counters + base_index + initiator_index)) : 0;
//		*(arbitration_unit_priority + base_index + initiator_index) = (*(control_unit_LRU_counters + base_index + initiator_index));
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            End_Of_Time_Period()                                           *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  res (1 = all counters equal to zero )                          *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//uchar End_Of_Time_Period(uint *bandwidth_counters, int NBR_INITIATOR)
uchar End_Of_Time_Period(uint *bandwidth_counters)
{
    int i;
    for(i=0; i<NBR_INITIATOR; i++)
	if(bandwidth_counters[i] != 0)
	    return 0;
    return 1;
}


/**********************************************************************************
*                                                                                 *
* Name:            Set_Bandwidth_Structures()                                     *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Set_Bandwidth_Structures(uint *bandwidth_counters, uchar *bandwidth_mask)
{
    int i;
    for(i=0; i<NBR_INITIATOR; i++)
    {
	bandwidth_counters[i] = initBandwidthReset[i];
	bandwidth_mask[i] = 1;
    }
}

/**********************************************************************************
*                                                                                 *
* Name:            Bandwidth_Algorithm()                                          *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Bandwidth_Algorithm(int resource_index, int *vect_req)
{
    int initiator_index, base_index = resource_index*NBR_INITIATOR;

    /*** check for end of time period ***/
    if(End_Of_Time_Period(&control_unit_Bandwidth_counters[base_index]) == 1)
    {
	Set_Bandwidth_Structures(&control_unit_Bandwidth_counters[base_index], &bandwidth_vect_req_mask[base_index]);
	return;
    }

    /*** look for enabled requests ***/
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
//	   (bandwidth_vect_req_mask[base_index + initiator_index] == 1))
	if(vect_req[initiator_index] != -1)
	{
	    if(targResourceMapping[vect_req[initiator_index]] == resource_index)
	    {
		if(transaction_management_output[resource_index] == initiator_index)
		    //	    if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1))
		{
		    if(control_unit_Bandwidth_counters[base_index + initiator_index] != 0)
		    {
			//		    printf("dec counter from req block\n");
			control_unit_Bandwidth_counters[base_index + initiator_index] -= 1;
			if((control_unit_Bandwidth_counters[base_index + initiator_index] == 0)&&((In[initiator_index].tid & 0x10) == 0))
			{
			    bandwidth_vect_req_mask[base_index + initiator_index] = 0;
			    //			printf("reset mask from request block\n");
			}
		    }
		    return;
		}
	    }
	}
    }

//    if(transaction_management_output[resource_index] == -1)
    {
      /*** look for not-zero time slot ***/
      for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if(bandwidth_vect_req_mask[base_index + initiator_index] == 1)
	{
	    if(control_unit_Bandwidth_counters[base_index + initiator_index] != 0)
	    {
		control_unit_Bandwidth_counters[base_index + initiator_index] -= 1;
//		printf("dec counter from slot block\n");
		if(control_unit_Bandwidth_counters[base_index + initiator_index] == 0)
		{
		    bandwidth_vect_req_mask[base_index + initiator_index] = 0;
//		    printf("reset mask from slot block\n");
		}
		return;
	    }
	}
    }
}

/**********************************************************************************
*                                                                                 *
* Name:            Message_Request_Mask_Out()                                     *
*                                                                                 *
* Comment:         It models the  functionality of the req mask generator         *
*                  for the Message Based algo                                     *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Message_Request_Mask_Out()
{
  int resource_index, initiator_index, base_index, current_init_winner;
  uchar curr_msg_pri, init_index_pri;
  for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    if( resourceArbitration[resource_index] == MSG_BASED )
      {
	switch( msg_based_req_mask_state[resource_index] )
	  {
	  case 0:
	    for ( initiator_index = 0; initiator_index < NBR_INITIATOR; initiator_index++ )
	      if( real_req[initiator_index] != -1)
		if( targResourceMapping[real_req[initiator_index]] == resource_index )
		  msg_based_req_mask[initiator_index] = 1;
	    break;
	    
	  case 1:
	    for ( initiator_index = 0; initiator_index < NBR_INITIATOR; initiator_index++ )
	      if( real_req[initiator_index] != -1)
		{
		  //if(transaction_management_output[resource_index]!=-1)
		    current_init_winner = transaction_management_output[resource_index];

		  if( current_init_winner != -1 )
		    {
		      if( current_init_winner != initiator_index )
			{
			  if( targResourceMapping[real_req[initiator_index]] == resource_index )
			    {
			      base_index = resource_index*NBR_INITIATOR;
			      curr_msg_pri = *(arbitration_unit_priority + base_index + current_init_winner);
			      init_index_pri = *(arbitration_unit_priority + base_index + initiator_index);
			      if( (init_index_pri & 0x08) && (init_index_pri > curr_msg_pri) )
				{
				  if(init_interrupt_rule_reset[initiator_index] == 0)
				    {
				      msg_based_req_mask[initiator_index] = 1;
				      msg_to_be_interrupted[resource_index] = 1;
				    }
				  else
				    {
				      uchar opc = In[initiator_index].opc & 0x0f;
				      if((opc==LOAD_M)||(opc==LOADG_M))
					{					  
					  msg_based_req_mask[initiator_index] = 1;
					  msg_to_be_interrupted[resource_index] = 1;					  
					}
				      else
					msg_based_req_mask[initiator_index] = 0;
				    }
				}
			      else
				msg_based_req_mask[initiator_index] = 0;
			    }
			}
		      else
			msg_based_req_mask[initiator_index] = 1;
		    }
		}
	    break;
	  }
      }
}

/**********************************************************************************
*                                                                                 *
* Name:            Message_Request_Mask_FSM()                                     *
*                                                                                 *
* Comment:         It models the  functionality of the Message Based algo FSM     *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Message_Request_Mask_FSM(int resource_index)
{
  int initiator_index = transaction_management_output[resource_index];
  switch(msg_based_req_mask_state[resource_index])
    {
    case 0:
      {
	if(initiator_index != -1)
	  if( ( real_req[initiator_index] != -1) && In[initiator_index].gnt && In[initiator_index].eop && ( (In[initiator_index].tid >> 4 ) & 1 ) )
	    {
	      msg_based_req_mask_state[resource_index] = 1;
	      msg_based_current_init[resource_index] = initiator_index;
	      /*** load timeout counter ***/
	      if(message_timeout_enabled == 1)
		{
		  if(message_timeout == 0)
		    message_counter[resource_index] = message_timeout;
		  else
		    message_counter[resource_index] = message_timeout + 1;
		}
	    }
      }
      break;
      
    case 1:
      if(initiator_index == -1)
	initiator_index = msg_based_current_init[resource_index];
      else
	msg_based_current_init[resource_index] = initiator_index;
      
      /*** if the message restart ***/
      if(real_req[initiator_index] != -1)
	{
	  if( In[initiator_index].gnt && In[initiator_index].eop )
	    {
	      /*** if end of message (end of last chunk of a message) ***/
	      if( ( ( (In[initiator_index].tid >> 4) & 1) == 0) && (In[initiator_index].lck == 0) )
		msg_based_req_mask_state[resource_index] = 0;
	      else
		/*** if still in a message ***/
		if(message_timeout_enabled == 1)
		  {
		    if(message_timeout == 0)
		      message_counter[resource_index] = message_timeout;
		    else
		      message_counter[resource_index] = message_timeout + 1;
		  }
	    }
	}
      else
	{
	  if(message_timeout_enabled == 1)
	    {
	      if(message_timeout != 0)
		message_counter[resource_index]--;
	      
	      if(message_counter[resource_index] <= 0)
		/*** return to the idle state ***/
		msg_based_req_mask_state[resource_index] = 0;
	    }
	}
      break;
    }
}



/**********************************************************************************
*                                                                                 *
* Name:            Control_Unit_FSM()                                             *
*                                                                                 *
* Comment:         It models the Control Unit functionality after the GNT gen     *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Control_Unit_FSM(int *vect_req)
{
  int resource_index, i, base_index;
  for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	base_index = resource_index*NBR_INITIATOR;
	switch(resourceArbitration[resource_index])
	  {
	    /*** STATIC ***/
	  case STATIC:
	    break;
	    /*** PRIORITY ***/
	  case PRIORITY:
	    break;
	    /*** LRU ***/
	  case LRU:
	    LRU_Algorithm(resource_index, transaction_management_output[resource_index], vect_req);
	    for(i=0; i<NBR_INITIATOR; i++)
	      *(arbitration_unit_priority + base_index + i) = *(control_unit_LRU_counters + base_index + i);
	    break;
	    
	    /*** LATENCY ***/
	  case LATENCY:
	    /* set priorities */
	    for(i=0; i<NBR_INITIATOR; i++)
	      {
		*(arbitration_unit_priority + base_index + i) = *(retimed_Latency_arbitration_unit_priority + base_index + i);
	      }
	    /* evaluate min counters */
            NewLatency_Get_Min(resource_index, base_index);
	    
            /*** evaluate new priorities ***/
	    for(i=0; i<NBR_INITIATOR; i++)
	      {
		*(retimed_Latency_arbitration_unit_priority + base_index + i) = Latency_initiator_value_vect[i];
	      }
	    
	    /* apply latency algo */
	    Latency_Algorithm(resource_index, vect_req);
	    /* set LRU counters for algo cascading */
	    LRU_Algorithm(resource_index, transaction_management_output[resource_index], vect_req);
	    break;
	    
	    /*** BANDWITH ***/
	  case BANDWITH:
	    Bandwidth_Algorithm(resource_index, vect_req);
	    break;
	    
	  case STB:
	    /* apply latency algo + STB mask handling (resourceArbitration[resource_index] == STB) */
	    Latency_Algorithm(resource_index, vect_req);
	    break;
	    
	    /*** MSG_BASED ***/
	  case MSG_BASED:
	    Message_Request_Mask_FSM(resource_index);
            break;
	  }
    }
}



/******************************************************************************************************
*                                                                                                     *
* Name:            Arbitration_Unit()                                                                 *
*                                                                                                     *
* Comment:         It models the  Arbitration Unit functionality                                      *
*                                                                                                     *
* Returned value:  void                                                                               *
*                                                                                                     *
*                                                                                                     *
*******************************************************************************************************/
//void Arbitration_Unit(int RESOURCE_NUMBER_K, int *real_req, uchar *bandwidth_vect_req_mask, uchar *arbitration_unit_priority, int *targResourceMapping,
//		      int *arbitration_unit_result, int NBR_INITIATOR)
void Arbitration_Unit()
{
  int resource_index;
  for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
      int i, base_index = resource_index*NBR_INITIATOR;
      int winner_initiator_priority = -1;
      //Added By Nizar
      //End Of Add
      if(arbitration_unit_interrupt_load8_state[resource_index] == 0)
	{
	  arbitration_unit_result[resource_index] = -1;
	  for (i = 0; i < NBR_INITIATOR; i++)
	    if (real_req[i] != -1)
	      if((targResourceMapping[real_req[i]] == resource_index) && (bandwidth_vect_req_mask[base_index + i] == 1)
		 && (*(arbitration_unit_priority + base_index + i) > winner_initiator_priority) && (msg_based_req_mask[i] ==1))
		{
		  arbitration_unit_result[resource_index] = i;
		  winner_initiator_priority = *(arbitration_unit_priority + resource_index*NBR_INITIATOR + i);
		}

	  if((resourceLd8Interrupt[resource_index] == 1)&&(STBUS_SIZE!=128)&&(STBUS_SIZE!=256))
	    if(arbitration_unit_result[resource_index] != -1)
	      if(((In[arbitration_unit_result[resource_index]].opc&0xFF)==0x31)&&(((In[arbitration_unit_result[resource_index]].add>>3)&1)==0))
		arbitration_unit_interrupt_load8_state[resource_index] = 1;
	}
      else
	arbitration_unit_interrupt_load8_state[resource_index] = 0;
    }
}


/******************************************************************************
*                                                                             *
* Name:            ResetPorts()                                               *
*                                                                             *
* Comment:                                                                    *
*                                                                             *
* Returned value:  void                                                       *
*                                                                             *
*******************************************************************************/
void ResetPorts()
{
    int target_index, initiator_index;
#ifndef NOSNOOP
    int index;
#endif

    /*** init slave ports ***/
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	In[initiator_index].gnt = false;
	In[initiator_index].r_req = false;
	In[initiator_index].r_eop = false;
	In[initiator_index].r_lck = false;
    }

    /*** init master ports ***/
    for(target_index=0; target_index<NBR_TARGET; target_index++)
    {
	Out[target_index].r_gnt = false;
	Out[target_index].eop = false;
	Out[target_index].req = false;
	Out[target_index].lck = false;
    }
#ifndef NOSNOOP
    if (SnoopingActive) {
      for(index=0; index<RESOURCE_NUMBER_K; index++)
      {
        SnoopPort_d[index].gnt = false;
        SnoopPort_d[index].eop = false;
        SnoopPort_d[index].req = false;
        SnoopPort_d[index].lck = false;
      }
      for(index=0; index<RETURN_RESOURCE_NUMBER_K; index++)
      {
        SnoopPort_r[index].r_req = false;
        SnoopPort_r[index].r_eop = false;
        SnoopPort_r[index].r_lck = false;
        SnoopPort_r[index].r_gnt = false;
      }
    }
#endif
}


/******************************************************************************
*                                                                             *
* Name:            Control_Signal_Generator()                                 *
*                                                                             *
* Comment:         It models the Control_Signal_Generator  Functionality      *
*                                                                             *
* Returned value:  void                                                       *
*                                                                             *
*******************************************************************************/
//void Control_Signal_Generator(int RESOURCE_NUMBER_K, int *p_req_aux3, int *real_req, STbus *In, STbus *Out)
void Control_Signal_Generator()
{
    int resource_index;

    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	int initiator_index = p_req_aux3[resource_index];
	if(initiator_index != -1)
	{
	    if((In[initiator_index].req == 1)&&(real_req[initiator_index] != -1))
	    {
		In[initiator_index].gnt = Out[real_req[initiator_index]].gnt;
		Out[real_req[initiator_index]].req = 1;
#ifndef NOSNOOP
                SnoopPort_d[resource_index].gnt = Out[real_req[initiator_index]].gnt;
                SnoopPort_d[resource_index].req = 1;
#endif
	    }
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Transaction_Management_Out()                               *
*                                                                             *
* Comment:         It controls the Transaction Management Output Propagation  *
*                                                                             *
* Returned value:  void                                                       *
*                                                                             *
*                                                                             *
*******************************************************************************/
void Transaction_Management_Out()
{
    int resource_index;
    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
	if(transaction_management_state[resource_index] == 0)
	    /*** propagate arbitration vector to the transaction management output ***/
	    transaction_management_output[resource_index] = arbitration_unit_result[resource_index];
}


/******************************************************************************
*                                                                             *
* Name:            Transaction_Management_FSM()                               *
*                                                                             *
* Comment:         It models the Transaction Management  Functionality        *
*                                                                             *
* Returned value:  void                                                       *
*                                                                             *
*                                                                             *
*******************************************************************************/
void Transaction_Management_FSM()
{
    int resource_index, initiator_index;
    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	initiator_index = transaction_management_output[resource_index];
	if(initiator_index != -1)
	{
	    switch(transaction_management_state[resource_index])
	    {
	    case 0:
		if (In[initiator_index].req == 1)
		{
		  if((In[initiator_index].gnt == 0) || (In[initiator_index].eop == 0)|| (((In[initiator_index].tid >> 4) & 1)==1))
		    //if((In[initiator_index].gnt == 0) || (In[initiator_index].eop == 0))
		    transaction_management_state[resource_index] = 1;
		  msg_to_be_interrupted[resource_index] = 0;
		}
		break;

	    case 1:
	      /*** if end of current message/packet ***/
	      if ((In[initiator_index].req == 1) && (In[initiator_index].gnt == 1) && (In[initiator_index].eop == 1) && ((((In[initiator_index].tid >> 4) & 1)==0)||(msg_to_be_interrupted[resource_index])))
		//if ((In[initiator_index].req == 1) && (In[initiator_index].gnt == 1) && (In[initiator_index].eop == 1))
		transaction_management_state[resource_index] = 0;
	      break;
	    }
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Direct_Datapath()                                          *
*                                                                             *
* Comment:         It models the Direct Datapath Functionality                *
*                                                                             *
* Returned value:  void                                                       *
*                                                                             *
*                                                                             *
*******************************************************************************/
//void Direct_Datapath(int RESOURCE_NUMBER_K, STbus *In, STbus *Out, int *p_req_aux3, int *p_req_aux2, fifo *tracker_fifo_struct,
//		     uchar stbusType, uint *init_src_min, uchar *init_src_mask)
void Direct_Datapath()
{
    int resource_index, initiator_index;
    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	initiator_index = p_req_aux3[resource_index];
	if(initiator_index != -1)
	{
	    if(In[initiator_index].req == 1)
	    {
		/*** datapath select and signal copy ***/
		int target_index = p_req_aux2[initiator_index];
//		printf("\n############## initiator index %d  target index %d src %d tid %x opc %x bus copy #############\n\n", initiator_index, target_index,
//		       In[initiator_index].src, In[initiator_index].tid, In[initiator_index].opc);
		if(target_index != -1)
		{
		    Out[target_index].add = In[initiator_index].add;
		    Out[target_index].be = In[initiator_index].be;
		    Out[target_index].opc = In[initiator_index].opc;
		    Out[target_index].eop = In[initiator_index].eop;

		    // Added after request from Walid NAIFER
		    //Out[target_index].src = In[initiator_index].src;

		    if(tracker_fifo_struct[target_index].fifosize == 0)
			Out[target_index].src = In[initiator_index].src | ((init_src_mask[initiator_index] == 1) ? init_src_min[initiator_index] : 0);
		    Out[target_index].tid = In[initiator_index].tid;
		    Out[target_index].pri = In[initiator_index].pri;
		    Out[target_index].lck = In[initiator_index].lck;
		    Out[target_index].data = In[initiator_index].data;
#ifndef NOSNOOP
                      /* Setting values for the snoop port */
                    if (SnoopingActive)
                    {
                        SnoopPort_d[resource_index].add = In[initiator_index].add;
                        SnoopPort_d[resource_index].be = In[initiator_index].be;
                        SnoopPort_d[resource_index].opc = In[initiator_index].opc;
                        SnoopPort_d[resource_index].eop = In[initiator_index].eop;
                        if(tracker_fifo_struct[target_index].fifosize == 0)
                            SnoopPort_d[resource_index].src = In[initiator_index].src | ((init_src_mask[initiator_index] == 1) ? init_src_min[initiator_index] : 0);
                        SnoopPort_d[resource_index].tid = In[initiator_index].tid;
                        SnoopPort_d[resource_index].pri = In[initiator_index].pri;
                        SnoopPort_d[resource_index].lck = In[initiator_index].lck;
                        SnoopPort_d[resource_index].data = In[initiator_index].data;
                    }
#endif

		}
	    }
	}
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Lock_Management_Block()                                        *
*                                                                                 *
* Comment:         It models the Lock_Management_Block functionality              *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Lock_Management_Block(STbus *In, int *p_req_aux2, uchar *lock_management_state, int *lock_management_output,
//			   int NBR_INITIATOR)
void Lock_Management_Block()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	if(p_req_aux2[initiator_index] != -1)
	    switch(lock_management_state[initiator_index])
	    {
	       case 0:
		   if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1)&&(In[initiator_index].lck == 1))
		   {
		       /*** change state to packet already started ***/
		       lock_management_state[initiator_index] = 1;
		       /*** set the filter lock_management_output to lock the current resource for the current master initiator ***/
		       lock_management_output[p_req_aux2[initiator_index]] = initiator_index;
		   }
		   break;

	       case 1:
		   if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1)&&(In[initiator_index].lck == 0)&&(In[initiator_index].eop == 1))
		   {
		       /*** change state to end of packet ***/
		       lock_management_state[initiator_index] = 0;
		       /*** reset the filter lock_management_output to enable all the request to this resource ***/
		       lock_management_output[p_req_aux2[initiator_index]] = -1;
		   }
		   break;
	    }
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Tracker_FSM()                                                  *
*                                                                                 *
* Comment:         It models the Tracker FSM functionality                        *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Tracker_FSM(int *transaction_management_output, int *p_req_aux2, int RESOURCE_NUMBER_K, uchar *tracker_state, STbus *Out, STbus *In,
//		 tracker_cell **tracker_array, fifo *tracker_fifo_struct, uchar *tracker_full_out, uint *init_src_min, uchar *init_src_mask)
void Tracker_FSM()
{
    int target_index, initiator_index, resource_index;

    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	initiator_index = transaction_management_output[resource_index];
	if(initiator_index != -1)
	{
	    target_index = p_req_aux2[initiator_index];
	    if((target_index != -1)&&(tracker_fifo_struct[target_index].fifosize != 0))
		switch(tracker_state[target_index])
		{
		   case 0:
		       if((Out[target_index].req)&&(Out[target_index].gnt))
		       {
			   tracker_cell *pcell = tracker_put(target_index);
			   pcell->src = In[initiator_index].src | ((init_src_mask[initiator_index] == 1) ? init_src_min[initiator_index] : 0);
			   pcell->tid = In[initiator_index].tid;
			   pcell->pri = In[initiator_index].pri;

			   if(!Out[target_index].eop.read())
			   {
			       tracker_state[target_index] = 1;
			   }
			   else
			   if(tracker_full(target_index))
			   {
			       tracker_full_out[target_index] = 1;
			   }
		       }
		       break;

		   case 1:
		       if((Out[target_index].req == 1)&&(Out[target_index].gnt == 1)&&(Out[target_index].eop == 1))
		       {
			   tracker_state[target_index] = 0;
			   if(tracker_full(target_index))
			   {
			       tracker_full_out[target_index] = 1;
			   }
		       }
		       break;
		}
	}
    }
}


/*********************************************************************
*                                                                    *
* Name:            ProcessResponseRequest()                          *
*                                                                    *
* Comment:         It collects transactions info                     *
*                  at the beginning of a packet;                     *
*                  set the priority table to the                     *
*                  current bus values if the                         *
*                  corresponding resource is                         *
*                  VARIABLE_PRIORITY programmed;                     *
*                                                                    *
*                                                                    *
*                                                                    *
**********************************************************************/
void ProcessResponseRequest(int target_index, uchar vpri)
{
    int i;
    for(i=0; i<RETURN_RESOURCE_NUMBER_K; i++)
	/*** if response resource i is programmed as variable priority ***/
	if(response_resourceArbitration[i] == RESPONSE_VARIABLE_PRIORITY)
	    *(response_arbitration_unit_priority + i*NBR_TARGET + target_index) = vpri;
}



/**********************************************************************************
*                                                                                 *
* Name:            Find_Initiator()                                               *
*                                                                                 *
* Comment:         It retrieve the initiator index from the source field          *
*                                                                                 *
* Returned value:  int                                                            *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
int Find_Initiator(uint src_val, uint *init_src_min, uint *init_src_max)
{
    int i;
    for(i=0; i<NBR_INITIATOR; i++)
	if((init_src_min[i] <= src_val)&&(src_val <= init_src_max[i]))
	    return i;
    return -1;
}



/***************************************************************************************
*                                                                                      *
* Name:            Response_Source_Decoder_FSM()                                       *
*                                                                                      *
* Comment:         It models the Response Source Decoder functionality                 *
*                                                                                      *
*                                                                                      *
****************************************************************************************/
//void Response_Source_Decoder(STbus *Out, uchar *response_source_decoder_state, int stbusType, int *response_source_decoder_output,
//			     tracker_cell **tracker_array, fifo *tracker_fifo_struct, uchar *response_arbitration_unit_priority, uchar *response_resourceArbitration,
//			     int RETURN_RESOURCE_NUMBER_K, int NBR_TARGET, uint *init_src_min, uint *init_src_max, int NBR_INITIATOR, const char *node_name, int postedValidOn,
//			     uchar *posting_send_out)
void Response_Source_Decoder()
{
    int target_index, out_resp_src_dec;
    for(target_index=0; target_index<NBR_TARGET; target_index++)
	if(response_source_decoder_state[target_index] == 0)
	{
	    /*** reset output ***/
	    response_source_decoder_output[target_index] = -1;
	    if(Out[target_index].r_req)
	    {
		tracker_cell cell, *pcell = NULL;
		/*** if exist a tracker ***/
		if(tracker_fifo_struct[target_index].fifosize != 0)
		{
		    if(!tracker_empty(target_index))
			pcell = tracker_first(target_index);
		    else
			cout << "\n!!!!! WARNING At T=" << sc_simulation_time() <<"ns => Node\"" << name() << "\": a response request on target port " << target_index
			     << " doesn't match any posted initiator request in the tracker ! r_src "
			     << ((uint)Out[target_index].r_src) << " r_tid " << ((uint)Out[target_index].r_tid) << "  time : " << sc_time_stamp() << "!!!!!\n" << endl;
		}
		else
		{
		    pcell = &cell;
		    cell.src = Out[target_index].r_src;
		    cell.tid = Out[target_index].r_tid;
		    cell.pri = Out[target_index].r_pri;
		}
		out_resp_src_dec = Find_Initiator(pcell->src, init_src_min, init_src_max);
		if(out_resp_src_dec != -1)
		{
//		    printf("postedValidOn %d out_resp_src_dec %d, posting_send_out[out_resp_src_dec] %d \n", postedValidOn, out_resp_src_dec,
//			   posting_send_out[out_resp_src_dec]);
		    /*** if write posting disabled or there's not dummy response running ***/
		    if((postedValidOn == 0) || (posting_send_out[out_resp_src_dec] == 0))
		    {
			/*** set output ***/
			response_source_decoder_output[target_index] = out_resp_src_dec;
			ProcessResponseRequest(target_index, ((uchar)pcell->pri));
			/*** change state ***/
			response_source_decoder_state[target_index] = 1;
		    }
		}
		else
		    cout << "\n!!!!! WARNING At T=" << sc_time_stamp() <<"ns => Node\"" << name() << "\": response source (r_src " << Out[target_index].r_src << ") on target port " 
                         << target_index << " doesn't match any initiator source range !!!!!\n" << endl;
	    }
	}
}


//void Response_Source_Decoder_R_GNT_FeedBack(int NBR_TARGET, STbus *Out, uchar *response_source_decoder_state,
//					    tracker_cell **tracker_array, fifo *tracker_fifo_struct, int stbusType, uchar *tracker_full_out)
void Response_Source_Decoder_R_GNT_FeedBack()
{
    int target_index;
    // Added By Nizar
    //tracker_cell *fictive_variable;
    tracker_cell fictive_variable;
    //End Of Add 
    
    for(target_index=0; target_index<NBR_TARGET; target_index++)
	if(response_source_decoder_state[target_index] == 1)
	    if((Out[target_index].r_req == 1) && (Out[target_index].r_gnt == 1) && (Out[target_index].r_eop == 1))
	    {
		response_source_decoder_state[target_index] = 0;
		if(tracker_fifo_struct[target_index].fifosize != 0)
		{
		  /*** discard tracker cell ***/
		  //tracker_get(target_index);
		  
		  // Added By Nizar
		  //*fictive_variable = tracker_get(target_index);
		  fictive_variable = tracker_get(target_index);
		  //End Of Add

		  /*** reset full output ***/
		  tracker_full_out[target_index] = 0;
		}
	    }
}


/****************************************************************************
*                                                                           *
* Name:            Initiator_Dependency_Stage()                             *
*                                                                           *
* Comment:         It models the Initiator Dependency Stage functionality   *
*                                                                           *
*                                                                           *
*****************************************************************************/
int Initiator_Dependency_Stage(int initiator_index)
{

    if(initiator_index != -1)
    {
      //	if(number_initiator_per_return_resource[initReturnResourceMapping[initiator_index]] == 1)
      //	    return 1;
      //	else
      return In[initiator_index].r_gnt;
    }
    else
	return 0;
}

/**********************************************************************************
*                                                                                 *
* Name:            Real_Response_Request_Generator()                              *
*                                                                                 *
* Comment:         It models the Real Response Request Gen functionality          *
*                                                                                 *
***********************************************************************************/
//void Real_Response_Request_Generator(int *p_r_req_aux1, STbus *In, STbus *Out, int *r_lock_management_output, int *real_r_req, int *initReturnResourceMapping, int *number_initiator_per_return_resource, int NBR_TARGET)
void Real_Response_Request_Generator()
{
    int i;
    for(i=0; i<NBR_TARGET; i++)
    {
	if(Initiator_Dependency_Stage(p_r_req_aux1[i]) == 1)
	{
	   if((Out[i].r_req == 1) && ((r_lock_management_output[p_r_req_aux1[i]] == -1) || (r_lock_management_output[p_r_req_aux1[i]] == i)))
	       real_r_req[i] = p_r_req_aux1[i];
	   else
	       real_r_req[i] = -1;
	}
	else
	    real_r_req[i] = -1;
    }
}


/******************************************************************************************************
*                                                                                                     *
* Name:            ResponseArbitration_Unit()                                                         *
*                                                                                                     *
* Comment:         It models the Response Arbitration Unit functionality                              *
*                                                                                                     *
*******************************************************************************************************/
//void ResponseArbitration_Unit(int RETURN_RESOURCE_NUMBER_K, int *real_r_req, uchar *response_arbitration_unit_priority,
//			      int *initReturnResourceMapping, int *response_arbitration_unit_result, int NBR_TARGET)
void ResponseArbitration_Unit()
{
    int i, response_resource_index, winner_target_priority;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	winner_target_priority = -1;
	response_arbitration_unit_result[response_resource_index] = -1;
	for (i = 0; i < NBR_TARGET; i++)
	{
	    if (real_r_req[i] != -1)
		if((initReturnResourceMapping[real_r_req[i]] == response_resource_index) &&
		(*(response_arbitration_unit_priority + response_resource_index*NBR_TARGET + i) > winner_target_priority))
	    {
		response_arbitration_unit_result[response_resource_index] = i;
		winner_target_priority = *(response_arbitration_unit_priority + response_resource_index*NBR_TARGET + i);
	    }
	}
    }
}



/***************************************************************************************
*                                                                                      *
* Name:            Write_Posting_Block()                                               *
*                                                                                      *
* Comment:         It models the Write Posting Block Functionality                     *
*                                                                                      *
***************************************************************************************/
//void Write_Posting_Block(int NBR_INITIATOR, STbus *In, tracker_cell **posting_array, fifo *posting_fifo_struct, uchar *posting_send_state, int stbusType)
void Write_Posting_Block()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	if((posting_send_state[initiator_index] == 1)&&((postfifo_first(initiator_index)->tid & 0x020) != 0))
	{
		/*** set dummy cell on the bus ***/
		In[initiator_index].r_req = 1;
		In[initiator_index].r_opc = 0;
		In[initiator_index].r_eop = 1;
		In[initiator_index].r_src = postfifo_first(initiator_index)->src;
		In[initiator_index].r_tid = postfifo_first(initiator_index)->tid;
		In[initiator_index].r_pri = postfifo_first(initiator_index)->pri;
		In[initiator_index].r_lck = postfifo_first(initiator_index)->lck;
		In[initiator_index].r_eop = (stbusType == 2)? (postfifo_first(initiator_index)->num_cell == 1) : 1;
	}
}

/***************************************************************************************
*                                                                                      *
* Name:            Write_Posting_Block()                                               *
*                                                                                      *
* Comment:         It models the Write Posting Block Functionality                     *
*                                                                                      *
***************************************************************************************/
//void Write_Posting_Block_R_GNT_FeedBack(int NBR_INITIATOR, STbus *In, int *initReturnResourceMapping, uchar *response_transaction_management_state,
//					int *response_transaction_management_output, tracker_cell **posting_array, fifo *posting_fifo_struct, uchar *posting_send_state,
//					uchar *posting_send_out, int stbusType)
void Write_Posting_Block_R_GNT_FeedBack()
{
    int initiator_index, response_resource_index;
    // Added By Nizar
    //tracker_cell *fictive_variable;
    tracker_cell fictive_variable;
    //End Of Add 

    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
	switch(posting_send_state[initiator_index])
	{
	    case 1:
		if((In[initiator_index].r_req == 1)&&(In[initiator_index].r_gnt == 1))
		{
		    if(stbusType == 3)
		    {
		      //postfifo_get(initiator_index);
		      // Add by Nizar
		      //*fictive_variable = postfifo_get(initiator_index);
		      fictive_variable = postfifo_get(initiator_index);
		      // end Of Add
		      
			posting_send_state[initiator_index] = 0;
			posting_send_out[initiator_index] = 0;
		    }
		    else
		    if(stbusType == 2)
		    {
			postfifo_first(initiator_index)->num_cell--;
			if(postfifo_first(initiator_index)->num_cell == 0)
			{
			    posting_send_state[initiator_index] = 0;
			    //postfifo_get(initiator_index);
			    // Add by Nizar
			    //*fictive_variable = postfifo_get(initiator_index);
			    fictive_variable = postfifo_get(initiator_index);
			    // end Of Add
			    
//			    printf("num cell = 0; pull\n");
			}
			else
			    break;
		    }
		}
		else
		    break;

	    case 0:
		if(!postfifo_empty(initiator_index))
		{
		    if(stbusType == 3)
		    {
			response_resource_index = initReturnResourceMapping[initiator_index];
			if((response_transaction_management_state[response_resource_index] == 0) ||
			   (response_transaction_management_output[response_resource_index] != initiator_index))
			{
			    posting_send_state[initiator_index] = 1;
			    posting_send_out[initiator_index] = 1;
			}
		    }
		    else
			if(stbusType == 2)
			    posting_send_state[initiator_index] = 1;
		}
		break;

	}
}

/***************************************************************************************
*                                                                                      *
* Name:            Response_Control_Signal_Generator()                                 *
*                                                                                      *
* Comment:         It models the Response_Control_Signal_Generator  Functionality      *
*                                                                                      *
***************************************************************************************/
//void Response_Control_Signal_Generator(int RETURN_RESOURCE_NUMBER_K, int *p_r_req_aux2, int *real_r_req, STbus *In, STbus *Out,
//				       int postedValidOn, tracker_cell **tracker_array, fifo *tracker_fifo_struct)
void Response_Control_Signal_Generator()
{
    int response_resource_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	int target_index = p_r_req_aux2[response_resource_index];

	if(target_index != -1)
	{
	    tracker_cell cell, *pcell;
	    if(tracker_fifo_struct[target_index].fifosize != 0)
		pcell = tracker_first(target_index);
	    else
	    {
		pcell = &cell;
		cell.tid = Out[target_index].r_tid;
	    }
	    if((Out[target_index].r_req == 1)&&(real_r_req[target_index] != -1))
	    {
		Out[target_index].r_gnt = In[real_r_req[target_index]].r_gnt;
#ifndef NOSNOOP
                SnoopPort_r[response_resource_index].r_gnt = In[real_r_req[target_index]].r_gnt;
#endif
		if((!postedValidOn)||((pcell->tid & 0x20) == 0))
		{
#ifndef NOSNOOP
                    SnoopPort_r[response_resource_index].r_req = 1;
#endif
		    In[real_r_req[target_index]].r_req = 1;
		}
	    }
	}
    }
}


/***************************************************************************************
*                                                                                      *
* Name:            Response_Transaction_Management_Out()                               *
*                                                                                      *
* Comment:         It models the Transaction Management  Functionality                 *
*                                                                                      *
***************************************************************************************/
//void Response_Transaction_Management_Out(int RETURN_RESOURCE_NUMBER_K, uchar *response_transaction_management_state,
//					 int *response_arbitration_unit_result, int *response_transaction_management_output)
void Response_Transaction_Management_Out()
{
    int response_resource_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
	if(response_transaction_management_state[response_resource_index] == 0)
	    /*** propagate arbitration vector to the transaction management output ***/
	    response_transaction_management_output[response_resource_index] = response_arbitration_unit_result[response_resource_index];
}


/***************************************************************************************
*                                                                                      *
* Name:            Response_Transaction_Management_FSM()                               *
*                                                                                      *
* Comment:         It models the Transaction Management  Functionality                 *
*                                                                                      *
***************************************************************************************/
//void Response_Transaction_Management_FSM(int RETURN_RESOURCE_NUMBER_K, STbus *Out, uchar *response_transaction_management_state,
//					 int *response_transaction_management_output)
void Response_Transaction_Management_FSM()
{
    int target_index, response_resource_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	target_index = response_transaction_management_output[response_resource_index];
	if(target_index != -1)
	{
	    switch(response_transaction_management_state[response_resource_index])
	    {
	    case 0:
		if (Out[target_index].r_req == 1)
		{
			if((Out[target_index].r_gnt == 0)||(Out[target_index].r_eop == 0))
			    response_transaction_management_state[response_resource_index] = 1;
		}
		break;

	    case 1:
		/*** if end of current message/packet ***/
		if ((Out[target_index].r_req == 1) && (Out[target_index].r_gnt == 1) && (Out[target_index].r_eop == 1))
		{
		    /*** return to the idle state ***/
		    response_transaction_management_state[response_resource_index] = 0;
		}
		break;
	    }
	}
    }
}

//void Inverse_Datapath(int RETURN_RESOURCE_NUMBER_K, STbus *In, STbus *Out, int *p_r_req_aux2,
//		      int *p_r_req_aux1, tracker_cell **tracker_array, fifo *tracker_fifo_struct, uint *init_src_min, uchar *init_src_mask, int postedValidOn)
void Inverse_Datapath()
{
    int response_resource_index, target_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	target_index = p_r_req_aux2[response_resource_index];
	if(target_index != -1)
	{
	    int initiator_index = p_r_req_aux1[target_index];
	    if(Out[target_index].r_req == 1)
	    {
		/*** retrieve response src, tid, pri ***/
		tracker_cell cell, *pcell;
		if(tracker_fifo_struct[target_index].fifosize != 0)
		    pcell = tracker_first(target_index);
		else
		{
		    pcell = &cell;
		    cell.src = Out[target_index].r_src;
		    cell.tid = Out[target_index].r_tid;
		    cell.pri = Out[target_index].r_pri;
		}
		/*** datapath select and signal copy ***/
		if((initiator_index != -1) && ((!postedValidOn) || ((pcell->tid & 0x20) == 0)))
		{
		    In[initiator_index].r_opc = Out[target_index].r_opc;
		    In[initiator_index].r_eop = Out[target_index].r_eop;
		    if(init_src_mask[initiator_index] == 1)
			In[initiator_index].r_src = pcell->src & (~init_src_min[initiator_index]);
		    else
			In[initiator_index].r_src = pcell->src;
		    In[initiator_index].r_tid = pcell->tid;
		    In[initiator_index].r_pri = pcell->pri;
		    In[initiator_index].r_lck = Out[target_index].r_lck;
		    In[initiator_index].r_data = Out[target_index].r_data;
//		    printf("\n==============  target index %d  initiator index %d r_src %x r_tid %x inverse bus copy ====================\n\n", target_index, initiator_index,
//			   In[initiator_index].r_src, In[initiator_index].r_tid);
#ifndef NOSNOOP
                      /* Setting values for the snoop port */
                    if (SnoopingActive)
                    {
                        SnoopPort_r[response_resource_index].r_opc = Out[target_index].r_opc;
                        SnoopPort_r[response_resource_index].r_eop = Out[target_index].r_eop;
                        if(init_src_mask[initiator_index] == 1)
                            SnoopPort_r[response_resource_index].r_src = pcell->src & (~init_src_min[initiator_index]);
                        else
                            SnoopPort_r[response_resource_index].r_src = pcell->src;
                        SnoopPort_r[response_resource_index].r_tid = pcell->tid;
                        SnoopPort_r[response_resource_index].r_pri = pcell->pri;
                        SnoopPort_r[response_resource_index].r_lck = Out[target_index].r_lck;
                        SnoopPort_r[response_resource_index].r_data = Out[target_index].r_data;
                    }
#endif
		}
//		else
//		    printf("************************** INVERSE DATAPATH OCCHIO !!!!!!!!!!!!!: initiator_index = -1 ********************************\n");
	    }
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Init_Retiming()                                            *
*                                                                             *
* Comment:         It models the Iint Retiming Stage Functionality            *
*                                                                             *
*******************************************************************************/
//void Init_Retiming(int NBR_INITIATOR, uchar *init_retiming_state, int *p_req_aux1, int *p_req_aux2, STbus *In)
void Init_Retiming()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	switch(init_retiming_state[initiator_index])
	{
	    case 0:
		if((p_req_aux1[initiator_index] != -1)&&(In[initiator_index].req == 1))
		{
		    p_req_aux2[initiator_index] = -1;
		    init_retiming_state[initiator_index] = 1;
		}
		else
		    p_req_aux2[initiator_index] = p_req_aux1[initiator_index];
		break;

	    case 1:
		/*** request copy ***/
		p_req_aux2[initiator_index] = p_req_aux1[initiator_index];
		break;
	}
    }
}

/******************************************************************************
*                                                                             *
* Name:            Init_Retiming_GNT_FeedBack()                               *
*                                                                             *
* Comment:         It models the Init Retiming Stage Functionality            *
*                                                                             *
*******************************************************************************/
//void Init_Retiming_GNT_FeedBack(int NBR_INITIATOR, uchar *init_retiming_state, STbus *In)
void Init_Retiming_GNT_FeedBack()
{
    int initiator_index;
    for(initiator_index=0; initiator_index<NBR_INITIATOR; initiator_index++)
    {
	if(init_retiming_state[initiator_index] == 1)
	{
	    if((In[initiator_index].req == 1)&&(In[initiator_index].gnt == 1)&&(In[initiator_index].eop == 1))
		init_retiming_state[initiator_index] = 0;
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Target_Return_Retiming()                                   *
*                                                                             *
* Comment:         It models the Response Retiming Stage Functionality        *
*                                                                             *
*******************************************************************************/
void Target_Return_Retiming()
{
    int target_index;
    for(target_index=0; target_index<NBR_TARGET; target_index++)
    {
	switch(target_return_retiming_state[target_index])
	{
	    case 0:
		if((response_source_decoder_output[target_index] != -1)&&(Out[target_index].r_req == 1))
		{
		    target_return_retiming_output[target_index] = -1;
//		    printf("retiming output = -1\n");
		    target_return_retiming_state[target_index] = 1;
		}
		else
		    target_return_retiming_output[target_index] = response_source_decoder_output[target_index];
		break;

	    case 1:
		/*** request copy ***/
		target_return_retiming_output[target_index] = response_source_decoder_output[target_index];
		break;
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Target_Return_Retiming_GNT_FeedBack()                      *
*                                                                             *
* Comment:         It models the Response Retiming Stage Functionality        *
*                                                                             *
*******************************************************************************/
//void Target_Return_Retiming_R_GNT_FeedBack(int NBR_TARGET, uchar *target_return_retiming_state, STbus *Out)
void Target_Return_Retiming_R_GNT_FeedBack()
{
    int target_index;
    for(target_index=0; target_index<NBR_TARGET; target_index++)
    {
	if(target_return_retiming_state[target_index] == 1)
	{
	    if((Out[target_index].r_req == 1)&&(Out[target_index].r_gnt == 1)&&(Out[target_index].r_eop == 1))
		target_return_retiming_state[target_index] = 0;
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Target_Retiming()                                          *
*                                                                             *
* Comment:         It models the Target Retiming Stage Functionality          *
*                                                                             *
*******************************************************************************/
//void Target_Retiming(int RESOURCE_NUMBER_K, uchar *target_retiming_state, int *transaction_management_output, int *target_retiming_output, STbus *In)
void Target_Retiming()
{
    int resource_index;
    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	switch(target_retiming_state[resource_index])
	{
	    case 0:
		if((transaction_management_output[resource_index] != -1)&&(In[transaction_management_output[resource_index]].req == 1))
		{
		    target_retiming_output[resource_index] = -1;
		    target_retiming_state[resource_index] = 1;
		}
		else
		    target_retiming_output[resource_index] = transaction_management_output[resource_index];
		break;

	    case 1:
		/*** request copy ***/
		target_retiming_output[resource_index] = transaction_management_output[resource_index];
		break;
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Target_Retiming_GNT_FeedBack()                             *
*                                                                             *
* Comment:         It models the Target Retiming Stage Functionality          *
*                                                                             *
*******************************************************************************/
//void Target_Retiming_GNT_FeedBack(int RESOURCE_NUMBER_K, uchar *target_retiming_state, int *target_retiming_output, STbus *In)
void Target_Retiming_GNT_FeedBack()
{
    int resource_index;
    for(resource_index=0; resource_index<RESOURCE_NUMBER_K; resource_index++)
    {
	if((target_retiming_state[resource_index] == 1)&&(target_retiming_output[resource_index] != -1))
	{
	    if((In[target_retiming_output[resource_index]].req == 1)&&(In[target_retiming_output[resource_index]].gnt == 1)&&
	       (In[target_retiming_output[resource_index]].eop == 1))
		target_retiming_state[resource_index] = 0;
	}
    }
}

/******************************************************************************
*                                                                             *
* Name:            Init_Return_Retiming()                                     *
*                                                                             *
* Comment:         It models the Init Return Retiming Stage Functionality     *
*                                                                             *
*******************************************************************************/
//void Init_Return_Retiming(int RETURN_RESOURCE_NUMBER_K, uchar *init_return_retiming_state, int *response_transaction_management_output, int *init_return_retiming_output, STbus *Out)
void Init_Return_Retiming()
{
    int response_resource_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	switch(init_return_retiming_state[response_resource_index])
	{
	    case 0:
		if((response_transaction_management_output[response_resource_index] != -1)&&
		   (Out[response_transaction_management_output[response_resource_index]].r_req == 1))
		{
		    init_return_retiming_output[response_resource_index] = -1;
		    init_return_retiming_state[response_resource_index] = 1;
		}
		else
		    init_return_retiming_output[response_resource_index] = response_transaction_management_output[response_resource_index];
		break;

	    case 1:
		/*** request copy ***/
		init_return_retiming_output[response_resource_index] = response_transaction_management_output[response_resource_index];
		break;
	}
    }
}


/******************************************************************************
*                                                                             *
* Name:            Init_Return_Retiming_R_GNT_FeedBack()                      *
*                                                                             *
* Comment:         It models the INit Return Retiming Stage Functionality     *
*                                                                             *
*******************************************************************************/
//void Init_Return_Retiming_R_GNT_FeedBack(int RETURN_RESOURCE_NUMBER_K, uchar *init_return_retiming_state, int *init_return_retiming_output, STbus *Out)
void Init_Return_Retiming_R_GNT_FeedBack()
{
    int response_resource_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	if((init_return_retiming_state[response_resource_index] == 1)&&(init_return_retiming_output[response_resource_index] != -1))
	{
	    if((Out[init_return_retiming_output[response_resource_index]].r_req == 1)&&(Out[init_return_retiming_output[response_resource_index]].r_gnt == 1)&&
	       (Out[init_return_retiming_output[response_resource_index]].r_eop == 1))
		init_return_retiming_state[response_resource_index] = 0;
	}
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Lock_Management_Block()                                        *
*                                                                                 *
* Comment:         It models the Lock_Management_Block functionality              *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Response_Lock_Management_Block(STbus *Out, int *p_r_req_aux1, uchar *r_lock_management_state, int *r_lock_management_output,
//				    int NBR_TARGET)
void Response_Lock_Management_Block()
{
    int target_index;
    for(target_index=0; target_index<NBR_TARGET; target_index++)
    {
	if(p_r_req_aux1[target_index] != -1)
	    switch(r_lock_management_state[target_index])
	    {
	       case 0:
		   if((Out[target_index].r_req == 1)&&(Out[target_index].r_gnt == 1)&&(Out[target_index].r_lck == 1))
		   {
		       /*** change state to packet already started ***/
		       r_lock_management_state[target_index] = 1;
		       /*** set the filter r_lock_management_output to lock the current resource for the current master initiator ***/
		       r_lock_management_output[p_r_req_aux1[target_index]] = target_index;
		   }
		   break;

	       case 1:
		   if((Out[target_index].r_req == 1)&&(Out[target_index].r_gnt == 1)&&(Out[target_index].r_lck == 0)&&
		      (Out[target_index].r_eop == 1))
		   {
		       /*** change state to end of packet ***/
		       r_lock_management_state[target_index] = 0;
		       /*** reset the filter r_lock_management_output to enable all the request to this resource ***/
		       r_lock_management_output[p_r_req_aux1[target_index]] = -1;
		   }
		   break;
	    }

    }
}

/**********************************************************************************
*                                                                                 *
* Name:            Inc_Response_LRU_Counters()                                    *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void Inc_Response_LRU_Counters(int target_index, uchar *p_response_LRU_counters)
{
    int i;
    for(i=0; i<NBR_TARGET; i++)
    {
	if((i != target_index)&&(p_response_LRU_counters[i] < 31))
	    p_response_LRU_counters[i] = p_response_LRU_counters[i] + 1;
//	printf("resp_pri[%d] = %d  ", i, p_response_LRU_counters[i]);
    }
//    printf("\n\n");
}

/**********************************************************************************
*                                                                                 *
* Name:            Response_LRU_Algorithm()                                       *
*                                                                                 *
* Comment:                                                                        *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Response_LRU_Algorithm(int response_resource_index, int target_index, int NBR_TARGET, uchar *response_LRU_target_packet_state, STbus *Out, int *p_r_req_aux1, uchar *response_LRU_counters)
void Response_LRU_Algorithm(int response_resource_index, int target_index)
{
    /*** NOTE: target_index == response_transaction_management_output[response_resource_index] -> internal grant = 1 & direct connection with resource ***/
    if(target_index != -1)
	if(p_r_req_aux1[target_index] != -1)
	{
	    int  base_index = response_resource_index*NBR_TARGET;
	    switch(response_LRU_target_packet_state[target_index])
	    {
	        case 0:
		{
		    /*** reset target counter ***/
		    response_LRU_counters[base_index + target_index] = 0;

		    Inc_Response_LRU_Counters(target_index, (response_LRU_counters + base_index));
		    if(Out[target_index].r_eop == 0)
			response_LRU_target_packet_state[target_index] = 1;
		}
		break;

	        case 1:
		    if((Out[target_index].r_req == 1)&&(Out[target_index].r_gnt == 1)&&(Out[target_index].r_eop == 1))
			response_LRU_target_packet_state[target_index] = 0;
		break;
	}
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Priority_Management_Unit()                                     *
*                                                                                 *
* Comment:         It models the Control Unit functionality                       *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Priority_Management_Unit_FSM(int RETURN_RESOURCE_NUMBER_K, uchar *response_resourceArbitration, int NBR_TARGET, uchar *response_LRU_target_packet_state, STbus *Out,
//				  int *p_r_req_aux1, uchar *response_LRU_counters, uchar *response_arbitration_unit_priority, int *response_transaction_management_output,
//				  int LATCHED_PRI)
void Priority_Management_Unit_FSM()
{
    int response_resource_index, i, base_index;
    for(response_resource_index=0; response_resource_index<RETURN_RESOURCE_NUMBER_K; response_resource_index++)
    {
	switch(response_resourceArbitration[response_resource_index])
	{
	    /*** PRIORITY (DYNAMIC OR VARIABLE) ***/
	case RESPONSE_DYNAMIC_PRIORITY:
	    break;
	case RESPONSE_VARIABLE_PRIORITY:
	    break;
	    /*** LRU ***/
	case LRU:
	    base_index = response_resource_index*NBR_TARGET;
	    /*** this cause retiming on arb_priorities ***/
	    for(i=0; i<NBR_TARGET; i++)
		*(response_arbitration_unit_priority + base_index + i) = *(response_LRU_counters + base_index + i);
	    Response_LRU_Algorithm(response_resource_index, response_transaction_management_output[response_resource_index]);
	    break;
	}
    }
}

/**********************************************************************************
*                                                                                 *
* Name:            T1_Get_reg_val()                                               *
*                                                                                 *
* Comment:         It returns the register contents corresponding to the addr     *
*                                                                                 *
* Returned value:  uchar (if 1 -> wrong address)                                  *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//uchar T1_Get_reg_val(uint *initPriorityReset, uint *initBandwidthReset, uint *initLatencyReset, uint *targPriorityReset,
//		       int NBR_INITIATOR, int NBR_TARGET, uint addr, uint *val)
uchar T1_Get_reg_val(uint addr, uint *val)
{
  if(addr < (NBR_INITIATOR<<4))
    {
      uchar reg_set = addr/(NBR_INITIATOR*4), reg_index = (addr/4)%NBR_INITIATOR;
      //	printf("reg set %d reg index %d\n", reg_set, reg_index);
      switch(reg_set)
        {
	case 0:
	  *val = initPriorityReset[reg_index]|(init_interrupt_rule_reset[reg_index]<<5);
	  return 0;
	  
	case 1:
	  *val = initLatencyReset[reg_index];
	  return 0;
	  
	case 2:
	  *val = initBandwidthReset[reg_index];
	  return 0;
	  
	case 3:
	  *val = initLowPriorityReset[reg_index] | (initFrameSizeReset[reg_index] << 4) | (initWordsLimitReset[reg_index] << 12);
	  return 0;
	  
	default:
	  return 1;
	}
    }
  else
    {
      uchar reg_index = (addr - (NBR_INITIATOR<<4))/4;
      if(reg_index < NBR_TARGET)
	{
	  *val = targPriorityReset[reg_index];
	  return 0;
	}
      else
	return 1;
    }
}

/**********************************************************************************
 *                                                                                 *
 * Name:            T1_Put_reg_val()                                               *
 *                                                                                 *
 * Comment:         It fills the register corresponding to the addr                *
 *                                                                                 *
 * Returned value:  uchar (if 1 -> wrong address)                                  *
 *                                                                                 *
 *                                                                                 *
 ***********************************************************************************/
//uchar T1_Put_reg_val(uint *initPriorityReset, uint *initBandwidthReset, uint *initLatencyReset, uint *targPriorityReset,
//		     int NBR_INITIATOR, int NBR_TARGET, uint addr, uint val, uchar vbe ,uchar *resourceArbitration,
//		     int RESOURCE_NUMBER_K, uchar *arbitration_unit_priority, uchar *response_resourceArbitration,
//		     int RETURN_RESOURCE_NUMBER_K, uchar *response_arbitration_unit_priority)
uchar T1_Put_reg_val(uint addr, uint val, uchar vbe )
{
  int i;
  uint mask = 0xffffff00, not_mask = 0;
  for(i = 0; i <4; i++)
    {
      if((vbe & 0x01) == 0)
	{
	  val &= mask;
	  not_mask |= ~mask;
	}
      mask = mask<<8 | 0x0ff;
      vbe >>= 1;
    }
  
  if(addr < ((NBR_INITIATOR<<4)))
    {
      uchar reg_set = addr/(NBR_INITIATOR*4), reg_index = (addr/4)%NBR_INITIATOR;
      switch(reg_set)
	{
	case 0:
	  initPriorityReset[reg_index] = initPriorityReset[reg_index]&not_mask | (val & 0x1f);
	  init_interrupt_rule_reset[reg_index] = init_interrupt_rule_reset[reg_index] | ((val & 0x20)>>5);
	  //	    Set_init_priority_vector();
	  return 0;
	  
	case 1:
	  initLatencyReset[reg_index] = initLatencyReset[reg_index]&not_mask | (val & 0x0ffff);
	  return 0;
	  
	case 2:
	  initBandwidthReset[reg_index] = initBandwidthReset[reg_index]&not_mask | (val & 0x0ffff);
	  return 0;
	  
	case 3:
	  {
	    /*** set 4 lower bits of val for pri ***/
	    uint low_pri_mask = not_mask & 0x0f;
	    initLowPriorityReset[reg_index] = initLowPriorityReset[reg_index] & low_pri_mask | (val & 0x0f );

	    low_pri_mask = ( not_mask & 0xff0 ) >> 4;
	    initFrameSizeReset[reg_index] = initFrameSizeReset[reg_index] & low_pri_mask | ( (val & 0xff0) >> 4 );
	    initFrameSizeValue[reg_index] = bandwidthFrameSizeValues[initFrameSizeReset[reg_index]];
	    bandwidthFrameSizeCounter[reg_index] = bandwidthFrameSizeValues[initFrameSizeReset[reg_index]] - 1;

	    low_pri_mask = ( not_mask & 0x1f000 ) >> 12;
	    initWordsLimitReset[reg_index] = initWordsLimitReset[reg_index] & low_pri_mask | ( (val & 0x1f000) >> 12 );
	    initWordsLimitValue[reg_index] = bandwidthWordsLimitValues[initWordsLimitReset[reg_index]];
	    bandwidthWordsLimitCounter[reg_index] = 0;

	    initBandwidthLimiterMovingWindowState[reg_index] = 0;
	    initBandwidthLimiterSwitchLowToHighState[reg_index] = 0;
	    initBandwidthLimiterSwitchHighToLowStateDone[reg_index] = 0;

	    return 0;
	  }
	  
	default:
	  return 1;
	}
    }
  else
    {
      uchar reg_index = (addr - (NBR_INITIATOR<<4))/4;
      if(reg_index < NBR_TARGET)
	{
	  targPriorityReset[reg_index] = targPriorityReset[reg_index]&not_mask | (val & 0x1f);
	  Set_target_priority_vector();
	  return 0;
	}
      else
	return 1;
    }
}


/**********************************************************************************
 *                                                                                 *
 * Name:            Programming_port_Handler()                                     *
 *                                                                                 *
 * Comment:         It manage the programming port for register access             *
 *                                                                                 *
 * Returned value:  void                                                           *
 *                                                                                 *
 *                                                                                 *
 ***********************************************************************************/
//void Programming_port_Handler(uchar *pp_state, STbus *p_Prog, uint *initPriorityReset, uint *initBandwidthReset, uint *initLatencyReset, uint *targPriorityReset,
//			      int NBR_INITIATOR, int NBR_TARGET, uchar *resourceArbitration, int RESOURCE_NUMBER_K, uchar *arbitration_unit_priority,
//			      uchar *response_resourceArbitration, int RETURN_RESOURCE_NUMBER_K, uchar *response_arbitration_unit_priority)
void Programming_port_Handler()
{
  int debug_req;
  debug_req = p_Prog->req;
  switch(pp_state)
    {
    case 0:
      if((p_Prog->req == 1) && (p_Prog->r_req == 1))
	{
	  p_Prog->r_req = 0;
	  p_Prog->r_opc = 0;
	  pp_state = 1;
	}
      break;
    case 1:
      if(p_Prog->req == 1)
	{
	  // Address masked to fit register port address size
	  uint adr = ((uint) p_Prog->add) & 0x3ff;
	  
	  /*** if store ***/
	  if((p_Prog->opc & 0x1) == 0)
	    {
	      uint val = p_Prog->data;
	      //uchar res = T1_Put_reg_val(((uint) p_Prog->add), val, ((uchar) p_Prog->be));
	      uchar res = T1_Put_reg_val(adr , val, ((uchar) p_Prog->be));
	      
	      p_Prog->r_req = 1;
	      p_Prog->r_opc = res;
	      pp_state = 0;
	    }
	  else
	    {
	      /*** load ***/
	      uint val;
	      //uchar res = T1_Get_reg_val(((uint) p_Prog->add), &val);
	      uchar res = T1_Get_reg_val(adr, &val);
	      
	      p_Prog->r_req = 1;
	      p_Prog->r_data = val;
	      p_Prog->r_opc = res;
	      pp_state = 0;
	    }
	}
      break;
    }
}


/**********************************************************************************
*                                                                                 *
* Name:            Set_init_priority_vector()                                     *
*                                                                                 *
* Comment:         It set the arbitration priority vector                         *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Set_init_priority_vector(uchar *resourceArbitration, int RESOURCE_NUMBER_K, uchar *arbitration_unit_priority, int NBR_INITIATOR, uint *initPriorityReset)
void Set_init_priority_vector()
{
    int i, j;
    for (i = 0; i<RESOURCE_NUMBER_K; i++)
      for (j = 0; j < NBR_INITIATOR; j++)
	  if(((resourceArbitration[i] == PRIORITY)&&(initPriorityType[j] == REGISTER_PRI))||(resourceArbitration[i] == BANDWITH))
	    *(arbitration_unit_priority + i*NBR_INITIATOR + j) = initPriorityReset[j];
}


/**********************************************************************************
*                                                                                 *
* Name:            Set_target_priority_vector()                                   *
*                                                                                 *
* Comment:         It set the arbitration priority vector                         *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
//void Set_target_priority_vector(uchar *response_resourceArbitration, int RETURN_RESOURCE_NUMBER_K, uchar *response_arbitration_unit_priority,
//				int NBR_TARGET, uint *targPriorityReset)
void Set_target_priority_vector()
{
    int i, j;
    for (i = 0; i<RETURN_RESOURCE_NUMBER_K; i++)
    if(response_resourceArbitration[i] == RESPONSE_DYNAMIC_PRIORITY)
	for (j = 0; j < NBR_TARGET; j++)
	    *(response_arbitration_unit_priority + i*NBR_TARGET + j) = targPriorityReset[j];
}

/**********************************************************************************
*                                                                                 *
* Name:            CatIntegerToString()                                           *
*                                                                                 *
* Comment:         It Adds an integer at the end of a string                      *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
char * CatNodeNameToString(char * string_to_cat)
{
  char * temp_string;
  temp_string = new char[100];

  sprintf(temp_string, "%s_", name()); 
  //strcpy(temp_string, string_to_cat);
  return strcat(temp_string, string_to_cat);
}

/**********************************************************************************
*                                                                                 *
* Name:            CatIntegerToString()                                           *
*                                                                                 *
* Comment:         It Adds an integer at the end of a string                      *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
char * CatIntegerToString(char * string_to_cat, int integer_to_cat)
{
  char * temp_string;
  char * temp_char;
  temp_string = new char[100];
  temp_char = new char[3];
  sprintf(temp_char,"%d", integer_to_cat);
  strcpy(temp_string, string_to_cat);
  return strcat(temp_string,temp_char);
}

/**********************************************************************************
*                                                                                 *
* Name:            TraceNodeInternalVariables()                                   *
*                                                                                 *
* Comment:         It Traces the Node Internal Variables                          *
*                                                                                 *
* Returned value:  void                                                           *
*                                                                                 *
*                                                                                 *
***********************************************************************************/
void TraceNodeInternalVariables(sc_trace_file * tf_node_debug)
{ 
  //Trace Clock And Reset
  //sc_trace(tf_node_debug, clk, "ClockGen_clk");
  //sc_trace(tf_node_debug, rst, "ResetGen_rst");

  sc_trace(tf_node_debug, initPriorityOut_changed, CatNodeNameToString("initPriorityOut_changed"));
  sc_trace(tf_node_debug, bandwidthPriorityOut_changed, CatNodeNameToString("bandwidthPriorityOut_changed"));
  
//Initiators Part
  for(i = 0; i < NBR_INITIATOR; i++)
    { 
      //Priority Output
      sc_trace(tf_node_debug, initPriorityOut[i], CatNodeNameToString(CatIntegerToString("initPriorityOut_", i+1)));
      //Real Request
      sc_trace(tf_node_debug, real_req[i], CatNodeNameToString(CatIntegerToString("real_req_", i+1)));
 
      // Address Decoder
      sc_trace(tf_node_debug, address_decoder_state[i], CatNodeNameToString(CatIntegerToString("address_decoder_state_", i+1)));
      sc_trace(tf_node_debug, hot_vector_req[i], CatNodeNameToString(CatIntegerToString("hot_vector_req_", i+1)));
 
      //Ordering Block
      sc_trace(tf_node_debug, ord_blk_counter[i], CatNodeNameToString(CatIntegerToString("ord_blk_counter_", i+1)));
      sc_trace(tf_node_debug, ord_blk_reg[i], CatNodeNameToString(CatIntegerToString("ord_blk_reg_", i+1)));
      sc_trace(tf_node_debug, ord_blk_output[i], CatNodeNameToString(CatIntegerToString("ord_blk_output_", i+1)));

      // Posting Fifos
      sc_trace(tf_node_debug, posting_fifo_struct[i].count, CatNodeNameToString(CatIntegerToString("posting_fifo_struct_count_", i+1)));
      sc_trace(tf_node_debug, posting_fifo_struct[i].fifosize, CatNodeNameToString(CatIntegerToString("posting_fifo_struct_fifosize_", i+1)));

      //Bandwidth Limiter Counters
      sc_trace(tf_node_debug, initFrameSizeValue[i], CatNodeNameToString(CatIntegerToString("bandwidthFrameSizeValues_", i+1)));
      //sc_trace(tf_node_debug, bandwidthFrameSizeValues[initFrameSizeReset[i]], CatNodeNameToString(CatIntegerToString("bandwidthFrameSizeValues_", i+1)));
      sc_trace(tf_node_debug, bandwidthFrameSizeCounter[i], CatNodeNameToString(CatIntegerToString("bandwidthFrameSizeCounter_", i+1)));
      sc_trace(tf_node_debug, initWordsLimitValue[i], CatNodeNameToString(CatIntegerToString("bandwidthWordsLimitValues_", i+1)));
      //sc_trace(tf_node_debug, bandwidthWordsLimitValues[initWordsLimitReset[i]], CatNodeNameToString(CatIntegerToString("bandwidthWordsLimitValues_", i+1)));
      sc_trace(tf_node_debug, bandwidthWordsLimitCounter[i], CatNodeNameToString(CatIntegerToString("bandwidthWordsLimitCounter_" ,i+1)));
      sc_trace(tf_node_debug, bandwidthLimiterPriorityOutput[i], CatNodeNameToString(CatIntegerToString("bandwidthLimiterPriorityOutput_" ,i+1)));

      // Message Based Request Mask
      sc_trace(tf_node_debug, msg_based_req_mask[i], CatNodeNameToString(CatIntegerToString("msg_based_req_mask_", i+1)));
    }

  for(int res=0; res<RESOURCE_NUMBER_K; res++)
    {
      sc_trace(tf_node_debug, resourceArbitration[res], CatNodeNameToString(CatIntegerToString("resourceArbitration_r",res+1)));
      sc_trace(tf_node_debug, arbitration_unit_result[res], CatNodeNameToString(CatIntegerToString("arbitration_unit_result_r",res+1)));
      sc_trace(tf_node_debug, arbitration_unit_interrupt_load8_state[res], CatNodeNameToString(CatIntegerToString("arbitration_unit_interrupt_load8_state_r",res+1)));

      for(i = 0; i < NBR_INITIATOR; i++)
	{
	  sc_trace(tf_node_debug, *(arbitration_unit_priority + res*NBR_INITIATOR + i), CatNodeNameToString(CatIntegerToString(CatIntegerToString("arbitration_unit_priority_", res+1), i+1)));
	}

      // Message Based Arbitration Scheme
      sc_trace(tf_node_debug, msg_based_req_mask_state[res], CatNodeNameToString(CatIntegerToString("msg_based_req_mask_state_r", res+1)));
      sc_trace(tf_node_debug, msg_based_current_init[res], CatNodeNameToString(CatIntegerToString("msg_based_current_init_r", res+1)));
      sc_trace(tf_node_debug, message_counter[res], CatNodeNameToString(CatIntegerToString("message_counter_r", res+1)));
      sc_trace(tf_node_debug, transaction_management_output[res], CatNodeNameToString(CatIntegerToString("transaction_management_output_",res+1)));
      sc_trace(tf_node_debug, transaction_management_state[res],  CatNodeNameToString(CatIntegerToString("transaction_management_state_",res+1)));
    }

  for(i = 0; i < NBR_TARGET; i++)
    {
      // Tracker Fifos
      sc_trace(tf_node_debug, tracker_fifo_struct[i].count, CatNodeNameToString(CatIntegerToString("tracker_fifo_count_", i+1)));
      sc_trace(tf_node_debug, tracker_fifo_struct[i].fifosize, CatNodeNameToString(CatIntegerToString("tracker_fifo_fifosize_", i+1)));

      // Response Source Decoder
      sc_trace(tf_node_debug, response_source_decoder_state[i], CatNodeNameToString(CatIntegerToString("response_source_decoder_state_", i+1)));
      sc_trace(tf_node_debug, response_source_decoder_output[i], CatNodeNameToString(CatIntegerToString("response_source_decoder_output_", i+1)));
      
      //Real r_req
      sc_trace(tf_node_debug, real_r_req[i], CatNodeNameToString(CatIntegerToString("real_r_req_", i+1)));
    }
}


SC_HAS_PROCESS(STBUS_NODE_BCA_with_snoop);

/******************************************************************** Constructor *************************************************************************/
STBUS_NODE_BCA_with_snoop( sc_module_name name_, double req_wnd, sc_time_unit req_wnd_unit ) :
  sc_module( name_ ), req_wnd( req_wnd ), req_wnd_unit( req_wnd_unit )
{
#ifndef NOSNOOP
            SnoopingActive = true;
#endif
#ifdef DEBUG
  cout << name() << ": instantiated !" << endl;
#endif
  
  SC_METHOD(Async_Thread);
  sensitive_pos << clk;
  
  SC_METHOD(Sync_Thread);
  sensitive_pos << clk;
  
  /* arbitration unit priorities initialization */
  arbitration_unit_priority = new uchar [NBR_INITIATOR*RESOURCE_NUMBER_K];
  /*** check pointer ***/
  Assert_Pointers((void *)arbitration_unit_priority);
  
  /* response arbitration unit priorities initialization */
  response_arbitration_unit_priority = new uchar [NBR_TARGET*RETURN_RESOURCE_NUMBER_K];
  /*** check pointer ***/
  Assert_Pointers((void *)response_arbitration_unit_priority);
  
  /* latency retimed arbitration unit priorities initialization */
  retimed_Latency_arbitration_unit_priority = new uchar [NBR_INITIATOR*RESOURCE_NUMBER_K];
  /*** check pointer ***/
  Assert_Pointers((void *)retimed_Latency_arbitration_unit_priority);
  
  /*** reset asynch flag ***/
  asynch_flag = true;
}
     
void Async_Thread()
{
  //sim_time
  sim_time = sc_simulation_time();

  if(rst.read())
    {
      /*** init state machines ***/
      Node_Reset();
      /*** initialize signals ***/
      Reset_Signals();
    }
  else
    {
      if(asynch_flag)
	{
	  next_trigger( req_wnd, req_wnd_unit );
	  asynch_flag = false;
	  return;
	}
      else
	asynch_flag = true;

      /*--- Latency counters load ---*/
      if(!rst_flag)
	{
	  /*** Load Latency counters ***/
	  for (i = 0; i < NBR_INITIATOR*RESOURCE_NUMBER_K; i++)
	    control_unit_Latency_counters[i] = initLatencyReset[i%NBR_INITIATOR];
	  /*** Reset internal vectors ***/
	  for (i = 0; i < RESOURCE_NUMBER_K; i++)
	    Latency_resource_changed_counts[i] = true;
	  rst_flag = true;
	}
      
      /*--- PRIORITY SELECTION  STAGE---*/
      SetPriorityVector();
      
      /*--- BANDWIDTH LIMITER STAGE---*/
      BandwidthLimiterAsync();
      
      /*--- ARBITRATION PROPAGATION  STAGE---*/
      SetArbitrationVector();	     
      
      /*--- ADDRESS DECODING STAGE---*/
      Address_Decoder();
      
      /*--- ORDERING BLOCK STAGE ---*/
      Ordering_Block();
      
      if(initRetime == 1)
	/*--- INIT RETIMING STAGE ---*/
	Init_Retiming();
      
      /*--- REAL REQUEST GENERATOR STAGE---*/
      Real_Request_Generator();
      
      /*--- MESSAGE BASED FILTERING STAGE---*/
      Message_Request_Mask_Out();
      
      
      /*--- ARBITRATION UNIT---*/
      Arbitration_Unit();
      
      /*--- TRANSACTION MANAGEMENT UNIT - OUTPUT PROPAGATION ---*/
      Transaction_Management_Out();
      
      if(targRetime == 1)
	/*--- TARGET RETIMING STAGE ---*/
	Target_Retiming();
      
      /*--- RESET PORTS ---*/
      ResetPorts();
      
      /*--- CONTROL SIGNAL GENERATOR ---*/
      Control_Signal_Generator();
      
      /*--- DIRECT DATAPATH ---*/
      Direct_Datapath();
      
      if(postedValidOn == 1)
	/*--- WRITE POSTING BLOCK ---*/
	Write_Posting_Block();
      
      /*--- RESPONSE SOURCE DECODER ---*/
      Response_Source_Decoder();
      
      if(targReturnRetime == 1)
	/*--- TARGET RETURN RETIMING STAGE ---*/
	Target_Return_Retiming();
      
      /*--- REAL RESPONSE REQUEST GENERATOR ---*/
      Real_Response_Request_Generator();
      
      /*--- RESPONSE ARBITRATION UNIT---*/
      ResponseArbitration_Unit();
      
      /*--- RESPONSE TRANSACTION MANAGEMENT UNIT - OUTPUT PROPAGATION ---*/
      Response_Transaction_Management_Out();
      
      if(initReturnRetime == 1)
	/*--- INIT RETURN RETIMING STAGE ---*/
	Init_Return_Retiming();
      
      /*--- RESPONSE CONTROL SIGNAL GENERATOR ---*/
      Response_Control_Signal_Generator();
      
      /*--- INVERSE DATAPATH ---*/
      Inverse_Datapath();
    }
}

void Sync_Thread()
{
  //sim_time
  sim_time = sc_simulation_time();

  if(!rst)
    {
      /*--- BANDWIDTH LIMITER SYNC ---*/
      BandwidthLimiterSync();
      
      /*--- TRANSACTION MANAGEMENT FSM ---*/
      Transaction_Management_FSM();
      
      /*--- LOCK MANAGEMENT BLOCK ---*/
      Lock_Management_Block();
      
      /*--- CONTROL UNIT---*/
      Control_Unit_FSM(p_req_aux2);
      
      /*--- ADDRESS DECODING STAGE GRANT FEEDBACK ---*/
      Address_Decoder_GNT_FeedBack();
      
      /*--- ORDERING BLOCK STAGE GRANT FEEDBACK ---*/
      Ordering_Block_GNT_FeedBack();
      
      if(initRetime == 1)
	/*--- INIT RETIMING STAGE  GRANT FEEDBACK ---*/
	Init_Retiming_GNT_FeedBack();
      
      if(targRetime == 1)
	/*--- TARGET RETIMING STAGE GRANT FEEDBACK ---*/
	Target_Retiming_GNT_FeedBack();
      
      /*--- TRACKER ---*/
      Tracker_FSM();
      
      /*--- RESPONSE TRANSACTION MANAGEMENT FSM ---*/
      Response_Transaction_Management_FSM();
      
      /*--- RESPONSE LOCK MANAGEMENT BLOCK ---*/
      Response_Lock_Management_Block();
      
      /*--- PRIORITY MANAGEMENT UNIT---*/
      Priority_Management_Unit_FSM();
      
      /*--- RESPONSE SOURCE DECODING STAGE RETURN GRANT FEEDBACK ---*/
      Response_Source_Decoder_R_GNT_FeedBack();
      
      /*--- ORDERING BLOCK STAGE RETURN GRANT FEEDBACK ---*/
      Ordering_Block_R_GNT_FeedBack();
      
      if(targReturnRetime == 1)
	/*--- TARGET RETURN RETIMING STAGE RETURN GRANT FEEDBACK ---*/
	Target_Return_Retiming_R_GNT_FeedBack();
      
      if(initReturnRetime == 1)
	/*--- INIT RETURN RETIMING STAGE GRANT FEEDBACK ---*/
	Init_Return_Retiming_R_GNT_FeedBack();
      
      if(postedValidOn == 1)
	/*--- WRITE POSTING BLOCK R_GNT FEEDBACK---*/
	Write_Posting_Block_R_GNT_FeedBack();
      
      if (programmingOn == 1)
	/*--- PROGRAMMING PORT ---*/
	Programming_port_Handler();
    }
}

};
	
#endif		
