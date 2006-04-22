/* Traffic Generators */
i=0;
switch(i)
{

	case 0:
	sprintf(buffer, "initiator_lx_%d", i);
        init0_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init0_lx->clk(interconnect_clock[0]);
           else
	  init0_lx->clk(ClockGen_1);
	  init0_lx->rst(ResetGen_1);
	  init0_lx->Out(initiator_to_node[0]);
     
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 1:
	sprintf(buffer, "initiator_lx_%d", i);
        init1_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 1,true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init1_lx->clk(interconnect_clock[0]);
           else
	  init1_lx->clk(ClockGen_1);
	  init1_lx->rst(ResetGen_1);
	  init1_lx->Out(initiator_to_node[1]);

        if(i==N_MASTERS-1) break;
	else i++;
	
	case 2:
	sprintf(buffer, "initiator_lx_%d", i);
        init2_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init2_lx->clk(interconnect_clock[0]);
           else
	  init2_lx->clk(ClockGen_1);
	  init2_lx->rst(ResetGen_1);
	  init2_lx->Out(initiator_to_node[2]);

        if(i==N_MASTERS-1) break;
	else i++;        
	
	case 3:
	sprintf(buffer, "initiator_lx_%d", i);
	init3_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init3_lx->clk(interconnect_clock[0]);
           else
	  init3_lx->clk(ClockGen_1);
	  init3_lx->rst(ResetGen_1);
	  init3_lx->Out(initiator_to_node[3]);
     
	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 4:
	sprintf(buffer, "initiator_lx_%d", i);
        init4_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init4_lx->clk(interconnect_clock[0]);
           else
	  init4_lx->clk(ClockGen_1);
	  init4_lx->rst(ResetGen_1);
	  init4_lx->Out(initiator_to_node[4]);

	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 5:  
	sprintf(buffer, "initiator_lx_%d", i);
        init5_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init5_lx->clk(interconnect_clock[0]);
           else
	  init5_lx->clk(ClockGen_1);
	  init5_lx->rst(ResetGen_1);
	  init5_lx->Out(initiator_to_node[5]);

	if(i==N_MASTERS-1) break;
	else i++;        	
	
	case 6:  
	sprintf(buffer, "initiator_lx_%d", i);
        init6_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init6_lx->clk(interconnect_clock[0]);
           else
	  init6_lx->clk(ClockGen_1);
	  init6_lx->rst(ResetGen_1);
	  init6_lx->Out(initiator_to_node[6]);
	  
	if(i==N_MASTERS-1) break;
	else i++;   

	case 7:  
	sprintf(buffer, "initiator_lx_%d", i);
        init7_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init7_lx->clk(interconnect_clock[0]);
           else
	  init7_lx->clk(ClockGen_1);
	  init7_lx->rst(ResetGen_1);
	  init7_lx->Out(initiator_to_node[7]);
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 8:  
	sprintf(buffer, "initiator_lx_%d", i);
        init8_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init8_lx->clk(interconnect_clock[0]);
           else
	  init8_lx->clk(ClockGen_1);
	  init8_lx->rst(ResetGen_1);
	  init8_lx->Out(initiator_to_node[8]);
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 9:  
	sprintf(buffer, "initiator_lx_%d", i);
        init9_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init9_lx->clk(interconnect_clock[0]);
           else
	  init9_lx->clk(ClockGen_1);
	  init9_lx->rst(ResetGen_1);
	  init9_lx->Out(initiator_to_node[9]);
	
	if(i==N_MASTERS-1) break;
	else i++; 
	
	case 10:  
	sprintf(buffer, "initiator_lx_%d", i);
        init10_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init10_lx->clk(interconnect_clock[0]);
           else
	  init10_lx->clk(ClockGen_1);
	  init10_lx->rst(ResetGen_1);
	  init10_lx->Out(initiator_to_node[10]); 
	
	if(i==N_MASTERS-1) break;
	else i++; 
		  
	case 11:  
	sprintf(buffer, "initiator_lx_%d", i);
        init11_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init11_lx->clk(interconnect_clock[0]);
           else
	  init11_lx->clk(ClockGen_1);
	  init11_lx->rst(ResetGen_1);
	  init11_lx->Out(initiator_to_node[11]); 
	
	if(i==N_MASTERS-1) break;
	else i++; 
		
	case 12:  
	sprintf(buffer, "initiator_lx_%d", i);
        init12_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init12_lx->clk(interconnect_clock[0]);
           else
	  init12_lx->clk(ClockGen_1);
	  init12_lx->rst(ResetGen_1);
	  init12_lx->Out(initiator_to_node[12]);  	
        
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 13:  
	sprintf(buffer, "initiator_lx_%d", i);
        init13_lx = new STBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > (buffer,8);
	   if (FREQSCALING || FREQSCALINGDEVICE)
          init13_lx->clk(interconnect_clock[0]);
           else
	  init13_lx->clk(ClockGen_1);
	  init13_lx->rst(ResetGen_1);
	  init13_lx->Out(initiator_to_node[13]);
	
	if(i==N_MASTERS-1) break;
	else i++; 
	 	
	printf("ERROR: you cannot simulate this numbers of cores:%d\n",N_CORES);
	exit(1); 
	  
}	  
