/* Traffic Generators */
if(CURRENT_ISS == LX){
i=0;
switch(i)
{

	case 0:
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init0_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init0_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init0_lx->clock(ClockGen_1);
          
	  init0_lx->reset(ResetGen_1);
          init0_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  
	  /*init0_lx->clk(ClockGen_1);
	  init0_lx->rst(ResetGen_1);
	  init0_lx->Out(initiator_to_node[0]);*/
     
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 1:
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init1_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 1,true, INI_DEBUG > (buffer,8);
	 if (FREQSCALING || FREQSCALINGDEVICE)
            init1_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init1_lx->clock(ClockGen_1);
          
	  init1_lx->reset(ResetGen_1);
          init1_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);

        if(i==N_MASTERS-1) break;
	else i++;
	
	case 2:
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init2_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init2_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init2_lx->clock(ClockGen_1);
          
	  init2_lx->reset(ResetGen_1);
          init2_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);

        if(i==N_MASTERS-1) break;
	else i++;        
	
	case 3:
	sprintf(buffer, "LX_AHB_Master_%d", i);
	init3_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init3_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init3_lx->clock(ClockGen_1);
          
	  init3_lx->reset(ResetGen_1);
          init3_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
     
	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 4:
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init4_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init4_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init4_lx->clock(ClockGen_1);
          
	  init4_lx->reset(ResetGen_1);
          init4_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);

	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 5:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init5_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG > (buffer,8);
	 if (FREQSCALING || FREQSCALINGDEVICE)
            init5_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init5_lx->clock(ClockGen_1);
          
	  init5_lx->reset(ResetGen_1);
          init5_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  
	if(i==N_MASTERS-1) break;
	else i++;        	
	
	case 6:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init6_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG > (buffer,8);
	 if (FREQSCALING || FREQSCALINGDEVICE)
            init6_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init6_lx->clock(ClockGen_1);
          
	  init6_lx->reset(ResetGen_1);
          init6_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  
	if(i==N_MASTERS-1) break;
	else i++;   

	case 7:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init7_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init7_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init7_lx->clock(ClockGen_1);
          
	  init7_lx->reset(ResetGen_1);
          init7_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 8:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init8_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG > (buffer,8);
	 if (FREQSCALING || FREQSCALINGDEVICE)
            init8_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init8_lx->clock(ClockGen_1);
          
	  init8_lx->reset(ResetGen_1);
          init8_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 9:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init9_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init9_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init9_lx->clock(ClockGen_1);
          
	  init9_lx->reset(ResetGen_1);
          init9_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 10:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init10_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init10_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init10_lx->clock(ClockGen_1);
          
	  init10_lx->reset(ResetGen_1);
          init10_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 11:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init11_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init11_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init11_lx->clock(ClockGen_1);
          
	  init11_lx->reset(ResetGen_1);
          init11_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 12:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init12_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init12_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init12_lx->clock(ClockGen_1);
          
	  init12_lx->reset(ResetGen_1);
          init12_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 13:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init13_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init13_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init13_lx->clock(ClockGen_1);
          
	  init13_lx->reset(ResetGen_1);
          init13_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 14:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init14_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init14_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init14_lx->clock(ClockGen_1);
          
	  init14_lx->reset(ResetGen_1);
          init14_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 15:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init15_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init15_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init15_lx->clock(ClockGen_1);
          
	  init15_lx->reset(ResetGen_1);
          init15_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 16:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init16_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16,16, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init16_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init16_lx->clock(ClockGen_1);
          
	  init16_lx->reset(ResetGen_1);
          init16_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 17:  
	sprintf(buffer, "LX_AHB_Master_%d", i);
        init17_lx = new AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > (buffer,8);
	  if (FREQSCALING || FREQSCALINGDEVICE)
            init17_lx->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init17_lx->clock(ClockGen_1);
          
	  init17_lx->reset(ResetGen_1);
          init17_lx->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	
	printf("ERROR: you cannot simulate this numbers of cores:%d\n",N_CORES);
	exit(1); 
	  
}	  
}

