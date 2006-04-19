
if(CURRENT_ISS == LX){
#ifdef AXIDEBUG
    DW_AXI_Monitor<32 > *AXI_MonitorMaster[N_MASTERS];
#endif

i=0;
switch(i)
{

	case 0:
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init0_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
        #ifdef AXIDEBUG
         sprintf(buffer2,"AXI_MonitorMaster%d" , i);
         sprintf(buffer3,"AXI_Master%d.txt" , i);
         AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
        #endif
          if ((FREQSCALING && M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding]) || FREQSCALINGDEVICE)
            init0_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init0_lx->Aclk(ClockGen_1);
          
	  init0_lx->Aresetn(Aresetn);
          #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init0_lx);
           init0_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init0_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init0_lx);
          #endif
          
     
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 1:
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init1_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 1,true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	 #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
         if (FREQSCALING || FREQSCALINGDEVICE)
            init1_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init1_lx->Aclk(ClockGen_1);
          
	  init1_lx->Aresetn(Aresetn);
          #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init1_lx);
           init1_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init1_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init1_lx);
          #endif

        if(i==N_MASTERS-1) break;
	else i++;
	
	case 2:
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init2_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init2_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init2_lx->Aclk(ClockGen_1);
          
	  init2_lx->Aresetn(Aresetn);
          #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init2_lx);
           init2_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init2_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init2_lx);
          #endif

        if(i==N_MASTERS-1) break;
	else i++;        
	
	case 3:
	sprintf(buffer, "LX_AXI_Master_%d", i);
	init3_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init3_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init3_lx->Aclk(ClockGen_1);
          
	  init3_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init3_lx);
           init3_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init3_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init3_lx);
          #endif
     
	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 4:
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init4_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init4_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init4_lx->Aclk(ClockGen_1);
          
	  init4_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init4_lx);
           init4_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init4_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init4_lx);
          #endif

	if(i==N_MASTERS-1) break;
	else i++;        
	
	case 5:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init5_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	 #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
         if (FREQSCALING || FREQSCALINGDEVICE)
            init5_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init5_lx->Aclk(ClockGen_1);
          
	  init5_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init5_lx);
           init5_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init5_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init5_lx);
          #endif
	  
	if(i==N_MASTERS-1) break;
	else i++;        	
	
	case 6:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init6_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	 #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
         if (FREQSCALING || FREQSCALINGDEVICE)
            init6_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init6_lx->Aclk(ClockGen_1);
          
	  init6_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init6_lx);
           init6_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init6_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init6_lx);
          #endif
	  
	if(i==N_MASTERS-1) break;
	else i++;   

	case 7:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init7_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init7_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init7_lx->Aclk(ClockGen_1);
          
	  init7_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init7_lx);
           init7_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init7_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init7_lx);
          #endif
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 8:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init8_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	 #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
         if (FREQSCALING || FREQSCALINGDEVICE)
            init8_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init8_lx->Aclk(ClockGen_1);
          
	  init8_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init8_lx);
           init8_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init8_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init8_lx);
          #endif
	  
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 9:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init9_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init9_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init9_lx->Aclk(ClockGen_1);
          
	  init9_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init9_lx);
           init9_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init9_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init9_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 10:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init10_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init10_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init10_lx->Aclk(ClockGen_1);
          
	  init10_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init10_lx);
           init10_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init10_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init10_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 11:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init11_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init11_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init11_lx->Aclk(ClockGen_1);
          
	  init11_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init11_lx);
           init11_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init11_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init11_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 12:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init12_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init12_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init12_lx->Aclk(ClockGen_1);
          
	  init12_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init12_lx);
           init12_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init12_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init12_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 13:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init13_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init13_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init13_lx->Aclk(ClockGen_1);
          
	  init13_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init13_lx);
           init13_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init13_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init13_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 14:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init14_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init14_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init14_lx->Aclk(ClockGen_1);
          
	  init14_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init14_lx);
           init14_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init14_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init14_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 15:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init15_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init15_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init15_lx->Aclk(ClockGen_1);
          
	  init15_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init15_lx);
           init15_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init15_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init15_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 16:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init16_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16,16, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init16_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init16_lx->Aclk(ClockGen_1);
          
	  init16_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init16_lx);
           init16_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init16_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init16_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	case 17:  
	sprintf(buffer, "LX_AXI_Master_%d", i);
        init17_lx = new AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > (buffer,8,1, DataBusBytes, ClockFrequency);
	  #ifdef AXIDEBUG
          sprintf(buffer2,"AXI_MonitorMaster%d" , i);
          sprintf(buffer3,"AXI_Master%d.txt" , i);
          AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
         #endif
          if (FREQSCALING || FREQSCALINGDEVICE)
            init17_lx->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
          else
            init17_lx->Aclk(ClockGen_1);
          
	  init17_lx->Aresetn(Aresetn);
                    #ifdef AXIDEBUG
           AXI_MonitorMaster[i]->SlaveP(*init17_lx);
           init17_lx->MasterP(*AXI_MonitorMaster[i]);
           AXI_MonitorMaster[i]->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
          #else
           init17_lx->MasterP(*AXI_IC);
           AXI_IC->SlaveP(*init17_lx);
          #endif
	  	
	if(i==N_MASTERS-1) break;
	else i++;
	
	
	printf("ERROR: you cannot simulate this numbers of cores:%d\n",N_CORES);
	exit(1); 
	  
}
}


