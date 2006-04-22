// Master_ahb.cpp: source file

#include "Master_ahb_syn.h"
//#define PRINTDEBUG
//#define EXTRAMPRINTDEBUG
//#define WRITE_BURST_DEBUG
//#define READ_DEBUG

Master_ahb::Master_ahb(sc_module_name name_, int MasterID, int Priority)
:sc_module(name_),
 p (name(), MasterID, Priority, false)

  {

   SC_THREAD( working );
   sensitive << hclk.pos();
   
   SC_THREAD( write_buffer );
   sensitive << hclk.pos();
   dont_initialize();
  }
  

void Master_ahb::working()
{
  uint burst=0, i, n=0,size,beat=0,m=0,passed=0;
  PINOUT mast_pinout;
  ahb_namespace::ahb_hresp my_status;
  Stato stato;
  unsigned int dato;
  
bool wr=false;
bool reset=false;

PRINT=false;
my = p.master_id()-1;
stato = ATTESA;
ready_to_wrapper.write(false);

while(true){

switch (stato){

case ATTESA:    
		for(i=0;i<16;i++)
		{
		_data[i]=0xdeadbeef;
		}
		reset=false;
		beat=0;
		passed=0;
		n = n+1;
		
		if(my==0 && n==923)
		PRINT=true;
	
		#ifdef PRINTDEBUG
		if(my==0){
		printf("Master%d %d \n",my,n);
		//cout << " at " << sc_time_stamp() << endl;
		}
		#endif
		
		do{
		wait();
		}while(request_from_wrapper == false);
      
		mast_pinout = pinout.read();
		
		switch (mast_pinout.bw)
			{
			case 0 :  _hsize = ahb_namespace::SIZE_32;
				  size = 0x4;
				  break;
			case 1 :  _hsize = ahb_namespace::SIZE_8;
				  size = 0x1;
				  break;
			case 2 :  _hsize = ahb_namespace::SIZE_16;
				  size = 0x2;
				  break;
			default : printf("Fatal error: Master detected a malformed data size at time %10.1f\n",
				sc_simulation_time());
			exit(0);
			}

		wr = mast_pinout.rw;  
     
		if(wr)
		_WR=WR;
		else
		_WR=RD;
		
		address = (unsigned int) addresser->Logical2Physical(mast_pinout.address, my);
                #ifdef PRINTDEBUG
		if(my==0)   
		printf("Master%d: addresser->address = %x\n",my,address);
		#endif
		burst = mast_pinout.burst;
		if (burst == 1)
		_burst_mode = ahb_namespace::SINGLE;
		if (burst == 4)
		_burst_mode = ahb_namespace::INCR4;
		if (burst == 8)
		_burst_mode = ahb_namespace::INCR8;
		if (burst == 16)
		_burst_mode = ahb_namespace::INCR16;
		
		

case RICHIESTA: 
		
		stato=TRANSA;
		if (STATS)
        statobject->requestsAccess(my);
	
		
		
		p.request();
		#ifdef EXTRAMPRINTDEBUG
		if(PRINT)
		printf("Master%d: Faccio richiesta all'arbitro\n",my);
		#endif
		while(!(p.has_grant() && p.response(my_status)))
			{
			wait();}

		#ifdef EXTRAMPRINTDEBUG
		if(PRINT)
		printf("Master%d: ho avuto il grant\n",my);
		#endif
		
		if (STATS)
		statobject->beginsAccess(2, !wr, burst, my);
		
		_address=(beat*(1<<_hsize))+address;
		#ifdef PRINTDEBUG
		if(PRINT)
		printf("Master%d: address = %x\n",my,address);
		#endif
		
		if(reset){
		#ifdef PRINTDEBUG
		if(PRINT)
		printf("Master%d: reset--->_burst_mode = ahb_namespace::INCR\n",my);
		#endif
		_burst_mode = ahb_namespace::INCR;}

		p.init_transaction(_WR, _data, _address , _burst_mode, _hsize);
		#ifdef PRINTDEBUG
		if(PRINT){
		printf("Master%d: _WR = %x; _burst_mode = %x;  _address = %x;\n",my,_WR,_burst_mode,_address);
		//cout << " at " << sc_time_stamp().to_seconds() << endl;
		}
		#endif
		wait();
                

		
		while(!p.response(my_status))
			{
			if(my_status==1){
			printf("Master%d: ERROR 1\n",my);
			printf("Master%d: tentato accesso a %x\n",my,_address);
			exit(1);
			}
			if(my_status==3){
			#ifdef EXTRAMPRINTDEBUG
			if(PRINT)
			printf("Master%d: SPLIT 2\n",my);
			#endif
			if(beat>0){
			#ifdef EXTRAMPRINTDEBUG
			if(PRINT)
			printf("Master%d: beat>0---> RESET 2\n",my);
			if(PRINT)
			printf("Master%d: passed = %x\n",my,passed);
			#endif
			reset=true;}
			stato=RICHIESTA;
			p.end_request();
			p.idle();
			wait();
			break;
			}
			wait();
			}
			
			
		if(stato==RICHIESTA){
		if (STATS)
		statobject->endsAccess(!wr, burst, my);
		wait();
		if (STATS)
		statobject->busFreed(my);
		break;}
		
		stato=TRANSA;

   
case TRANSA:    
		for (i = 0; i < burst-passed; i++)
		{
		if(wr==1 && burst>1){
		#ifdef WRITE_BURST_DEBUG
		if(my==1)
		printf("Master%d : data[%x]=buffer[%x]=%x\n",my,i,i,buffer[i]);
		#endif
		if(reset){
		m=i+passed;
		dato=buffer[m];	
		_data[i]=dato;}
		else{
		_data[i]=buffer[i];}
		}
		#ifdef WRITE_BURST_DEBUG
		if(my==1){
		printf("Master%d : set_data() data[%x] = %x\n",my,i,_data[i]);
		//cout << " at " << sc_time_stamp().to_seconds() << endl;
		}
		#endif

		p.set_data();
		if (i==(burst-1-passed)){ 
		#ifdef WRITE_BURST_DEBUG
		if(my==1){
		printf("Master%d : i==(burst-1-passed)-> idle\n",my);
		//cout << " at " << sc_time_stamp().to_seconds() << endl;
		}
		#endif
		p.idle();
		p.end_request();
		}
		wait();
				
		ready_to_wrapper.write(false);
	
		while(!p.response(my_status))
			{
			if(my_status==1){
			printf("Master%d: ERROR 2\n",my);
			printf("Master%d: tentato accesso a %x\n",my,_address);
			exit(1);
			}
			if(my_status==3){
			passed=beat;
			i=0;
			#ifdef EXTRAMPRINTDEBUG
			if(PRINT)
			printf("Master%d: SPLIT 3\n",my);
			#endif
			if(beat>0){
			#ifdef EXTRAMPRINTDEBUG
			if(PRINT)
			printf("Master%d: beat>0 ---> RESET 3\n",my);
			if(PRINT)
			printf("Master%d: passed = %x\n",my,passed);
			#endif
			reset=true;
			}
			stato=RICHIESTA;
			p.idle();
			p.end_request();
			wait();
			break;
			}
		#ifdef WRITE_BURST_DEBUG
		if(my==1){
		printf("Master%d : response?\n",my);
		//cout << " at " << sc_time_stamp().to_seconds() << endl;
		}
		#endif
			wait();
			}
		
		if(stato==RICHIESTA){
		if (STATS)
		statobject->endsAccess(!wr, burst, my);
		wait();
		if (STATS)
		statobject->busFreed(my);
		break;}
		
	if(!wr){
		mast_pinout.data = _data[i];
		#ifdef READ_DEBUG
		if(my==0){
		printf("Master%d: Ho letto data[%x] = %x\n",my,i+passed,_data[i]);
		//cout << " at " << sc_time_stamp().to_seconds() << endl;
		}
		#endif
		pinout.write(mast_pinout);
		ready_to_wrapper.write(true);
	}
	else
	{
	#ifdef WRITE_BURST_DEBUG
	if(my==1){
	printf("Master%d: Ho scritto data[%x] = %x\n",my,i,_data[i]);
	//cout << " at " << sc_time_stamp().to_seconds() << endl;
	}
	#endif
	}
		
		beat=beat+1;
		
		if (STATS)
		    if (i==burst-1)
		      statobject->endsAccess(!wr, burst, my);
		
	}
	
		if(stato==RICHIESTA){
		break;}
    
 case FINISH:  
		stato=ATTESA;
		ready_to_wrapper.write(true);
		wait();
		ready_to_wrapper.write(false);
		
		
		if (STATS)
		statobject->busFreed(my);

}
}
}


void Master_ahb::write_buffer()
{
bool wr;
uint burst=0, n;
PINOUT mast_pinout;

#ifdef PRINTDEBUG
if(PRINT)
printf("write_buffer()\n");
#endif
ready_to_wrapper.write(false);

while(true)
  {
      do{
      wait();
      }while(request_from_wrapper == false);
      mast_pinout = pinout.read();
      wr = mast_pinout.rw;
      burst = mast_pinout.burst;
      
      if(wr)//write
      {
      if (burst == 1)//------------------------Single write transfer
      {
      _data[0] = mast_pinout.data;
      }
      else  //------------------------- Burst write transfer
    {
    #ifdef WRITE_BURST_DEBUG
    if(my==1)
    printf("master%d Burst write\n",my);
    #endif
      for(n=0;n<16;n++){
      buffer[n]=0;
       }

    ready_to_wrapper.write(true);
    wait();
    for(n=0;n<burst;n++){
    mast_pinout = pinout.read();
    buffer[n] = mast_pinout.data;
    #ifdef WRITE_BURST_DEBUG
    if(my==1)
    printf("write_buffer=>buffer[%x]=%x\n",n,buffer[n]);    
    #endif
    if(n==burst-2)
    ready_to_wrapper.write(false);
    wait();
    }
    ready_to_wrapper.write(false);
    }
      }
    
}
}



