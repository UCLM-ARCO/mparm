/* Traffic Generators */
i=1;
switch(i)
{

	case 1:
        init0 = new STBus_initiator< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG > ("init0");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init0->clk(interconnect_clock[0]);
        else
	  init0->clk(ClockGen_1);
	  init0->rst(ResetGen_1);
	  init0->Out(initiator_to_node[0]);
          if ((FREQSCALING && M_DIVIDER[0] != I_DIVIDER[MASTER_CONFIG[0].binding]) || FREQSCALINGDEVICE)
          {
            init0->pinout(pinoutmast_interconnect[0]);
            init0->ready_to_wrapper(readymast_interconnect[0]);
            init0->request_from_wrapper(requestmast_interconnect[0]);
          }
          else
          {
            init0->pinout(pinoutmast[0]);
            init0->ready_to_wrapper(readymast[0]);
            init0->request_from_wrapper(requestmast[0]);
          }
        if(i==N_MASTERS) break;
	else i++;
	
	case 2:
        init1 = new STBus_initiator< uint, uint, uchar, 32, 16, 1, true, INI_DEBUG > ("init1");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init1->clk(interconnect_clock[0]);
        else
	  init1->clk(ClockGen_1);
	  init1->rst(ResetGen_1);
	  init1->Out(initiator_to_node[1]);
          if ((FREQSCALING && M_DIVIDER[1] != I_DIVIDER[MASTER_CONFIG[1].binding]) || FREQSCALINGDEVICE)
          {
            init1->pinout(pinoutmast_interconnect[1]);
            init1->ready_to_wrapper(readymast_interconnect[1]);
            init1->request_from_wrapper(requestmast_interconnect[1]);
          }
          else
          {
            init1->pinout(pinoutmast[1]);
            init1->ready_to_wrapper(readymast[1]);
            init1->request_from_wrapper(requestmast[1]);
          }
        if(i==N_MASTERS) break;
	else i++;
	
	case 3:
        init2 = new STBus_initiator< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG > ("init2");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init2->clk(interconnect_clock[0]);
        else
	  init2->clk(ClockGen_1);
	  init2->rst(ResetGen_1);
	  init2->Out(initiator_to_node[2]);
          if ((FREQSCALING && M_DIVIDER[2] != I_DIVIDER[MASTER_CONFIG[2].binding]) || FREQSCALINGDEVICE)
          {
            init2->pinout(pinoutmast_interconnect[2]);
            init2->ready_to_wrapper(readymast_interconnect[2]);
            init2->request_from_wrapper(requestmast_interconnect[2]);
          }
          else
          {
            init2->pinout(pinoutmast[2]);
            init2->ready_to_wrapper(readymast[2]);
            init2->request_from_wrapper(requestmast[2]);
          }
        if(i==N_MASTERS) break;
	else i++;        
	
	case 4:
	init3 = new STBus_initiator< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG > ("init3");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init3->clk(interconnect_clock[0]);
        else
	  init3->clk(ClockGen_1);
	  init3->rst(ResetGen_1);
	  init3->Out(initiator_to_node[3]);
          if ((FREQSCALING && M_DIVIDER[3] != I_DIVIDER[MASTER_CONFIG[3].binding]) || FREQSCALINGDEVICE)
          {
            init3->pinout(pinoutmast_interconnect[3]);
            init3->ready_to_wrapper(readymast_interconnect[3]);
            init3->request_from_wrapper(requestmast_interconnect[3]);
          }
          else
          {
            init3->pinout(pinoutmast[3]);
            init3->ready_to_wrapper(readymast[3]);
            init3->request_from_wrapper(requestmast[3]);
          }  
	if(i==N_MASTERS) break;
	else i++;        
	
	case 5:
        init4 = new STBus_initiator< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG > ("init4");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init4->clk(interconnect_clock[0]);
        else
	  init4->clk(ClockGen_1);
	  init4->rst(ResetGen_1);
	  init4->Out(initiator_to_node[4]);
          if ((FREQSCALING && M_DIVIDER[4] != I_DIVIDER[MASTER_CONFIG[4].binding]) || FREQSCALINGDEVICE)
          {
            init4->pinout(pinoutmast_interconnect[4]);
            init4->ready_to_wrapper(readymast_interconnect[4]);
            init4->request_from_wrapper(requestmast_interconnect[4]);
          }
          else
          {
            init4->pinout(pinoutmast[4]);
            init4->ready_to_wrapper(readymast[4]);
            init4->request_from_wrapper(requestmast[4]);
          }
	if(i==N_MASTERS) break;
	else i++;        
	
	case 6:  
        init5 = new STBus_initiator< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG > ("init5");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init5->clk(interconnect_clock[0]);
        else
	  init5->clk(ClockGen_1);
	  init5->rst(ResetGen_1);
	  init5->Out(initiator_to_node[5]);
          if ((FREQSCALING && M_DIVIDER[5] != I_DIVIDER[MASTER_CONFIG[5].binding]) || FREQSCALINGDEVICE)
          {
            init5->pinout(pinoutmast_interconnect[5]);
            init5->ready_to_wrapper(readymast_interconnect[5]);
            init5->request_from_wrapper(requestmast_interconnect[5]);
          }
          else
          {
            init5->pinout(pinoutmast[5]);
            init5->ready_to_wrapper(readymast[5]);
            init5->request_from_wrapper(requestmast[5]);
          }
	if(i==N_MASTERS) break;
	else i++;        
	
	case 7: 
        init6 = new STBus_initiator< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG > ("init6");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init6->clk(interconnect_clock[0]);
        else
	  init6->clk(ClockGen_1);
	  init6->rst(ResetGen_1);
	  init6->Out(initiator_to_node[6]);
          if ((FREQSCALING && M_DIVIDER[6] != I_DIVIDER[MASTER_CONFIG[6].binding]) || FREQSCALINGDEVICE)
          {
            init6->pinout(pinoutmast_interconnect[6]);
            init6->ready_to_wrapper(readymast_interconnect[6]);
            init6->request_from_wrapper(requestmast_interconnect[6]);
          }
          else
          {
            init6->pinout(pinoutmast[6]);
            init6->ready_to_wrapper(readymast[6]);
            init6->request_from_wrapper(requestmast[6]);
          }
	if(i==N_MASTERS) break;
	else i++;        
	
	case 8:  
        init7 = new STBus_initiator< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG > ("init7");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init7->clk(interconnect_clock[0]);
        else
	  init7->clk(ClockGen_1);
	  init7->rst(ResetGen_1);
	  init7->Out(initiator_to_node[7]);
          if ((FREQSCALING && M_DIVIDER[7] != I_DIVIDER[MASTER_CONFIG[7].binding]) || FREQSCALINGDEVICE)
          {
            init7->pinout(pinoutmast_interconnect[7]);
            init7->ready_to_wrapper(readymast_interconnect[7]);
            init7->request_from_wrapper(requestmast_interconnect[7]);
          }
          else
          {
            init7->pinout(pinoutmast[7]);
            init7->ready_to_wrapper(readymast[7]);
            init7->request_from_wrapper(requestmast[7]);
          }
	if(i==N_MASTERS) break;
	else i++;        
	
	case 9:  
        init8 = new STBus_initiator< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG > ("init8");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init8->clk(interconnect_clock[0]);
        else
	  init8->clk(ClockGen_1);
	  init8->rst(ResetGen_1);
	  init8->Out(initiator_to_node[8]);
          if ((FREQSCALING && M_DIVIDER[8] != I_DIVIDER[MASTER_CONFIG[8].binding]) || FREQSCALINGDEVICE)
          {
            init8->pinout(pinoutmast_interconnect[8]);
            init8->ready_to_wrapper(readymast_interconnect[8]);
            init8->request_from_wrapper(requestmast_interconnect[8]);
          }
          else
          {
            init8->pinout(pinoutmast[8]);
            init8->ready_to_wrapper(readymast[8]);
            init8->request_from_wrapper(requestmast[8]);
          }
	if(i==N_MASTERS) break;
	else i++;        
	
	case 10:  
        init9 = new STBus_initiator< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG > ("init9");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init9->clk(interconnect_clock[0]);
        else
	  init9->clk(ClockGen_1);
	  init9->rst(ResetGen_1);
	  init9->Out(initiator_to_node[9]);
          if ((FREQSCALING && M_DIVIDER[9] != I_DIVIDER[MASTER_CONFIG[9].binding]) || FREQSCALINGDEVICE)
          {
            init9->pinout(pinoutmast_interconnect[9]);
            init9->ready_to_wrapper(readymast_interconnect[9]);
            init9->request_from_wrapper(requestmast_interconnect[9]);
          }
          else
          {
            init9->pinout(pinoutmast[9]);
            init9->ready_to_wrapper(readymast[9]);
            init9->request_from_wrapper(requestmast[9]);
          }
	if(i==N_MASTERS) break;
	else i++;
	
	case 11:  
        init10 = new STBus_initiator< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > ("init10");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init10->clk(interconnect_clock[0]);
        else
	  init10->clk(ClockGen_1);
	  init10->rst(ResetGen_1);
	  init10->Out(initiator_to_node[10]);
          if ((FREQSCALING && M_DIVIDER[10] != I_DIVIDER[MASTER_CONFIG[10].binding]) || FREQSCALINGDEVICE)
          {
            init10->pinout(pinoutmast_interconnect[10]);
            init10->ready_to_wrapper(readymast_interconnect[10]);
            init10->request_from_wrapper(requestmast_interconnect[10]);
          }
          else
          {
            init10->pinout(pinoutmast[10]);
            init10->ready_to_wrapper(readymast[10]);
            init10->request_from_wrapper(requestmast[10]);
          }
	if(i==N_MASTERS) break;
	else i++;
	
	case 12:  
        init11 = new STBus_initiator< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > ("init11");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init11->clk(interconnect_clock[0]);
        else
	  init11->clk(ClockGen_1);
	  init11->rst(ResetGen_1);
	  init11->Out(initiator_to_node[11]);
          if ((FREQSCALING && M_DIVIDER[11] != I_DIVIDER[MASTER_CONFIG[11].binding]) || FREQSCALINGDEVICE)
          {
            init11->pinout(pinoutmast_interconnect[11]);
            init11->ready_to_wrapper(readymast_interconnect[11]);
            init11->request_from_wrapper(requestmast_interconnect[11]);
          }
          else
          {
            init11->pinout(pinoutmast[11]);
            init11->ready_to_wrapper(readymast[11]);
            init11->request_from_wrapper(requestmast[11]);
          }
	if(i==N_MASTERS) break;
	else i++;
	
	case 13:  
        init12 = new STBus_initiator< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > ("init12");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init12->clk(interconnect_clock[0]);
        else
	  init12->clk(ClockGen_1);
	  init12->rst(ResetGen_1);
	  init12->Out(initiator_to_node[12]);
          if ((FREQSCALING && M_DIVIDER[12] != I_DIVIDER[MASTER_CONFIG[12].binding]) || FREQSCALINGDEVICE)
          {
            init12->pinout(pinoutmast_interconnect[12]);
            init12->ready_to_wrapper(readymast_interconnect[12]);
            init12->request_from_wrapper(requestmast_interconnect[12]);
          }
          else
          {
            init12->pinout(pinoutmast[12]);
            init12->ready_to_wrapper(readymast[12]);
            init12->request_from_wrapper(requestmast[12]);
          }
	if(i==N_MASTERS) break;
	else i++;

	case 14:  
        init13 = new STBus_initiator< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > ("init13");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init13->clk(interconnect_clock[0]);
        else
	  init13->clk(ClockGen_1);
	  init13->rst(ResetGen_1);
	  init13->Out(initiator_to_node[13]);
          if ((FREQSCALING && M_DIVIDER[13] != I_DIVIDER[MASTER_CONFIG[13].binding]) || FREQSCALINGDEVICE)
          {
            init13->pinout(pinoutmast_interconnect[13]);
            init13->ready_to_wrapper(readymast_interconnect[13]);
            init13->request_from_wrapper(requestmast_interconnect[13]);
          }
          else
          {
            init13->pinout(pinoutmast[13]);
            init13->ready_to_wrapper(readymast[13]);
            init13->request_from_wrapper(requestmast[13]);
          }
	if(i==N_MASTERS) break;
	else i++;

	case 15:  
        init14 = new STBus_initiator< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > ("init14");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init14->clk(interconnect_clock[0]);
        else
	  init14->clk(ClockGen_1);
	  init14->rst(ResetGen_1);
	  init14->Out(initiator_to_node[14]);
          if ((FREQSCALING && M_DIVIDER[14] != I_DIVIDER[MASTER_CONFIG[14].binding]) || FREQSCALINGDEVICE)
          {
            init14->pinout(pinoutmast_interconnect[14]);
            init14->ready_to_wrapper(readymast_interconnect[14]);
            init14->request_from_wrapper(requestmast_interconnect[14]);
          }
          else
          {
            init14->pinout(pinoutmast[14]);
            init14->ready_to_wrapper(readymast[14]);
            init14->request_from_wrapper(requestmast[14]);
          }
	if(i==N_MASTERS) break;
	else i++;	

	case 16:  
        init15 = new STBus_initiator< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > ("init15");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init15->clk(interconnect_clock[0]);
        else
	  init15->clk(ClockGen_1);
	  init15->rst(ResetGen_1);
	  init15->Out(initiator_to_node[15]);
          if ((FREQSCALING && M_DIVIDER[15] != I_DIVIDER[MASTER_CONFIG[15].binding]) || FREQSCALINGDEVICE)
          {
            init15->pinout(pinoutmast_interconnect[15]);
            init15->ready_to_wrapper(readymast_interconnect[15]);
            init15->request_from_wrapper(requestmast_interconnect[15]);
          }
          else
          {
            init15->pinout(pinoutmast[15]);
            init15->ready_to_wrapper(readymast[15]);
            init15->request_from_wrapper(requestmast[15]);
          }
	if(i==N_MASTERS) break;
	else i++;	

	case 17:  
        init16 = new STBus_initiator< uint, uint, uchar, 32, 16, 16, true, INI_DEBUG > ("init16");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init16->clk(interconnect_clock[0]);
        else
	  init16->clk(ClockGen_1);
	  init16->rst(ResetGen_1);
	  init16->Out(initiator_to_node[16]);
          if ((FREQSCALING && M_DIVIDER[16] != I_DIVIDER[MASTER_CONFIG[16].binding]) || FREQSCALINGDEVICE)
          {
            init16->pinout(pinoutmast_interconnect[16]);
            init16->ready_to_wrapper(readymast_interconnect[16]);
            init16->request_from_wrapper(requestmast_interconnect[16]);
          }
          else
          {
            init16->pinout(pinoutmast[16]);
            init16->ready_to_wrapper(readymast[16]);
            init16->request_from_wrapper(requestmast[16]);
          }
	if(i==N_MASTERS) break;
	else i++;

	case 18:  
        init17 = new STBus_initiator< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > ("init17");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init17->clk(interconnect_clock[0]);
        else
	  init17->clk(ClockGen_1);
	  init17->rst(ResetGen_1);
	  init17->Out(initiator_to_node[17]);
          if ((FREQSCALING && M_DIVIDER[17] != I_DIVIDER[MASTER_CONFIG[17].binding]) || FREQSCALINGDEVICE)
          {
            init17->pinout(pinoutmast_interconnect[17]);
            init17->ready_to_wrapper(readymast_interconnect[17]);
            init17->request_from_wrapper(requestmast_interconnect[17]);
          }
          else
          {
            init17->pinout(pinoutmast[17]);
            init17->ready_to_wrapper(readymast[17]);
            init17->request_from_wrapper(requestmast[17]);
          }
	if(i==N_MASTERS) break;
	else i++;
	
	case 19:  
        init18 = new STBus_initiator< uint, uint, uchar, 32, 16, 18, true, INI_DEBUG > ("init18");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init18->clk(interconnect_clock[0]);
        else
	  init18->clk(ClockGen_1);
	  init18->rst(ResetGen_1);
	  init18->Out(initiator_to_node[18]);
          if ((FREQSCALING && M_DIVIDER[18] != I_DIVIDER[MASTER_CONFIG[18].binding]) || FREQSCALINGDEVICE)
          {
            init18->pinout(pinoutmast_interconnect[18]);
            init18->ready_to_wrapper(readymast_interconnect[18]);
            init18->request_from_wrapper(requestmast_interconnect[18]);
          }
          else
          {
            init18->pinout(pinoutmast[18]);
            init18->ready_to_wrapper(readymast[18]);
            init18->request_from_wrapper(requestmast[18]);
          }
	if(i==N_MASTERS) break;
	else i++;

	case 20:  
        init19 = new STBus_initiator< uint, uint, uchar, 32, 16, 19, true, INI_DEBUG > ("init19");
        if (FREQSCALING || FREQSCALINGDEVICE)
          init19->clk(interconnect_clock[0]);
        else
	  init19->clk(ClockGen_1);
	  init19->rst(ResetGen_1);
	  init19->Out(initiator_to_node[19]);
          if ((FREQSCALING && M_DIVIDER[19] != I_DIVIDER[MASTER_CONFIG[19].binding]) || FREQSCALINGDEVICE)
          {
            init19->pinout(pinoutmast_interconnect[19]);
            init19->ready_to_wrapper(readymast_interconnect[19]);
            init19->request_from_wrapper(requestmast_interconnect[19]);
          }
          else
          {
            init19->pinout(pinoutmast[19]);
            init19->ready_to_wrapper(readymast[19]);
            init19->request_from_wrapper(requestmast[19]);
          }
	if(i==N_MASTERS) break;
	else i++;
			
	printf("ERROR: you cannot simulate this numbers of cores:%d\n",N_CORES);
	exit(1); 
	  
}	  
