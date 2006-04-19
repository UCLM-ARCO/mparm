///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         sim_support.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Simulation support: memory-mapped user-app I/O, stats interaction
//
///////////////////////////////////////////////////////////////////////////////

#include "sim_support.h"

Sim_Support *simsuppobject;

Sim_Support::Sim_Support(int argc, char *argv[], char *envp[])
  : argc(argc), argv(argv), envp(envp)
{
  char dummystring[] = "";

  stopped_time     = new bool [N_CORES];
  stopped_cycle    = new bool [N_CORES];
  current_time     = new uint64_t [N_CORES];
  current_cycle    = new uint64_t [N_CORES];
  
  debug_msg_string = new char * [N_CORES];
  debug_msg_value  = new uint32_t [N_CORES];
  debug_msg_mode   = new uint32_t [N_CORES];
  debug_msg_id     = new uint32_t [N_CORES];
  
  file_read        = new bool [N_CORES];
  current_file     = new FILE *[N_CORES];
  file_window_valid_data     = new uint32_t [N_CORES];
  filebuffer       = new uint32_t *[N_CORES];
  filename         = new char *[N_CORES];
  
  for (uint8_t i = 0; i < N_CORES; i ++)
  {
    stopped_time[i]     = false;
    stopped_cycle[i]    = false;
    
    debug_msg_string[i] = dummystring;
    debug_msg_value[i]  = 0x0;
    debug_msg_mode[i]   = 0x0000001f;
    debug_msg_id[i]     = i;
    
    file_read[i]        = true;
    current_file[i]     = NULL;
    file_window_valid_data[i]     = 0;
    filebuffer[i]       = new uint32_t [FILE_WINDOW_SIZE];
  }
}

Sim_Support::~Sim_Support()
{
}

bool Sim_Support::catch_sim_message(uint32_t addr, uint32_t *data, bool write, uint8_t ID)
{
 unsigned int idx_mem1;
 int mem_id;

  ASSERT(ID < N_CORES);
  
  //printf("Called by %hu with addr 0x%08x data 0x%08x wr %d\n", ID, addr, data, write);
  
  uint32_t offset = 0x0;
  
  if (addr >= ARGV_ADDRESS && addr < ARGV_ADDRESS + 0x00010000)
  {
    offset = addr - ARGV_ADDRESS;
    addr = ARGV_ADDRESS;
  }
  if (addr >= ENVP_ADDRESS && addr < ENVP_ADDRESS + 0x00010000)
  {
    offset = addr - ENVP_ADDRESS;
    addr = ENVP_ADDRESS;
  }
  if (addr >= FILE_ADDRESS && addr < FILE_ADDRESS + (FILE_WINDOW_SIZE * 4))
  {
    offset = addr - FILE_ADDRESS;
    addr = FILE_ADDRESS;
  }
  
  switch (addr)
  {
    // Start statistics collection
    case START_METRIC_ADDRESS:
          statobject->startMeasuring(ID);
          return true;
          break;
    // Stop statistics collection
    case STOP_METRIC_ADDRESS:
          statobject->stopMeasuring(ID);
          return true;
          break;
    // Mark the end of the boot stage
    case ENDBOOT_ADDRESS:
          // FIXME this may not be what we want (check in stats)
          if (AUTOSTARTMEASURING)
          {
            statobject->stopMeasuring(ID);
            statobject->quit(ID);
          }
          return true;
          break;
    // Shutdown this processor
    case SHUTDOWN_ADDRESS:
          statobject->quit(ID);
          return true;
          break;
    // Dump system statistics
    case DUMP_ADDRESS:
          statobject->dump(ID);
          return true;
          break;
    // Dump system statistics (light version)
    case DUMP_LIGHT_ADDRESS:
          statobject->dump_light(ID);
          return true;
          break;
    // Clear system statistics
    case CLEAR_ADDRESS:
          statobject->clear();
          return true;
          break;
    // Get the ID of this CPU
    case GET_CPU_ID_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          // From 1 onwards
          *data = (uint32_t) (ID + 1);
          return true;
          break;
    // Get the total amount of CPUs in this system
    case GET_CPU_CNT_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          *data = (uint32_t) N_CORES;
          return true;
          break;
    // 
    case SET_REQ_IO_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          //TODO
          return true;
          break;
    // Get the current simulation time (32 LSBs)
    case GET_TIME_ADDRESS_LO:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (!stopped_time[ID])
          {
            printf("Fatal Error: Sim support module received a Get Time (LO) call by core %hu while the counter was not still\n", ID);
            exit(1);
          }
          *data = (uint32_t)(current_time[ID] & 0x00000000FFFFFFFF);
          return true;
          break;
    // Get the current simulation time (32 MSBs)
    case GET_TIME_ADDRESS_HI:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (!stopped_time[ID])
          {
            printf("Fatal Error: Sim support module received a Get Time (HI) call by core %hu while the counter was not still\n", ID);
            exit(1);
          }
          *data = (uint32_t)(current_time[ID] >> 32);
          return true;
          break;
    // Get the current simulation cycle (32 LSBs)
    case GET_CYCLE_ADDRESS_LO:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (stopped_cycle[ID])
          {
            printf("Fatal Error: Sim support module received a Get Cycle (LO) call by core %hu while the counter was not still\n", ID);
            exit(1);
          }
          *data = (uint32_t)(current_cycle[ID] & 0x00000000FFFFFFFF);
          return true;
          break;
    // Get the current simulation cycle (32 MSBs)
    case GET_CYCLE_ADDRESS_HI:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (!stopped_cycle[ID])
          {
            printf("Fatal Error: Sim support module received a Get Cycle (HI) call by core %hu while the counter was not still\n", ID);
            exit(1);
          }
          stopped_cycle[ID] = false;
          *data = (uint32_t)(current_cycle[ID] >> 32);
          return true;
          break;
    // Freeze the current simulation time for retrieval
    case STOP_TIME_ADDRESS:
          stopped_time[ID] = true;
          current_time[ID] = (uint64_t)(sc_simulation_time());
          return true;
          break;
    // Unfreeze the simulation time counter
    case RELEASE_TIME_ADDRESS:
          stopped_time[ID] = false;
          return true;
          break;
    // Freeze the current simulation cycle for retrieval
    case STOP_CYCLE_ADDRESS:
          stopped_cycle[ID] = true;
          current_cycle[ID] = (uint64_t)(sc_simulation_time() / (float)(CLOCKPERIOD * M_DIVIDER[ID]));
          return true;
          break;
    // Unfreeze the simulation cycle counter
    case RELEASE_CYCLE_ADDRESS:
          stopped_cycle[ID] = false;
          return true;
          break;
    // Print a debug message to console: set output string
    case DEBUG_MSG_STRING_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
	  
	  //debug_msg_string[ID] = (char *)(addresser->pMemoryDebug[ID] + (*data));
          
	   
	   mem_id= addresser->MapPhysicalToSlave(addresser->Logical2Physical((*data),ID));
	    if(addresser->IsInterrupt(mem_id) || mem_id==-1)
	     {printf("Trying to perform Dump on a non \"READABLE\" device\n");
	      exit(1);
	     }

	  idx_mem1=addresser->ReturnSlavePhysicalAddress(mem_id);
 
	  debug_msg_string[ID] = (char *)(addresser->pMemoryDebug[mem_id] + addresser->Logical2Physical((*data),ID) - idx_mem1);
          
	  return true;
          break;
    // Print a debug message to console: set output value
    case DEBUG_MSG_VALUE_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          debug_msg_value[ID] = *data;
          return true;
          break;
    // Print a debug message to console: set output mode (newline, etc.) and print
    case DEBUG_MSG_MODE_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          debug_msg_mode[ID] = *data;
          if (debug_msg_mode[ID] == 0x00000000)
          {
            printf("Fatal Error: Sim support module \"print\" invoked with no print mode by core %hu\n",
              ID);
            exit(1);
          }
          if ((debug_msg_mode[ID] & PR_HEX) && (debug_msg_mode[ID] & PR_DEC))
          {
            printf("Fatal Error: Sim support module \"print\" invoked with print mode where both hex and dec are active by core %hu\n",
              ID);
            exit(1);
          }

          for (int i = 0; i < ID; i ++)
            printf("\t");
          // Processor ID
          if (debug_msg_mode[ID] & PR_CPU_ID)
            printf("Processor %d - ", ID);
          // A custom string
          if (debug_msg_mode[ID] & PR_STRING)
            printf("%s ", debug_msg_string[ID]);
          // A hex value
          if (debug_msg_mode[ID] & PR_HEX)
            printf("0x%08x ", debug_msg_value[ID]);
          // A dec value
          if (debug_msg_mode[ID] & PR_DEC)
            printf("%d ", debug_msg_value[ID]);
          // A char value
          if (debug_msg_mode[ID] & PR_CHAR)
            printf("%c ", debug_msg_value[ID]);
          // The timestamp
          if (debug_msg_mode[ID] & PR_TSTAMP)
            printf("@ %10.1f ", sc_simulation_time());
          // A newline
          if (debug_msg_mode[ID] & PR_NEWL)
            puts("");
          return true; 
          break;
    // Print a debug message to console: ID of the involved processor
    case DEBUG_MSG_ID_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          // FIXME currently unused
          debug_msg_id[ID] = *data;
          return true;
          break;
    // Location where to find the command line argc
    case GET_ARGC_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (offset != 0x0)
          {
            printf("Fatal Error: Sim support module received an invalid request for argc data at address 0x%08x, by core %hu\n",
              DEBUG_MSG_ID_ADDRESS + offset, ID);
            exit(1);
          }
          *data = argc;
          return true;
          break;
    // Location where to find a pointer to the command line argv
    case GET_ARGV_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          *data = (uint32_t)(SIMSUPPORT_BASE + ARGV_ADDRESS);
          return true;
          break;
    // Location where to find a pointer to the environment
    case GET_ENVP_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          *data = (uint32_t)(SIMSUPPORT_BASE + ENVP_ADDRESS);
          return true;
          break;
    // Location where to find the command line argv (64 kB area)
    case ARGV_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          *data = *(uint32_t *)(*argv + offset);
          return true;
          break;
    // Location where to find the environment (64 kB area)
    case ENVP_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          *data = *(uint32_t *)(*envp + offset);
          return true;
          break;
    // Set the path and name of the file to be accessed (512 B area)
    case FILE_NAME_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          filename[ID] = (char *)(addresser->pMemoryDebug[ID] + (*data));
          return true;
          break;
    // Set the mode for file access
    case FILE_MODE_ADDRESS:
          if (write == false)
          {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (*data & FILE_READ)
            file_read[ID] = true;
          else if (*data & FILE_WRITE)
            file_read[ID] = false;
          return true;
          break;
    // Shift the file window forward
    case FILE_SHIFT_ADDRESS:
          if (!current_file[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a shift window command, while no file was open, by core %hu\n", ID);
            exit(1);
          }
          if (file_read[ID])
          {
            file_window_valid_data[ID] = fread(filebuffer[ID], 1, FILE_WINDOW_SIZE * 4, current_file[ID]);
            //printf("Simsupport: just read %u bytes from file %s for core %u\n", file_window_valid_data[ID], filename[ID], ID);
          }
          else
          {
            fwrite(filebuffer[ID], 1, file_window_valid_data[ID], current_file[ID]);
            //printf("Simsupport: just written %u bytes to file %s for core %u\n", file_window_valid_data[ID], filename[ID], ID);
          }
          return true;
          break;
    // Open a file on disk
    case FILE_OPEN_ADDRESS:
          if (write == true)
          {
            printf("Fatal Error: Sim support module received a write at address 0x%08x, which is read-only, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (current_file[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received an open command, while a previous file was still open, by core %hu\n", ID);
            exit(1);
          }
          if (!filename[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received an open command, but file name is unkwnown, by core %hu\n", ID);
            exit(1);
          }
          if (file_read[ID])
            current_file[ID] = fopen(filename[ID], "r");
          else
            current_file[ID] = fopen(filename[ID], "w");
          if (!current_file[ID])
            *data = 0;
          else
            *data = 1;
          file_window_valid_data[ID]     = 0;
          return true;
          break;
    // Close a file
    case FILE_CLOSE_ADDRESS:
          if (!current_file[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a close command, while no file was open, by core %hu\n", ID);
            exit(1);
          }
          if (fclose(current_file[ID]))
          {
            printf("Fatal Error: Sim support module \"file\" failure when closing the file opened by core %hu\n", ID);
            exit(1);
          }
          file_read[ID]                  = false;
          current_file[ID]               = NULL;
          file_window_valid_data[ID]     = 0;
          filename[ID]                   = NULL;
          return true;
          break;
    // Amount of useful data in the file window
    case FILE_WINDOW_DATA_ADDRESS:
          if (write == true && file_read[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a write at address 0x%08x, while reading a file, by core %hu\n",
              addr, ID);
            exit(1);
          }
          else if (write == false && !file_read[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a read at address 0x%08x, while writing a file, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (file_read[ID])
            *data = file_window_valid_data[ID];
          else
            file_window_valid_data[ID] = *data;
          return true;
          break;
    // Buffer where to exchange file data (64 kB area)
    case FILE_ADDRESS:
          if (write == true && file_read[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a write at address 0x%08x, while reading a file, by core %hu\n",
              addr, ID);
            exit(1);
          }
          else if (write == false && !file_read[ID])
          {
            printf("Fatal Error: Sim support module \"file\" received a read at address 0x%08x, while writing a file, by core %hu\n",
              addr, ID);
            exit(1);
          }
          if (offset > file_window_valid_data[ID])
          {
            printf("Warning: Sim support module \"file\" was asked to access a file portion after the EOF by core %hu\n", ID);
          }
          offset >>= 2;
          if (file_read[ID])
            *data = filebuffer[ID][offset];
          else
            filebuffer[ID][offset] = *data;
          return true;
          break;
     // Profile print functions
     case DUMP_TIME_START:
          if (write == false)
	   {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
           }
	  printf("Processor %d  -  0x%x start_time: %10.1f\n", ID, *data, sc_simulation_time());
          return true;   
          break;
    case DUMP_TIME_STOP:
          if (write == false)
	   {
            printf("Fatal Error: Sim support module received a read at address 0x%08x, which is write-only, by core %hu\n",
              addr, ID);
            exit(1);
           }
	  printf("Processor %d  -  0x%x stop_time: %10.1f\n", ID, *data, sc_simulation_time());
          return true;   
          break;
	  
    default:
          printf("Fatal Error: Sim support module received an access to address 0x%08x, which is unknown, by core %hu\n",
            addr, ID);
          exit(1);
          break;
  }
}
