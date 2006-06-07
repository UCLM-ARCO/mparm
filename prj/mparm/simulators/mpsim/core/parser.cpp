///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         parser.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Performs command line option parsing
//
///////////////////////////////////////////////////////////////////////////////

#include <systemc.h>
#include "globals.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include "debug.h"

using namespace std;

// Minimum and maximum acceptable cache sizes (2^LOWER_CACHE_SIZE_BOUND to
// 2^UPPER_CACHE_SIZE_BOUND included are acceptable)
#define LOWER_CACHE_SIZE_BOUND 4
#define UPPER_CACHE_SIZE_BOUND 20
// Minimum and maximum cache way associativity (2^LOWER_ASSOC_BOUND to
// 2^UPPER_ASSOC_BOUND included are acceptable)
#define LOWER_ASSOC_BOUND 1
#define UPPER_ASSOC_BOUND 5
// Minimum and maximum acceptable cache sizes (2^LOWER_SCRATCH_SIZE_BOUND to
// 2^UPPER_SCRATCH_SIZE_BOUND included are acceptable)
#define LOWER_SCRATCH_SIZE_BOUND 4
#define UPPER_SCRATCH_SIZE_BOUND 20

///////////////////////////////////////////////////////////////////////////////
// validAssoc - Checks to see if input is a valid power of 2.
bool validAssoc(unsigned int assoc)
{
  // check powers of 2 from 2^LOWER_ASSOC_BOUND to 2^UPPER_ASSOC_BOUND
  if (assoc < (unsigned int)(1 << LOWER_ASSOC_BOUND))
  {
    puts("Parameter too small!");
    return false;
  }
  
  if (assoc > (unsigned int)(1 << UPPER_ASSOC_BOUND))
  {
    puts("Parameter too big!");
    return false;
  }
  
  if (assoc & (assoc - 1))
    return false;
  else
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// getNextToken - Gets the next token of one configuration line.
string getNextToken(istringstream *thisline)
{
  string token;
  
  do
  {
    getline(*thisline, token, ' ');
  } while (*thisline && (token == "\t" || token == " " || token == ""));
  
  if (*thisline)
    return token;
  else
    return "";
}


///////////////////////////////////////////////////////////////////////////////
// assumeNullToken - Checks that no additional tokens are present on the line.
void assumeNullToken(istringstream *thisline)
{
  string token = getNextToken(thisline);
  
  if (token != "")
  {
    cout << "Fatal Error: unexpected token " << token << " at the end of line!" << endl;
    exit(1);
  }
}


typedef enum {GLOBAL_SCOPE, CORE_SCOPE, SLAVE_SCOPE, BUS_SCOPE, BRIDGE_SCOPE} PARSING_STATUS;
PARSING_STATUS parsing_status = GLOBAL_SCOPE;
bool core_num_specified = false;
bool slave_num_specified = false;
bool bus_num_specified = false;
bool bridge_num_specified = false;
bool bridge_nr_known = false;
unsigned short int current_ID;
unsigned short int current_nr;


///////////////////////////////////////////////////////////////////////////////
// parseLine - Interprets one configuration line.
void parseLine(string inputline)
{
  string token;
  istringstream thisline(inputline);
    
  token = getNextToken(&thisline);
  
  if (token != "")
  {
    switch (parsing_status)
    {
      // ---------------
      // GLOBAL SETTINGS
      // ---------------
      case GLOBAL_SCOPE:
        if (token == "system_cores")
        {
          token = getNextToken(&thisline);
          // cout << "System cores " << token << endl;
          N_CORES = atoi(token.c_str());
          N_MASTERS = N_CORES;
          if (N_CORES == 0)
          {
            cout << "Fatal Error: at least one core is required in the system!" << endl;
            exit(1);
          }
          
          if (M_DIVIDER)
            delete M_DIVIDER;
          M_DIVIDER = new unsigned short int [N_FREQ_DEVICE];
          // Initialize to a safe value
          for (unsigned short int i = 0; i < N_FREQ_DEVICE; i ++)
            M_DIVIDER[i] = 1;
          
          if (MASTER_CONFIG)
            delete MASTER_CONFIG;
          MASTER_CONFIG = new MASTER_CONF [N_MASTERS];
          // Initialize to a safe value
          for (unsigned short int i = 0; i < N_MASTERS; i ++)
            MASTER_CONFIG[i].binding = 0;
          
          assumeNullToken(&thisline);
          core_num_specified = true;
        }
        else if (token == "system_slaves")
        {
          token = getNextToken(&thisline);
          // cout << "System slaves " << token << endl;
          N_SLAVES = atoi(token.c_str());
          if (N_SLAVES == 0)
          {
            cout << "Fatal Error: at least one slave is required in the system!" << endl;
            exit(1);
          }
          
          if (SLAVE_CONFIG)
            delete SLAVE_CONFIG;
          SLAVE_CONFIG = new SLAVE_CONF [N_SLAVES];
          // Initialize to a safe value
          for (unsigned short int i = 0; i < N_SLAVES; i ++)
          {
            SLAVE_CONFIG[i].binding = 0;
            SLAVE_CONFIG[i].range_set = false;
          }
          
          assumeNullToken(&thisline);
          slave_num_specified = true;
        }
        else if (token == "system_buses")
        {
          token = getNextToken(&thisline);
          // cout << "System buses " << token << endl;
          N_BUSES = atoi(token.c_str());
          if (N_BUSES == 0)
          {
            cout << "Fatal Error: at least one bus is required in the system!" << endl;
            exit(1);
          }
          
          if (I_DIVIDER)
            delete I_DIVIDER;
          I_DIVIDER = new unsigned short int [N_BUSES];
          // Initialize to a safe value
          for (unsigned short int i = 0; i < N_BUSES; i ++)
            I_DIVIDER[i] = 1;
          
          assumeNullToken(&thisline);
          bus_num_specified = true;
        }
        else if (token == "system_bridges")
        {
          token = getNextToken(&thisline);
          // cout << "System bridges " << token << endl;
          N_BRIDGES = atoi(token.c_str());
          
          if (N_BRIDGES > 0)
          {
            if (BRIDGE_CONFIG)
              delete BRIDGE_CONFIG;
            BRIDGE_CONFIG = new BRIDGE_CONF [N_BRIDGES];
            // Initialize to a safe value
            for (unsigned short int i = 0; i < N_BRIDGES; i ++)
            {
              BRIDGE_CONFIG[i].master_binding = 1;
              BRIDGE_CONFIG[i].slave_binding = 0;
            }
          }
          
          assumeNullToken(&thisline);
          bridge_num_specified = true;
        }
        else if (token == "entity_core")
        {
          if (!core_num_specified)
          {
            cout << "Fatal Error: specify total number of cores before configuring one!" << endl;
            exit(1);
          }
          token = getNextToken(&thisline);
          current_ID = atoi(token.c_str());
          if (current_ID >= N_CORES)
          {
            cout << "Fatal Error: configuration was specified for non-existing core '" << token << "'!" << endl;
            exit(1);
          }
          assumeNullToken(&thisline);
          parsing_status = CORE_SCOPE;
          // cout << "Entering configuration of core " << current_ID << endl;
        }
        else if (token == "entity_slave")
        {
          if (!slave_num_specified)
          {
            cout << "Fatal Error: specify total number of slaves before configuring one!" << endl;
            exit(1);
          }
          token = getNextToken(&thisline);
          current_ID = atoi(token.c_str());
          if (current_ID >= N_SLAVES)
          {
            cout << "Fatal Error: configuration was specified for non-existing slave '" << token << "'!" << endl;
            exit(1);
          }
          assumeNullToken(&thisline);
          parsing_status = SLAVE_SCOPE;
          // cout << "Entering configuration of slave " << current_ID << endl;
        }
        else if (token == "entity_bus")
        {
          if (!bus_num_specified)
          {
            cout << "Fatal Error: specify total number of buses before configuring one!" << endl;
            exit(1);
          }
          token = getNextToken(&thisline);
          current_ID = atoi(token.c_str());
          if (current_ID >= N_BUSES)
          {
            cout << "Fatal Error: configuration was specified for non-existing bus '" << token << "'!" << endl;
            exit(1);
          }
          assumeNullToken(&thisline);
          parsing_status = BUS_SCOPE;
          // cout << "Entering configuration of bus " << current_ID << endl;
        }
        else if (token == "entity_bridge")
        {
          if (!bridge_num_specified)
          {
            cout << "Fatal Error: specify total number of bridges before configuring one!" << endl;
            exit(1);
          }
          token = getNextToken(&thisline);
          current_ID = atoi(token.c_str());
          if (current_ID >= N_BRIDGES)
          {
            cout << "Fatal Error: configuration was specified for non-existing bridge '" << token << "'!" << endl;
            exit(1);
          }
          assumeNullToken(&thisline);
          parsing_status = BRIDGE_SCOPE;
          // cout << "Entering configuration of bridge " << current_ID << endl;
        }
        else
        {
          cout << "Fatal Error: invalid token '" << token << "' while in global scope!" << endl;
          exit(1);
        }
        break;
      // ------------------
      // CORE CONFIGURATION
      // ------------------
      case CORE_SCOPE:
        if (token == "bus")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          unsigned short int tgt_bus = atoi(token.c_str());
          // Sanity check will follow later
          MASTER_CONFIG[current_ID].binding = tgt_bus;
          // cout << "Binding master " << current_ID << " to bus " << tgt_bus << endl;
        }
        else if (token == "freqdiv")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          unsigned short int freq = atoi(token.c_str());
          // Sanity check will follow later
          M_DIVIDER[current_ID] = freq;
          // cout << "Setting frequency divider of master " << current_ID << " to " << freq << endl;
        }
        else if (token == "end")
        {
          parsing_status = GLOBAL_SCOPE;
        }
        else
        {
          cout << "Fatal Error: invalid token '" << token << "' while in core scope!" << endl;
          exit(1);
        }
        break;
      // -------------------
      // SLAVE CONFIGURATION
      // -------------------
      case SLAVE_SCOPE:
        if (token == "bus")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
   
          unsigned short int tgt_bus = atoi(token.c_str());
          // Sanity check will follow later
          // FIXME the ID scheme assumes that slaves will be specified in-order (private first, shared second, ...)
          SLAVE_CONFIG[current_ID].binding = tgt_bus;
          // cout << "Binding slave " << current_ID << " to bus " << tgt_bus << endl;
        }
        else if (token == "type")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
   
          if (token == "private")
          {
            N_PRIVATE ++;
          }
          else if (token == "shared")
          {
            N_SHARED ++;
          }
          else if (token == "semaphore")
          {
            N_SEMAPHORE ++;
          }
          else if (token == "interrupt")
          {
            N_INTERRUPT ++;
          }
          else
          {
            cout << "Fatal Error: invalid slave type '" << token << "'!" << endl;
            exit(1);
          }
          
          // cout << "Setting up slave " << current_ID << " as type " << token << endl;
        }
        else if (token == "range")
        {
          uint32_t start_add, end_add;
          token = getNextToken(&thisline);
          istringstream startadd(token);
          startadd >> hex >> start_add;
          token = getNextToken(&thisline);
          istringstream endadd(token);
          endadd >> hex >> end_add;
          assumeNullToken(&thisline);
   
          // FIXME just hoping the user will provide ranges without overlapping
          SLAVE_CONFIG[current_ID].range_set = true;
          SLAVE_CONFIG[current_ID].start_address = start_add;
          // FIXME Ugly hack +1
          SLAVE_CONFIG[current_ID].end_address = end_add + 1;
          
          if (start_add >= end_add)
          {
            cout << "Fatal Error: invalid mapping range 0x" << hex << setfill('0') << setw(8) << start_add <<
              "-0x" << setw(8) << end_add << dec << " set for slave " << current_ID << "!" << endl;
            exit(1);
          }
          
          // cout << "Slave " << current_ID << " mapping range set to 0x"
          //   << hex << setfill('0') << setw(8) << start_add << "-0x" << setw(8) << end_add << dec << endl;
        }
        else if (token == "end")
        {
          parsing_status = GLOBAL_SCOPE;
        }
        else
        {
          cout << "Fatal Error: invalid token '" << token << "' while in slave scope!" << endl;
          exit(1);
        }
        break;
      // -----------------
      // BUS CONFIGURATION
      // -----------------
      case BUS_SCOPE:
        if (token == "freqdiv")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          unsigned short int freq = atoi(token.c_str());
          // Sanity check will follow later
          I_DIVIDER[current_ID] = freq;
          // cout << "Setting frequency divider of bus " << current_ID << " to " << freq << endl;
        }
        else if (token == "end")
        {
          parsing_status = GLOBAL_SCOPE;
        }
        else
        {
          cout << "Fatal Error: invalid token '" << token << "' while in bus scope!" << endl;
          exit(1);
        }
        break;
      // --------------------
      // BRIDGE CONFIGURATION
      // --------------------
      case BRIDGE_SCOPE:
        if (token == "slaveonbus")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          unsigned short int tgt_bus = atoi(token.c_str());
          // Sanity check will follow later
          BRIDGE_CONFIG[current_ID].slave_binding = tgt_bus;
          // cout << "Binding bridge " << current_ID << " slave to bus " << tgt_bus << endl;
        }
        else if (token == "masteronbus")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          unsigned short int tgt_bus = atoi(token.c_str());
          // Sanity check will follow later
          BRIDGE_CONFIG[current_ID].master_binding = tgt_bus;
          // cout << "Binding bridge " << current_ID << " master to bus " << tgt_bus << endl;
        }
        // FIXME this token is, in theory, unneeded
        else if (token == "mapped_ranges")
        {
          token = getNextToken(&thisline);
          assumeNullToken(&thisline);
          
          bridge_nr_known = true;
          
          current_nr = atoi(token.c_str());
          if (current_nr == 0)
          {
            cout << "Fatal Error: bus bridge " << current_ID << " must map at least one addressing range!" << endl;
            exit(1);
          }
          
          BRIDGE_CONFIG[current_ID].n_ranges = current_nr;
          BRIDGE_CONFIG[current_ID].start_address = new uint32_t [current_nr];
          BRIDGE_CONFIG[current_ID].end_address = new uint32_t [current_nr];
          
          // cout << "Configuring bridge " << current_ID << " for " << current_nr << " ranges" << endl;
        }
        else if (token == "range")
        {
          uint32_t start_add, end_add;
          uint16_t range_ID;
          
          if (!bridge_nr_known)
          {
            cout << "Fatal Error: specify number of addressing ranges for bridge " << current_ID << " before configuring one!" << endl;
            exit(1);
          }
        
          // FIXME just hoping the user will provide all ranges and without overlapping
          token = getNextToken(&thisline);
          range_ID = atoi(token.c_str());
          if (range_ID >= current_nr)
          {
            cout << "Fatal Error: attempt to configure addressing range " << range_ID << " of bridge " << current_ID <<
              ", which only has " << current_nr << " addressing ranges!" << endl;
            exit(1);
          }
          token = getNextToken(&thisline);
          istringstream startadd(token);
          startadd >> hex >> start_add;
          token = getNextToken(&thisline);
          istringstream endadd(token);
          endadd >> hex >> end_add;
          assumeNullToken(&thisline);
          
          if (start_add >= end_add)
          {
            cout << "Fatal Error: invalid mapping range 0x" << hex << setfill('0') << setw(8) << start_add <<
              "-0x" << setw(8) << end_add << dec << " set for bridge " << current_ID << "!" << endl;
            exit(1);
          }
          
          BRIDGE_CONFIG[current_ID].start_address[range_ID] = start_add;
          BRIDGE_CONFIG[current_ID].end_address[range_ID]   = end_add;
          // cout << "Bridge " << current_ID << " mapping range " << range_ID << " set to 0x"
          //      << hex << setfill('0') << setw(8) << start_add << "-0x" << setw(8) << end_add << dec << endl;
        }
        else if (token == "end")
        {
          parsing_status = GLOBAL_SCOPE;
          bridge_nr_known = false;
        }
        else
        {
          cout << "Fatal Error: invalid token '" << token << "' while in bridge scope!" << endl;
          exit(1);
        }
        break;
      // -------
      // UNKNOWN
      // -------
      default:
      {
        cout << "Fatal Error: unexpected internal status while parsing configuration file!" << endl;
        exit(1);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// stripFileLine - Strips leading spaces and comments from a line.
string stripFileLine(string inputline)
{
  string outputline = "";
  bool assumeleadinggarbage = true;
  
  for (unsigned int i = 0; i < inputline.length(); i ++)
  {
    // Parse every char of the string
    char thischar = inputline.at(i);
    
    // This is a comment, skip everything from now on
    if (thischar == '#')
      break;
    
    // Are we hitting leading garbage (indentation)?
    if (assumeleadinggarbage)
    {
      if (thischar != ' ' && thischar != '\t')
      {
        assumeleadinggarbage = false;
        outputline += thischar;
      }
    }
    else
      outputline += thischar;
  }
  
  return outputline;
}


///////////////////////////////////////////////////////////////////////////////
// getFileLine - Gets one line from an input file.
string getFileLine(istream &inputfile)
{
  string inputline, strippedline;
  
  // Get a non-empty line
  getline(inputfile, inputline, '\n');
  strippedline = stripFileLine(inputline);
  while (inputfile && (strippedline.length() == 0))
  {
    getline(inputfile, inputline, '\n');
    strippedline = stripFileLine(inputline);
  }
  
  return strippedline;
}


///////////////////////////////////////////////////////////////////////////////
// parseCfgFile - Parses a configuration file.
void parseCfgFile()
{
  ifstream cfgfile(CFGFILENAME.c_str());
  string inputline, cmdstring;
  
  // ----------------------------
  // Parse the configuration file
  // ----------------------------
  if (cfgfile.fail())
  {
    cout << "Fatal Error: The specified configuration file " << CFGFILENAME << " does not exist!" << endl;
    exit(1);
  }
  
  // Base values
  FREQSCALING = false;
  FREQSCALINGDEVICE = false;
  N_CORES = 0;
  N_MASTERS = 0;
  N_SLAVES = 0;
  N_PRIVATE = 0;
  N_SHARED = 0;
  N_SEMAPHORE = 0;
  N_INTERRUPT = 0;
  
  inputline = getFileLine(cfgfile);
  
  while (cfgfile)
  {
    parseLine(inputline);
    inputline = getFileLine(cfgfile);
  }
  
  // --------------------------
  // Check configuration sanity
  // --------------------------
  for (unsigned short int j = 0; j < N_MASTERS; j ++)
  {
    if (MASTER_CONFIG[j].binding >= N_BUSES)
    {
      cout << "Fatal error: attempt to connect bus master " << j << " to bus " << MASTER_CONFIG[j].binding <<
        ", which does not exist (" << N_BUSES << " buses are present!)" << endl;
      exit(1);
    }
    
    if (M_DIVIDER[j] == 0)
    {
      cout << "Fatal error: attempt to set incorrect frequency divider 0 for core " << j << "!" << endl;
      exit(1);
    }
    else if (M_DIVIDER[j] != 1)
      FREQSCALING = true;
  }
  
  for (unsigned short int j = 0; j < N_SLAVES; j ++)
  {
    if (SLAVE_CONFIG[j].binding >= N_BUSES)
    {
      cout << "Fatal error: attempt to connect bus slave " << j << " to bus " << SLAVE_CONFIG[j].binding <<
        ", which does not exist (" << N_BUSES << " buses are present!)" << endl;
      exit(1);
    }
  }
  
  if (N_PRIVATE != N_MASTERS)
  {
    cout << "Fatal error: there must be as many private memories as there are cores (" << N_MASTERS << ")." << endl;
    cout << "             Instead, " << N_PRIVATE << " private memories are being configured!" << endl;
    exit(1);
  }
  
  if (N_SLAVES != N_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT)
  {
    cout << "Fatal error: there must be as many slaves as there are private memories (" << N_PRIVATE << ")," << endl;
    cout << "             plus shared memories (" << N_SHARED << "), plus semaphore devices (" << N_SEMAPHORE <<
      ") plus interrupt devices (" << N_INTERRUPT << "); instead, " << N_SLAVES << " slaves are being configured!" << endl;
    exit(1);
  }
  
  for (unsigned short int j = 0; j < N_BUSES; j ++)
  {
    if (I_DIVIDER[j] == 0)
    {
      cout << "Fatal error: attempt to set incorrect frequency divider 0 for bus " << j << "!" << endl;
      exit(1);
    }
    else if (I_DIVIDER[j] != 1)
      FREQSCALING = true;
  }
  
  // FIXME no check for buses isolated from the system due to lack of bridges
  if (N_BRIDGES > 0 && N_BUSES == 1)
  {
    cout << "Fatal error: " << N_BRIDGES << "bus bridges should be instantiated, but only one bus is present!" << endl;
    exit(1);
  }
  
  for (unsigned short int j = 0; j < N_BRIDGES; j ++)
  {
    if (BRIDGE_CONFIG[j].master_binding >= N_BUSES)
    {
      cout << "Fatal error: attempt to connect bus bridge " << j << " master to bus " << BRIDGE_CONFIG[j].master_binding <<
        ", which does not exist (" << N_BUSES << " buses are present!)" << endl;
      exit(1);
    }
    if (BRIDGE_CONFIG[j].slave_binding >= N_BUSES)
    {
      cout << "Fatal error: attempt to connect bus bridge " << j << " slave to bus " << BRIDGE_CONFIG[j].slave_binding <<
        ", which does not exist (" << N_BUSES << " buses are present!)" << endl;
      exit(1);
    }
    if (BRIDGE_CONFIG[j].master_binding == BRIDGE_CONFIG[j].slave_binding)
    {
      cout << "Fatal error: attempt to use bus bridge " << j << " to connect bus " << BRIDGE_CONFIG[j].master_binding <<
        " to itself!" << endl;
      exit(1);
    }
  }

  cfgfile.close();
}


///////////////////////////////////////////////////////////////////////////////
// parseArgs - Parses command line parameters.
void parseArgs(int argc, char *argv[], char *envp[], int *new_argc, char **new_argv[], char **new_envp[])
{

  // If the Makefile provided default values at compile time, use them.
  // Otherwise, use built-in defaults. Every option will then be
  // configurable at runtime via command line
#ifdef MAKEFILE_DEFAULT_CURRENT_ISS
  CURRENT_ISS = MAKEFILE_DEFAULT_CURRENT_ISS;
#else
  CURRENT_ISS = BUILTIN_DEFAULT_CURRENT_ISS;
#endif

#ifdef MAKEFILE_DEFAULT_CURRENT_INTERC
  CURRENT_INTERC = MAKEFILE_DEFAULT_CURRENT_INTERC;
#else
  CURRENT_INTERC = BUILTIN_DEFAULT_CURRENT_INTERC;
#endif
  
#ifdef MAKEFILE_DEFAULT_N_CORES
  N_CORES = MAKEFILE_DEFAULT_N_CORES;
#else
  N_CORES = BUILTIN_DEFAULT_N_CORES;
#endif

#ifdef MAKEFILE_DEFAULT_SHARED_CACHE
  SHARED_CACHE = MAKEFILE_DEFAULT_SHARED_CACHE;
#else
  SHARED_CACHE = BUILTIN_DEFAULT_SHARED_CACHE;
#endif

#ifdef MAKEFILE_DEFAULT_NSIMCYCLES
  NSIMCYCLES = MAKEFILE_DEFAULT_NSIMCYCLES;
#else
  NSIMCYCLES = BUILTIN_DEFAULT_NSIMCYCLES;
#endif

#ifdef MAKEFILE_DEFAULT_ACCTRACE
  ACCTRACE = MAKEFILE_DEFAULT_ACCTRACE;
#else
  ACCTRACE = BUILTIN_DEFAULT_ACCTRACE;
#endif

#ifdef MAKEFILE_DEFAULT_VCD
  VCD = MAKEFILE_DEFAULT_VCD;
#else
  VCD = BUILTIN_DEFAULT_VCD;
#endif

#ifdef MAKEFILE_DEFAULT_AUTOSTARTMEASURING
  AUTOSTARTMEASURING = MAKEFILE_DEFAULT_AUTOSTARTMEASURING;
#else
  AUTOSTARTMEASURING = BUILTIN_DEFAULT_AUTOSTARTMEASURING;
#endif

#ifdef MAKEFILE_DEFAULT_SPCHECK
  SPCHECK = MAKEFILE_DEFAULT_SPCHECK;
#else
  SPCHECK = BUILTIN_DEFAULT_SPCHECK;
#endif

#ifdef MAKEFILE_DEFAULT_SHOWPROMPT
  SHOWPROMPT = MAKEFILE_DEFAULT_SHOWPROMPT;
#else
  SHOWPROMPT = BUILTIN_DEFAULT_SHOWPROMPT;
#endif

#ifdef MAKEFILE_DEFAULT_FREQSCALING
  FREQSCALING = MAKEFILE_DEFAULT_FREQSCALING;
#else
  FREQSCALING = BUILTIN_DEFAULT_FREQSCALING;
#endif

#ifdef MAKEFILE_DEFAULT_FREQSCALINGDEVICE
  FREQSCALINGDEVICE = MAKEFILE_DEFAULT_FREQSCALINGDEVICE;
#else
  FREQSCALINGDEVICE = BUILTIN_DEFAULT_FREQSCALINGDEVICE;
#endif

#ifdef MAKEFILE_DEFAULT_CORESLAVE
  CORESLAVE = MAKEFILE_DEFAULT_CORESLAVE;
#else
  CORESLAVE = BUILTIN_DEFAULT_CORESLAVE;
#endif

#ifdef MAKEFILE_DEFAULT_INT_WS
  INT_WS = MAKEFILE_DEFAULT_INT_WS;
#else
  INT_WS = BUILTIN_DEFAULT_INT_WS;
#endif

#ifdef MAKEFILE_DEFAULT_MEM_IN_WS
  MEM_IN_WS = MAKEFILE_DEFAULT_MEM_IN_WS;
#else
  MEM_IN_WS = BUILTIN_DEFAULT_MEM_IN_WS;
#endif

#ifdef MAKEFILE_DEFAULT_MEM_BB_WS
  MEM_BB_WS = MAKEFILE_DEFAULT_MEM_BB_WS;
#else
  MEM_BB_WS = BUILTIN_DEFAULT_MEM_BB_WS;
#endif

#ifdef MAKEFILE_DEFAULT_STATS
  STATS = MAKEFILE_DEFAULT_STATS;
#else
  STATS = BUILTIN_DEFAULT_STATS;
#endif

#ifdef MAKEFILE_DEFAULT_POWERSTATS
  POWERSTATS = MAKEFILE_DEFAULT_POWERSTATS;
#else
  POWERSTATS = BUILTIN_DEFAULT_POWERSTATS;
#endif

#ifdef MAKEFILE_DEFAULT_DMA
  DMA = MAKEFILE_DEFAULT_DMA;
#else
  DMA = BUILTIN_DEFAULT_DMA;
#endif

#ifdef MAKEFILE_DEFAULT_SMARTMEM
  SMARTMEM = MAKEFILE_DEFAULT_SMARTMEM;
#else
  SMARTMEM = BUILTIN_DEFAULT_SMARTMEM;
#endif

#ifdef MAKEFILE_DEFAULT_SCRATCH
  SCRATCH = MAKEFILE_DEFAULT_SCRATCH;
#else
  SCRATCH = BUILTIN_DEFAULT_SCRATCH;
#endif

#ifdef MAKEFILE_DEFAULT_ISCRATCH
  ISCRATCH = MAKEFILE_DEFAULT_ISCRATCH;
#else
  ISCRATCH = BUILTIN_DEFAULT_ISCRATCH;
#endif

#ifdef MAKEFILE_DEFAULT_ISCRATCHSIZE
  ISCRATCHSIZE = MAKEFILE_DEFAULT_ISCRATCHSIZE;
#else
  ISCRATCHSIZE = BUILTIN_DEFAULT_ISCRATCHSIZE;
#endif

#ifdef MAKEFILE_DEFAULT_N_PRIVATE
  N_PRIVATE = MAKEFILE_DEFAULT_N_PRIVATE;
#else
  N_PRIVATE = BUILTIN_DEFAULT_N_PRIVATE;
#endif

#ifdef MAKEFILE_DEFAULT_N_LX_PRIVATE
  N_LX_PRIVATE = MAKEFILE_DEFAULT_N_LX_PRIVATE;
#else
  N_LX_PRIVATE = BUILTIN_DEFAULT_N_LX_PRIVATE;
#endif

#ifdef MAKEFILE_DEFAULT_N_SHARED
  N_SHARED = MAKEFILE_DEFAULT_N_SHARED;
#else
  N_SHARED = BUILTIN_DEFAULT_N_SHARED;
#endif

#ifdef MAKEFILE_DEFAULT_N_SEMAPHORE
  N_SEMAPHORE = MAKEFILE_DEFAULT_N_SEMAPHORE;
#else
  N_SEMAPHORE = BUILTIN_DEFAULT_N_SEMAPHORE;
#endif

#ifdef MAKEFILE_DEFAULT_N_INTERRUPT
  N_INTERRUPT = MAKEFILE_DEFAULT_N_INTERRUPT;
#else
  N_INTERRUPT = BUILTIN_DEFAULT_N_INTERRUPT;
#endif

#ifdef MAKEFILE_DEFAULT_N_STORAGE
  N_STORAGE = MAKEFILE_DEFAULT_N_STORAGE;
#else
  N_STORAGE = BUILTIN_DEFAULT_N_STORAGE;
#endif

#ifdef MAKEFILE_DEFAULT_N_SMARTMEM
  N_SMARTMEM = MAKEFILE_DEFAULT_N_SMARTMEM;
#else
  N_SMARTMEM = BUILTIN_DEFAULT_N_SMARTMEM;
#endif

#ifdef MAKEFILE_DEFAULT_N_FFT
  N_FFT = MAKEFILE_DEFAULT_N_FFT;
#else
  N_FFT = BUILTIN_DEFAULT_N_FFT;
#endif

#ifdef MAKEFILE_DEFAULT_N_BUSES
  N_BUSES = MAKEFILE_DEFAULT_N_BUSES;
#else
  N_BUSES = BUILTIN_DEFAULT_N_BUSES;
#endif

#ifdef MAKEFILE_DEFAULT_N_BRIDGES
  N_BRIDGES = MAKEFILE_DEFAULT_N_BRIDGES;
#else
  N_BRIDGES = BUILTIN_DEFAULT_N_BRIDGES;
#endif

#ifdef MAKEFILE_DEFAULT_N_IP_TG
  N_IP_TG = MAKEFILE_DEFAULT_N_IP_TG;
#else
  N_IP_TG = BUILTIN_DEFAULT_N_IP_TG;
#endif

#ifdef MAKEFILE_DEFAULT_N_FREQ_DEVICE
  N_FREQ_DEVICE = MAKEFILE_DEFAULT_N_FREQ_DEVICE;
#else
  N_FREQ_DEVICE = BUILTIN_DEFAULT_N_FREQ_DEVICE;
#endif

MASTER_CONFIG = NULL;
SLAVE_CONFIG  = NULL;
BRIDGE_CONFIG = NULL;

#ifdef MAKEFILE_DEFAULT_DCACHESIZE
  DCACHESIZE = MAKEFILE_DEFAULT_DCACHESIZE;
#else
  DCACHESIZE = BUILTIN_DEFAULT_DCACHESIZE;
#endif

#ifdef MAKEFILE_DEFAULT_ICACHESIZE
  ICACHESIZE = MAKEFILE_DEFAULT_ICACHESIZE;
#else
  ICACHESIZE = BUILTIN_DEFAULT_ICACHESIZE;
#endif

#ifdef MAKEFILE_DEFAULT_UCACHESIZE
  UCACHESIZE = MAKEFILE_DEFAULT_UCACHESIZE;
#else
  UCACHESIZE = BUILTIN_DEFAULT_UCACHESIZE;
#endif

#ifdef MAKEFILE_DEFAULT_DCACHETYPE
  DCACHETYPE = MAKEFILE_DEFAULT_DCACHETYPE;
#else
  DCACHETYPE = BUILTIN_DEFAULT_DCACHETYPE;
#endif

#ifdef MAKEFILE_DEFAULT_ICACHETYPE
  ICACHETYPE = MAKEFILE_DEFAULT_ICACHETYPE;
#else
  ICACHETYPE = BUILTIN_DEFAULT_ICACHETYPE;
#endif

#ifdef MAKEFILE_DEFAULT_UCACHETYPE
  UCACHETYPE = MAKEFILE_DEFAULT_UCACHETYPE;
#else
  UCACHETYPE = BUILTIN_DEFAULT_UCACHETYPE;
#endif

#ifdef MAKEFILE_DEFAULT_DCACHEWAYS
  DCACHEWAYS = MAKEFILE_DEFAULT_DCACHEWAYS;
#else
  DCACHEWAYS = BUILTIN_DEFAULT_DCACHEWAYS;
#endif

#ifdef MAKEFILE_DEFAULT_ICACHEWAYS
  ICACHEWAYS = MAKEFILE_DEFAULT_ICACHEWAYS;
#else
  ICACHEWAYS = BUILTIN_DEFAULT_ICACHEWAYS;
#endif

#ifdef MAKEFILE_DEFAULT_UCACHEWAYS
  UCACHEWAYS = MAKEFILE_DEFAULT_UCACHEWAYS;
#else
  UCACHEWAYS = BUILTIN_DEFAULT_UCACHEWAYS;
#endif

#ifdef MAKEFILE_DEFAULT_DCACHE_WS
  DCACHE_WS = MAKEFILE_DEFAULT_DCACHE_WS;
#else
  DCACHE_WS = BUILTIN_DEFAULT_DCACHE_WS;
#endif

#ifdef MAKEFILE_DEFAULT_ICACHE_WS
  ICACHE_WS = MAKEFILE_DEFAULT_ICACHE_WS;
#else
  ICACHE_WS = BUILTIN_DEFAULT_ICACHE_WS;
#endif

#ifdef MAKEFILE_DEFAULT_UCACHE_WS
  UCACHE_WS = MAKEFILE_DEFAULT_UCACHE_WS;
#else
  UCACHE_WS = BUILTIN_DEFAULT_UCACHE_WS;
#endif

#ifdef MAKEFILE_DEFAULT_SCRATCH_WS
  SCRATCH_WS = MAKEFILE_DEFAULT_SCRATCH_WS;
#else
  SCRATCH_WS = BUILTIN_DEFAULT_SCRATCH_WS;
#endif

#ifdef MAKEFILE_DEFAULT_SCRATCHSIZE
  SCRATCH_SIZE = MAKEFILE_DEFAULT_SCRATCH_SIZE;
#else
  SCRATCH_SIZE = BUILTIN_DEFAULT_SCRATCH_SIZE;
#endif

#ifdef MAKEFILE_DEFAULT_QUEUESIZE
  QUEUE_SIZE = MAKEFILE_DEFAULT_QUEUE_SIZE;
#else
  QUEUE_SIZE = BUILTIN_DEFAULT_QUEUE_SIZE;
#endif

#ifdef MAKEFILE_DEFAULT_TG_TRACE_COLLECTION
  TG_TRACE_COLLECTION = MAKEFILE_DEFAULT_TG_TRACE_COLLECTION;
#else
  TG_TRACE_COLLECTION = BUILTIN_DEFAULT_TG_TRACE_COLLECTION;
#endif

#ifdef MAKEFILE_DEFAULT_USING_OCP
  USING_OCP = MAKEFILE_DEFAULT_USING_OCP;
#else
  USING_OCP = BUILTIN_DEFAULT_USING_OCP;
#endif

#ifdef MAKEFILE_DEFAULT_STATSFILENAME
  STATSFILENAME = MAKEFILE_DEFAULT_STATSFILENAME;
#else
  STATSFILENAME = BUILTIN_DEFAULT_STATSFILENAME;
#endif

#ifdef MAKEFILE_DEFAULT_CFGFILENAME
  CFGFILENAME = MAKEFILE_DEFAULT_CFGFILENAME;
#else
  CFGFILENAME = BUILTIN_DEFAULT_CFGFILENAME;
#endif

M_DIVIDER = NULL;
I_DIVIDER = NULL;
PLL_DELAY = NULL;

#ifdef MAKEFILE_DEFAULT_SNOOPING
  SNOOPING = MAKEFILE_DEFAULT_SNOOPING;
#else
  SNOOPING = BUILTIN_DEFAULT_SNOOPING;
#endif

#ifdef MAKEFILE_DEFAULT_SNOOP_POLICY
  SNOOP_POLICY = MAKEFILE_DEFAULT_SNOOP_POLICY;
#else
  SNOOP_POLICY = BUILTIN_DEFAULT_SNOOP_POLICY;
#endif

#ifdef WB_CACHE
  #ifdef MAKEFILE_DEFAULT_CACHE_WRITE_POLICY
  CACHE_WRITE_POLICY = MAKEFILE_DEFAULT_CACHE_WRITE_POLICY;
  #else
  CACHE_WRITE_POLICY = BUILTIN_DEFAULT_CACHE_WRITE_POLICY;
  #endif
#endif

#ifdef MAKEFILE_DEFAULT_DRAM
  DRAM = MAKEFILE_DEFAULT_DRAM;
#else
  DRAM = BUILTIN_DEFAULT_DRAM;
#endif
  
  *new_argc = 0;     // new_argc and new_argv might be modified later on, if the --cmdl
  *new_argv = argv;  // switch will be provided
  *new_envp = envp;  // envp will always be forwarded as-is at the moment
  
  bool private_memories_explicitly_set = false; // Did the user manually specify a value for this?
  
  int c;
  int option_index = 0;
  unsigned short int temp;
  unsigned int length;
  char *stringbuff;
  unsigned short int found_args;
  extern char *optarg;
  extern int optind, opterr, optopt;
  static struct option long_options[] = {
                                          {"help",         0, 0,  'h'},
                                          {"ds",           1, 0,  301},
                                          {"is",           1, 0,  302},
                                          {"us",           1, 0,  303},
                                          {"dt",           1, 0,  304},
                                          {"it",           1, 0,  305},
                                          {"ut",           1, 0,  306},
                                          {"dws",          1, 0,  307},
                                          {"iws",          1, 0,  308},
                                          {"uws",          1, 0,  309},
                                          {"Sws",          1, 0,  310},

                                          {"core",         1, 0,  401},
                                          {"intc",         1, 0,  402},
#ifndef NOSNOOP
                                          {"snoop",        0, 0,  501},
                                          {"snoop-policy", 1, 0,  502},
#endif
#ifdef WB_CACHE
                                          {"cwp",          1, 0,  503},
#endif
                                          {"i-s",          0, 0,  601},
                                          {"i-size",       1, 0,  602},
                                          {"s-size",       1, 0,  701},
                                          {"q-size",       1, 0,  702},
                                          {"tgtrace",      0, 0,  801},
                                          {"sfile",        1, 0,  901},
                                          {"cfile",        1, 0,  902},
                                          {"cmdl",         1, 0, 1001},                                          
                                          {"iptg",         1, 0, 1101},
                                          {"dram",         0, 0, 1201},
                                          {0,              0, 0,    0}
                                        };
  
  while (1)
  {
    opterr = 0;
    c = getopt_long(argc, argv, "ab:B:c:CdDfF:ghH:i:I:k:L:m:n:opP:r:R:sStuvw", long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
      case 301:    // --ds
        temp = atoi(optarg);
        if (temp < LOWER_CACHE_SIZE_BOUND || temp > UPPER_CACHE_SIZE_BOUND)
        {
          printf("Fatal error: Invalid value %hu for option --ds! Use -h for help\n", temp);
          exit(1);
        }
        DCACHESIZE = 1 << temp;
        break;
      case 302:    // --is
        temp = atoi(optarg);
        if (temp < LOWER_CACHE_SIZE_BOUND || temp > UPPER_CACHE_SIZE_BOUND)
        {
          printf("Fatal error: Invalid value %hu for option --is! Use -h for help\n", temp);
          exit(1);
        }
        ICACHESIZE = 1 << temp;
        break;
      case 303:    // --us
        temp = atoi(optarg);
        if (temp < LOWER_CACHE_SIZE_BOUND || temp > UPPER_CACHE_SIZE_BOUND)
        {
          printf("Fatal error: Invalid value %hu for option --us! Use -h for help\n", temp);
          exit(1);
        }
        UCACHESIZE = 1 << temp;
        break;
      case 304:    // --dt
        DCACHEWAYS = atoi(optarg); // results in a valid number since variable is unsigned
        if (DCACHEWAYS == 0)
          DCACHETYPE = FASSOC;
        else
          if (DCACHEWAYS == 1)
            DCACHETYPE = DIRECT;
          else
            if (validAssoc(DCACHEWAYS))
              DCACHETYPE = SETASSOC;
            else
            {
              printf("Fatal error: Invalid value %u for option --dt! Use -h for help\n", DCACHEWAYS);
              exit(1);
            }
        break;
      case 305:    // --it
        ICACHEWAYS = atoi(optarg); // results in a valid number since variable is unsigned
        if (ICACHEWAYS == 0)
          ICACHETYPE = FASSOC;
        else
          if (ICACHEWAYS == 1)
            ICACHETYPE = DIRECT;
          else
            if (validAssoc(ICACHEWAYS))
              ICACHETYPE = SETASSOC;
            else
            {
              printf("Fatal error: Invalid value %u for option --it! Use -h for help\n", ICACHEWAYS);
              exit(1);
            }
        break;
      case 306:    // --ut
        UCACHEWAYS = atoi(optarg); // results in a valid number since variable is unsigned
        if (UCACHEWAYS == 0)
          UCACHETYPE = FASSOC;
        else
          if (UCACHEWAYS == 1)
            UCACHETYPE = DIRECT;
          else
            if (validAssoc(UCACHEWAYS))
              UCACHETYPE = SETASSOC;
            else
            {
              printf("Fatal error: Invalid value %u for option --ut! Use -h for help\n", UCACHEWAYS);
              exit(1);
            }
        break;
      case 307:    // --dws
        DCACHE_WS = atoi(optarg); // results in a valid number since variable is unsigned
        if (DCACHE_WS > 10)
          printf("Warning: unlikely value %hu assigned to switch --dws!\n", DCACHE_WS);
        break;
      case 308:    // --iws
        ICACHE_WS = atoi(optarg); // results in a valid number since variable is unsigned
        if (ICACHE_WS > 10)
          printf("Warning: unlikely value %hu assigned to switch --iws!\n", ICACHE_WS);
        break;
      case 309:    // --uws
        UCACHE_WS = atoi(optarg); // results in a valid number since variable is unsigned
        if (UCACHE_WS > 10)
          printf("Warning: unlikely value %hu assigned to switch --uws!\n", UCACHE_WS);
        break;
      case 310:    // --Sws
        SCRATCH_WS = atoi(optarg); // results in a valid number since variable is unsigned
        if (SCRATCH_WS > 10)
          printf("Warning: unlikely value %hu assigned to switch --Sws!\n", SCRATCH_WS);
        break;
      case 401:    // --core
        switch (optarg[0])
        {
          case 'w':
          case 'W':   CURRENT_ISS = SWARM;
                      break;
          case 's':
          case 'S':   CURRENT_ISS = SIMITARM;

                      break;
          case 'p':
          case 'P':   CURRENT_ISS = POWERPC;
                      break;
          case 'l':
          case 'L':   CURRENT_ISS = LX;
                      break;
          case 't':
          case 'T':   CURRENT_ISS = TG;
                      break;
          default:    printf("Fatal error: Invalid value %c for option --core! Use -h for help\n\n", optarg[0]);
                      exit(1);
        }
        if ((CURRENT_ISS == SWARM && !HAVE_SWARM) || (CURRENT_ISS == SIMITARM && !HAVE_SIMITARM) ||
            (CURRENT_ISS == POWERPC && !HAVE_POWERPC) || (CURRENT_ISS == LX && !HAVE_LX))
        {
          printf("Fatal error: Invalid value %c for option --core: core unavailable! Use -h for help\n\n", optarg[0]);
          exit(1);
        }
        break;
      case 402:    // --intc
        switch (optarg[0])
        {
          case 'a':
          case 'A':   CURRENT_INTERC = AMBASIG;
                      break;
          case 'h':
          case 'H':   CURRENT_INTERC = AMBAAHBSYN;
                      break;
          case 'i':    
          case 'I':   CURRENT_INTERC = AMBAAHB;
                      break;
          case 'o':
          case 'O':   CURRENT_INTERC = AMBAOCCN;
                      break;
          case 's':
          case 'S':   CURRENT_INTERC = STBUS;
                      break;
          case 'x':
          case 'X':   CURRENT_INTERC = XPIPES;
                      break;
          case 'y':
          case 'Y':   CURRENT_INTERC = AMBAAXISYN;
                      break;
          default:    printf("Fatal error: Invalid value for option --intc! Use -h for help\n\n");
                      exit(1);
        }
        if ((CURRENT_INTERC == AMBASIG && !HAVE_AMBA_SIG) || (CURRENT_INTERC == AMBAOCCN && !HAVE_AMBA_OCCN) ||
            (CURRENT_INTERC == STBUS && !HAVE_STBUS) || (CURRENT_INTERC == XPIPES && !HAVE_XPIPES) ||
            (CURRENT_INTERC == AMBAAHBSYN && !HAVE_AMBA_AHB_SYN) || (CURRENT_INTERC == AMBAAXISYN && !HAVE_AMBA_AXI_SYN))
        {
          printf("Fatal error: Invalid value %c for option --intc: interconnect unavailable! Use -h for help\n\n", optarg[0]);
          exit(1);
        }
        break;
#ifndef NOSNOOP
      case 501:    // --snoop
  #ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"snoop\" disabled in the preview release!!\n\n\n");
        exit(1);
  #endif
        SNOOPING = SNOOPING ? 0 : 1;
        break;
      case 502:    // --snoop-policy
  #ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"snoop-policy\" disabled in the preview release!!\n\n\n");
        exit(1);
  #endif
        switch (optarg[0])
        {
          case 'i':
            SNOOP_POLICY = SNOOP_INVALIDATE;
            break;
          case 'u':
            SNOOP_POLICY = SNOOP_UPDATE;
            break;
          default:
            printf("Fatal error: Invalid value for option --snoop-policy! Use -h for help\n\n");
            exit(1);
        }
        break;
#endif
#ifdef WB_CACHE
      case 503:    // --cwp
        switch (optarg[0]) {
        case 't':
          CACHE_WRITE_POLICY = WT;
          DCACHE_DIRTY_BITS = 1;
          break;
        case 'b':
          CACHE_WRITE_POLICY = WB;
          DCACHE_DIRTY_BITS = 2;
          break;
        default:
          printf("Fatal error: Invalid value for option --cwp! Use -h for help\n\n");
          exit(1);
        }
        break;
#endif
      case 601:    // --i-s
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"i-s\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        ISCRATCH = !ISCRATCH;
        break;
      case 602:    // --i-size
        temp = atoi(optarg);
        if (temp == 0)
        {
          printf("Fatal error: Invalid value %hu for option --i-size! Use -h for help\n", temp);
          exit(1);
        }
        ISCRATCHSIZE = 1 << temp;
        break;
      case 701:    // --s-size
        temp = atoi(optarg);
        if (temp < LOWER_SCRATCH_SIZE_BOUND || temp > UPPER_SCRATCH_SIZE_BOUND)
        {
          printf("Fatal error: Invalid value %hu for option --s-size! Use -h for help\n", temp);
          exit(1);
        }
        SCRATCH_SIZE = 1 << temp;
        break;
      case 702:    // --q-size
        temp = atoi(optarg);
        if (temp < LOWER_SCRATCH_SIZE_BOUND || temp > UPPER_SCRATCH_SIZE_BOUND)
        {
          printf("Fatal error: Invalid value %hu for option --q-size! Use -h for help\n", temp);
          exit(1);
        }
        QUEUE_SIZE = 1 << temp;
        break;
      case 801:    // --tgtrace
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"tgtrace\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        TG_TRACE_COLLECTION = !TG_TRACE_COLLECTION;
        break;
      case 901:    // --sfile
        STATSFILENAME = optarg;
        break;
      case 902:    // --cfile
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"--cfile\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        CFGFILENAME = optarg;
        break;
      case 1001:   // --cmdl
        length = strlen(optarg);
        stringbuff = new char [length + 1];           // buffer long enough to prevent any overflow
        *new_argv = new char* [length];               // pointer array certainly having enough locations
        found_args = 0;
        
        for (unsigned int sweepstring = 0; sweepstring < length; sweepstring++)
        {
          if (optarg[sweepstring] != ' ')
          {
            found_args++;

            unsigned int copystring = 0;
            do
            {
              stringbuff[copystring] = optarg[sweepstring];
              copystring++;
              sweepstring++;
            } while ((sweepstring < length) && (optarg[sweepstring] != ' '));
            stringbuff[copystring] = '\0';

            (*new_argv)[found_args - 1] = new char [strlen(stringbuff)];
            strcpy((*new_argv)[found_args - 1], stringbuff);
          }
        }
        *new_argc = found_args;
        for (uint k = 0; k < (uint)(*new_argc); k++)
          printf("argc = %u, argv[%u] = %s\n", (uint)(*new_argc), k, (*new_argv)[k]);
        break;
      case 1101:   // --iptg
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"--iptg\" disabled in the preview release!!\n\n\n");
        exit(1);        
#endif
        N_IP_TG = atoi(optarg);
        if (N_IP_TG < 1 || N_IP_TG > 31)
        {
          printf("Fatal error: Invalid value %u for option --iptg! Use -h for help\n", N_IP_TG);
          exit(1);
        }
        break;
      case 1201:   // --dram
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"--dram\" disabled in the preview release!!\n\n\n");
        exit(1);        
#endif
        DRAM = !DRAM;
        break;    
      case 'a':
        AUTOSTARTMEASURING = !AUTOSTARTMEASURING;
        break;
      case 'b':
        MEM_BB_WS = atoi(optarg); // results in a valid number since variable is unsigned
        break;
      case 'B':
        N_BUSES = atoi(optarg); // results in a valid number since variable is unsigned
        if (N_BUSES == 0)
        {
          printf("Fatal error: Invalid value %u for option -B! Use -h for help\n", N_BUSES);
          exit(1);
        }
        break;
      case 'c':
        N_CORES = atoi(optarg);
        if (N_CORES < 1 || N_CORES > 31)
        {
          printf("Fatal error: Invalid value %u for option -c! Use -h for help\n", N_CORES);
          exit(1);
        }
        break;
      case 'C':
        CORESLAVE = !CORESLAVE;
        break;
      case 'd':
        SHOWPROMPT = !SHOWPROMPT;
        break;
      case 'D':
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"D\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        DMA = !DMA;
        break;
      case 'f':
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"f\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        FREQSCALINGDEVICE = !FREQSCALINGDEVICE;
        break;
      case 'F':
//FIXME ugly I_DIV; no toggle; assumes N_CORES to be already set; improve syntax checking; M_DIV, I_DIV not initializable
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"F\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        unsigned short int frqtarget, frequency_divider;
        length = strlen(optarg);
        stringbuff = (char *)malloc(length + 1);
        found_args = 0;
        N_FREQ_DEVICE = N_CORES;
        frqtarget = 0;
        frequency_divider = 0;
        
        if (!M_DIVIDER)
        {
          M_DIVIDER = new unsigned short int [N_FREQ_DEVICE];
          for (unsigned short int i = 0; i < N_FREQ_DEVICE; i ++)
            M_DIVIDER[i] = 1;
        }
        
        if (!I_DIVIDER)
        {
          I_DIVIDER = new unsigned short int [N_BUSES];
          for (unsigned short int i = 0; i < N_BUSES; i ++)
            I_DIVIDER[i] = 1;
        }
        
        for (unsigned int sweepstring = 0; sweepstring < length; sweepstring ++)
        {
          if (optarg[sweepstring] != ',')
          {
            found_args ++;
            unsigned int copystring = 0;
            do
              stringbuff[copystring ++] = optarg[sweepstring ++];
            while ((sweepstring < length) && (optarg[sweepstring] != ','));
            stringbuff[copystring] = '\0';
          }

          if (found_args == 1)
            frqtarget = atoi(stringbuff);
          else if (found_args == 2)
          {
            frequency_divider = atoi(stringbuff);

            if (frqtarget >= N_FREQ_DEVICE + N_BUSES)
            {
              printf("Fatal error: Invalid frequency target %hu for option -F (%hu devices and %hu buses can be set)! Use -h for help\n",
                frqtarget, N_FREQ_DEVICE, N_BUSES);
              exit(1);
            }
            if (frequency_divider <= 0)
            {
              printf("Fatal error: Invalid frequency for option -F! Use -h for help\n");
              exit(1);
            }

            if (frqtarget < N_FREQ_DEVICE)
            {
              M_DIVIDER[frqtarget] = frequency_divider;
              printf("Setting the frequency divider of Master %hu to %u\n",
                frqtarget, M_DIVIDER[frqtarget]);
            }
            else
            {
              I_DIVIDER[frqtarget - N_FREQ_DEVICE] = frequency_divider;
              printf("Setting the frequency divider of interconnect %hu to %u\n",
                frqtarget - N_FREQ_DEVICE, I_DIVIDER[frqtarget - N_FREQ_DEVICE]);
            }
          }
          else
          {
            printf("Fatal error: Invalid value for option -F! Use -h for help\n");
            exit(1);
          }
        }
        FREQSCALING = true;
        break;
      case 'g':
        N_STORAGE = atoi(optarg);
        break;
      case 'h':
        if (argc > 2)
        {
          puts("Please use the -h/--help option alone on the command line!");
          exit(1);
        }
#ifdef REDUCED_RELEASE
        printf("\n\n\n--------WARNING--------\nThis is a PREVIEW version of the simulator, with many features disabled!\n");
        printf("Please contact the project lead for a full version release:\n");
        printf("Prof. Luca Benini\nDEIS-Universita' di Bologna\nViale Risorgimento, 2\n40136 Bologna (ITALY)\n");
        printf("lbenini@deis.unibo.it\nTel +39 051 2093782\nFax +39 051 2093785\n\n\n\n");
#endif
        printf("\n\nAvailable options:\n");
        printf("-a          Toggles statistics collection since boot (default %s)\n", AUTOSTARTMEASURING ? "on" : "off");
        printf("-b x        Sets the wait states of memories (back-to-back) to x (default %hu)\n", MEM_BB_WS);
        printf("-B x        Sets the platform as having x interconnection buses (min 1) (default %hu)\n", N_BUSES);
        printf("-c x        Sets the platform as having x cores (max 31) (default %hu)\n", N_CORES);
        printf("--cfile=x   Gets platform configuration from file \"x\" (default \"%s\")\n", CFGFILENAME.c_str());
        printf("--cmdl=\"x\"  Defines command line parameters to be passed to the underlying ISSs (default \"\")\n");
        printf("--core=x    Sets the ISS to use as the core to x ('w'=SWARM, 's'=SimIt-ARM, 'p'=PowerPC,\n");
        printf("            'l'=LX, 't'=Traffic Gen) (available: %s, %s, %s, %s, %s) (default %s)\n",
          HAVE_SWARM ? "yes" : "no",
          HAVE_SIMITARM ? "yes" : "no",
          HAVE_POWERPC ? "yes" : "no",
          HAVE_LX ? "yes" : "no",
          HAVE_TG ? "yes" : "no",
          CURRENT_ISS == SWARM ? "SWARM" :
          (CURRENT_ISS == SIMITARM ? "SimIt-ARM" :
          (CURRENT_ISS == POWERPC ? "PowerPC" :
          (CURRENT_ISS == LX ? "LX" : "TG"))));
#ifdef WB_CACHE
        printf("--cwp=x     Sets the Cache Write Policy to x ('t' = Write Through, 'b' = Write Back)\n");
        printf("            (default %s)\n", (CACHE_WRITE_POLICY == WT) ? "Write Through" : "Write Back");
#endif
        printf("-C          Toggles the presence of core-associated slaves (default %s) (if enabled, implies -S enabled)\n",
          CORESLAVE ? "enabled" : "disabled");
        printf("-d          Toggles the prompt to connect to /dev/pts devices (default %s)\n", SHOWPROMPT ? "on" : "off");
        printf("--dram      Toggles the presence of a DRAM controller device (default %s)\n", DRAM ? "enabled" : "disabled");
        printf("--ds=x      Sets the D-Cache size to 2^x bytes (ignored if unified cache is selected)\n");
        printf("            (%hu <= x <= %hu) (default %lu bytes)\n", LOWER_CACHE_SIZE_BOUND, UPPER_CACHE_SIZE_BOUND, DCACHESIZE);
        printf("--dt=x      Sets the D-Cache type (ignored if unified cache is selected) (default %hu)\n",
          DCACHETYPE == FASSOC ? 0 : (DCACHETYPE == DIRECT ? 1 : DCACHEWAYS));
        printf("            x=0: fully associative; x=1: direct mapped; %hu <= x <= %hu: x-way set associative\n",
          1<<LOWER_ASSOC_BOUND, 1<<UPPER_ASSOC_BOUND);
        printf("--dws=x     Sets the D-Cache wait states (ignored if unified cache is selected) (default %hu)\n",
          DCACHE_WS);
        printf("-D          Toggles the presence of DMA controllers (if enabled, implies -S enabled) (default %s)\n",
          DMA ? "enabled" : "disabled");
        printf("-f          Instantiates support for dynamic frequency scaling (default: %s)\n",
          FREQSCALINGDEVICE ? "dynamic" :
          (FREQSCALING ? "static only" : "isofrequential system"));
        printf("-F x,y      Enables static frequency assignment and sets the frequency of device \"x\" to be \"1/y\" the base\n");
        printf("            system frequency (low IDs: masters, high IDs: buses) (default: %s)\n",
          FREQSCALING ? "enabled" : "disabled");
        printf("-g x        Sets the platform as having x storage devices (default %hu)\n", N_STORAGE);
        printf("-h, --help  Prints this output\n");
        printf("-H x        Sets the platform as having x shared memories (default %hu)\n", N_SHARED);
        printf("-i x        Sets the wait states of the interrupt slave to x (default %hu)\n", INT_WS);
        printf("--intc=x    Sets the interconnection to use to x ('a'=AMBA AHB (signal), 'h'=AMBA AHB (Synopsys),\n");
        printf("            'i'=AMBA AHB (interfaces), 'o'=AMBA AHB (OCCN), 's'=STBus, 'x'=xpipes, 'y'=AMBA AXI (Synopsys))\n");
        printf("            (available: %s, %s, %s, %s, %s, %s, %s) (default %s)\n",
          HAVE_AMBA_SIG ? "yes" : "no",
          HAVE_AMBA_AHB_SYN ? "yes" : "no",
          HAVE_AMBA_AHB ? "yes" : "no",
          HAVE_AMBA_OCCN ? "yes" : "no",
          HAVE_STBUS ? "yes" : "no",
          HAVE_XPIPES ? "yes" : "no",
          HAVE_AMBA_AXI_SYN ? "yes" : "no",
          CURRENT_INTERC == STBUS ? "STBus" :
          (CURRENT_INTERC == AMBASIG ? "AMBA AHB (signal)" :
          (CURRENT_INTERC == AMBASIG ? "AMBA AHB (interfaces)" :
          (CURRENT_INTERC == AMBAOCCN ? "AMBA AHB (OCCN)" :
          (CURRENT_INTERC == AMBAAHBSYN ? "AMBA AHB (Synopsys)" :
          (CURRENT_INTERC == AMBAAXISYN ? "AMBA AXI (Synopsys)" : "xpipes"))))));
        printf("--iptg x    Sets the platform as having x STM IPTG traffic generators (1 <= x <= 31) (default %hu)\n", N_IP_TG);    
        printf("--i-s       Toggles the presence of instruction scratchpad memories (default %s)\n",
          ISCRATCH ? "enabled" : "disabled");
        printf("--i-size=x  Sets the instruction scratchpad size to 2^x bytes (> 0) (default %lu bytes)\n", ISCRATCHSIZE);
        printf("--is=x      Sets the I-Cache size to 2^x bytes (ignored if unified cache is selected)\n");
        printf("            (%hu <= x <= %hu) (default %lu bytes)\n", LOWER_CACHE_SIZE_BOUND, UPPER_CACHE_SIZE_BOUND, ICACHESIZE);
        printf("--it=x      Sets the I-Cache type (ignored if unified cache is selected) (default %hu)\n",
          ICACHETYPE == FASSOC ? 0 : (ICACHETYPE == DIRECT ? 1 : ICACHEWAYS));
        printf("            x=0: fully associative; x=1: direct mapped; %hu <= x <= %hu: x-way set associative\n",
          1<<LOWER_ASSOC_BOUND, 1<<UPPER_ASSOC_BOUND);
        printf("--iws=x     Sets the I-Cache wait states (ignored if unified cache is selected) (default %hu)\n",
          ICACHE_WS);
        printf("-I x        Sets the platform as having x interrupt devices (default %hu)\n", N_INTERRUPT);
        printf("-k x        Sets the platform as having x Smart Memories (memories including a DMA) (default %hu) (max %hu)\n",
          N_SMARTMEM, SMARTMEM_MAX_CONF);
        printf("-L x,y      Sets the PLL delay for frequency scaling of device \"x\" to be \"y\" master system\n");
        printf("            clock cycles (low IDs: masters, high IDs: buses) (default: 100)\n");
        printf("-m x        Sets the wait states of memories (initial) to x (default %hu)\n", MEM_IN_WS);
        printf("-n x        Sets simulation duration to x clock cycles (-1 = endlessly) (default %ld)\n", NSIMCYCLES);
        printf("-o          Toggles the usage of OCP where applicable (default %s)\n", USING_OCP ? "used" : "unused");
        printf("-p          Toggles a partitioned SPM (if on, implies -s and -S on) (default %s)\n", SPCHECK ? "on" : "off");
        printf("-P x        Sets the platform as having x semaphore memories (default %hu)\n", N_SEMAPHORE);
        printf("--q-size=x  Sets the queue memory size to 2^x bytes (ignored if -C is disabled)\n");
        printf("            (%hu <= x <= %hu) (default %lu bytes)\n", LOWER_SCRATCH_SIZE_BOUND, UPPER_SCRATCH_SIZE_BOUND, QUEUE_SIZE);
        printf("-r x        Sets the platform as having x private memories (default: as many as the cores)\n");
        printf("-R x        Sets the platform as having x bridges between interconnects (default %hu)\n", N_BRIDGES);
        printf("-s          Toggles advanced statistics (default %s)\n", STATS ? "on" : "off");
        printf("--sfile=x   Outputs statistics on file \"x\" (default \"%s\")\n", STATSFILENAME.c_str());
#ifndef NOSNOOP
        printf("--snoop     Toggles snooping on bus for cache coherence (default %s)\n", SNOOPING ? "on" : "off");
        printf("--snoop-policy x  Sets the snooping policy to x ('u'=update, 'i'=invalidate) (default %s)\n", 
                 SNOOP_POLICY == SNOOP_INVALIDATE ? "invalidate" : (SNOOP_POLICY == SNOOP_UPDATE ? "update" : "NOT-VALID-ERROR") );
#endif
        printf("--s-size=x  Sets the scratchpad memory size to 2^x bytes (ignored if -S is disabled)\n");
        printf("            (%hu <= x <= %hu) (default %lu bytes)\n", LOWER_SCRATCH_SIZE_BOUND, UPPER_SCRATCH_SIZE_BOUND, SCRATCH_SIZE);
        printf("-S          Toggles the presence of scratchpad memories (default %s)\n", SCRATCH ? "enabled" : "disabled");
        printf("--Sws=x     Sets the scratchpad memory wait states (default %hu)\n", SCRATCH_WS);
        printf("-t          Toggles traces of memory accesses (if on, implies -s on) (default %s)\n",
          ACCTRACE ? "on" : "off");
        printf("-u          Toggles unified vs split cache memory (default %s)\n", SHARED_CACHE ? "unified" : "split");
        printf("--us=x      Sets the Unified Cache size to 2^x bytes (ignored if split cache is selected)\n");
        printf("            (%hu <= x <= %hu) (default %lu bytes)\n", LOWER_CACHE_SIZE_BOUND, UPPER_CACHE_SIZE_BOUND, UCACHESIZE);
        printf("--ut=x      Sets the Unified Cache type (ignored if split cache is selected) (default %hu)\n",
          UCACHETYPE == FASSOC ? 0 : (UCACHETYPE == DIRECT ? 1 : UCACHEWAYS));
        printf("            x=0: fully associative; x=1: direct mapped; %hu <= x <= %hu: x-way set associative\n",
          1<<LOWER_ASSOC_BOUND, 1<<UPPER_ASSOC_BOUND);
        printf("--uws=x     Sets the Unified Cache wait states (ignored if split cache is selected) (default %hu)\n",
          UCACHE_WS);
        printf("--tgtrace   Toggles collection of traces for TG use (default %s)\n", TG_TRACE_COLLECTION ? "on" : "off");
        printf("-v          Toggles VCD waveforms of interconnection signals (default %s)\n", VCD ? "on" : "off");
        printf("-w          Toggles power statistics (if on, implies -s on) (default %s)\n", POWERSTATS ? "on" : "off");
        puts("\n");
        exit(1);
      case 'H':
        N_SHARED = atoi(optarg);
        break;
      case 'i':
        INT_WS = atoi(optarg); // results in a valid number since variable is unsigned
        break;
      case 'I':
        N_INTERRUPT = atoi(optarg);
        break;
      case 'k':
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"k\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        N_SMARTMEM = atoi(optarg);
        if (N_SMARTMEM > SMARTMEM_MAX_CONF)
        {
          printf("Fatal Error: Invalid value %hu for option -k! Use -h for help\n", N_SMARTMEM);
          exit(1);
        }
        SMARTMEM = !SMARTMEM;
        break;
      case 'L':
//FIXME ugly I_DIV; no toggle; assumes N_CORES to be already set; improve syntax checking; PLL_DELAY not initializable
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"p\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        unsigned short int plltarget, pll_delay;
        length = strlen(optarg);
        stringbuff = (char *)malloc(length + 1);
        found_args = 0;
        N_MASTERS = N_CORES;
        plltarget = 0;
        pll_delay = 0;
        
        if (!PLL_DELAY)
        {
          PLL_DELAY = new unsigned short int [N_FREQ_DEVICE + N_BUSES];
          for (unsigned short int i = 0; i < N_FREQ_DEVICE + N_BUSES; i ++)
            PLL_DELAY[i] = 100;
        }
        
        for (unsigned int sweepstring = 0; sweepstring < length; sweepstring ++)
        {
          if (optarg[sweepstring] != ',')
          {
            found_args ++;
            unsigned int copystring = 0;
            do
              stringbuff[copystring ++] = optarg[sweepstring ++];
            while ((sweepstring < length) && (optarg[sweepstring] != ','));
            stringbuff[copystring] = '\0';
          }

          if (found_args == 1)
            plltarget = atoi(stringbuff);
          else if (found_args == 2)
          {
            pll_delay = atoi(stringbuff);
            if (plltarget >= N_FREQ_DEVICE + N_BUSES)
            {
              printf("Fatal error: Invalid PLL delay target %hu for option -p (%hu devices and %hu buses can be set)! Use -h for help\n",
                plltarget, N_FREQ_DEVICE, N_BUSES);
              exit(1);
            }
            PLL_DELAY[plltarget] = pll_delay;
            printf("Setting the PLL delay of device %hu to %u\n", plltarget, PLL_DELAY[plltarget]);
          }
          else
          {
            printf("Fatal error: Invalid value for option -p! Use -h for help\n");
            exit(1);
          }
        }
        break;
      case 'm':
        MEM_IN_WS = atoi(optarg); // results in a valid number since variable is unsigned
        break;
      case 'n':
        NSIMCYCLES = atoi(optarg);
        if (NSIMCYCLES < 1)
        {
          printf("Fatal error: Invalid value %ld for option -n! Use -h for help\n", NSIMCYCLES);
          exit(1);
        }
        break;
      case 'o':
        USING_OCP = !USING_OCP;
        break;
      case 'p':
        SPCHECK = !SPCHECK;
        break;
      case 'P':
        N_SEMAPHORE = atoi(optarg); // results in a valid number since variable is unsigned
        break;
      case 'r':
        N_PRIVATE = atoi(optarg); // results in a valid number since variable is unsigned
        private_memories_explicitly_set = true;
        break;
      case 'R':
        N_BRIDGES = atoi(optarg); // results in a valid number since variable is unsigned
        break;
      case 's':
        STATS = !STATS;
        break;
      case 'S':
        SCRATCH = !SCRATCH;
        break;
      case 't':
        ACCTRACE = !ACCTRACE;
        break;
      case 'u':
        SHARED_CACHE = !SHARED_CACHE;
        break;
      case 'v':
        VCD = !VCD;
        break;
      case 'w':
#ifdef REDUCED_RELEASE
        printf("\n\nError: feature \"w\" disabled in the preview release!!\n\n\n");
        exit(1);
#endif
        POWERSTATS = !POWERSTATS;
        break;
      case '?':
        printf("Fatal error: Unrecognized option -%c! A numeric parameter to it may be missing. Use -h for help\n", optopt);
        exit(1);
      default:
        printf("Fatal error: getopt() returned unexpected character code %d! Use -h for help\n", c);
        exit(1);
    }
  } //end of while

  if (optind < argc)
  {
    printf ("Fatal error: Detected invalid command line parameters ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    printf ("! Use -h for help\n");
    exit(1);
  }

  if (DMA)
  {
    SCRATCH = true;
    N_MASTERS = N_CORES * 2;
  }
  else
    N_MASTERS = N_CORES;

  if (SMARTMEM)
    N_MASTERS += N_SMARTMEM;

  if (N_FFT > 0)
    N_MASTERS += N_FFT;
    
#if defined DRAMBUILD
  if (DRAM)
    N_MASTERS += 1;
#endif
    
  if (FREQSCALINGDEVICE)
    N_FREQ = 1;
  else
    N_FREQ = 0;
    
  if (FREQSCALING && !FREQSCALINGDEVICE)
    puts("Warning: Only static frequency scaling will be allowed in this system!\n");

  if (!private_memories_explicitly_set)
    N_PRIVATE = N_CORES;
  else
    if (N_PRIVATE != N_CORES)
      printf("Warning: The number of instantiated system cores (%hu) and private memories (%hu) does not match!\n", N_CORES, N_PRIVATE);
    
  //Poletti FIXME currently not allowed to instatiate both LX cores and ARM cores 
  if(CURRENT_ISS == LX)
  {
    N_LX_PRIVATE = N_CORES;
    N_PRIVATE = 0;
  }
  else
    N_LX_PRIVATE = 0;
    
  if (CORESLAVE)
  {
    N_CORESLAVE = N_CORES;
    SCRATCH = true;
  }
  else
    N_CORESLAVE = 0;

  N_SLAVES = N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE + N_STORAGE + N_FREQ + N_SMARTMEM + N_FFT;
  
#ifdef DRAMBUILD
  if (DRAM)
  {
    N_SLAVES += 1;
    if(CURRENT_ISS != LX) 
      printf("Warning: DRAM instantiated without LX core\n");
  }
#endif
  
  NUMBER_OF_EXT = N_INTERRUPT + N_SMARTMEM*SMARTMEM_MAX_OBJ + (CORESLAVE);
  
  SMARTMEM_DMA_SIZE = N_CORES * (4 + (7 * 4 * SMARTMEM_MAX_OBJ));
  
  DRAMDMA_SIZE = N_CORES * (4 + (7 * 4 * DRAMDMA_MAX_OBJ));
  
  if (SHARED_CACHE && (UCACHESIZE < UCACHEWAYS * (uint)16))
  {
    printf ("Fatal error: Detected invalid command line parameters. You cannot\n");
    printf ("instantiate a %hu-way set-associative unified cache, having 16 byte lines,\n", UCACHEWAYS);
    printf ("with a memory having just %lu byte size!\n", UCACHESIZE);
    exit(1);
  }
  else if (!SHARED_CACHE && (DCACHESIZE < DCACHEWAYS * (uint)16))
       {
         printf ("Fatal error: Detected invalid command line parameters. You cannot\n");
         printf ("instantiate a %hu-way set-associative D-Cache, having 16 byte lines,\n", DCACHEWAYS);
         printf ("with a memory having just %lu byte size!\n", DCACHESIZE);
         exit(1);
       }
       else if (!SHARED_CACHE && (ICACHESIZE < ICACHEWAYS * (uint)16))
            {
              printf ("Fatal error: Detected invalid command line parameters. You cannot\n");
              printf ("instantiate a %hu-way set-associative I-Cache, having 16 byte lines,\n", ICACHEWAYS);
              printf ("with a memory having just %lu byte size!\n", ICACHESIZE);
              exit(1);
            }
  
  if (SPCHECK)
    SCRATCH = true;

  if (ACCTRACE || SPCHECK || POWERSTATS)
    STATS = true;
  
  if (CURRENT_INTERC == XPIPES)
    USING_OCP = true;
    
#ifndef WB_CACHE
  CACHE_WRITE_POLICY = WT;
  DCACHE_DIRTY_BITS = 1;
#endif
    
  // -----------------------------------------------------
  // If provided, parse a configuration file at this point
  // -----------------------------------------------------
  if (strcmp(CFGFILENAME.c_str(), ""))
  {
    // Some settings are INCOMPATIBLE with the cfg file for the time being!!
    // TODO implement such support
    if (DMA || SMARTMEM || N_FFT > 0 || N_STORAGE > 0 || CORESLAVE || DRAM)
    {
      printf ("Fatal error: when using a configuration file, you cannot instantiate DMAs, smart memories, FFT devices,\n");
      printf ("             storage devices, core-associated slaves, DRAM controllers.\n");
      exit(1);
    }
    
    // FIXME Add a proper list of actually overridden settings before choosing to print this warning
    // FIXME only allow on amba-ahb
    // if (N_CORES N_PRIVATE N_SHARED N_SEMAPHORE N_INTERRUPT CORESLAVE FREQSCALING)*/
    {
      printf ("Warning: when using a configuration file, the command line settings about the number of cores, private memories,\n");
      printf ("         shared memories, semaphore devices, interrupt devices, bridges and those about frequency scaling will be overridden.\n");
    }

    parseCfgFile();
    
    // All of the following arrays should have been set if the parsing went fine
    ASSERT(M_DIVIDER && I_DIVIDER && MASTER_CONFIG && SLAVE_CONFIG && (N_BRIDGES == 0 || BRIDGE_CONFIG));
  }
  else
  {
    // Explicit topology configuration can only be done with a
    // configuration file. If that is present, all of the code below
    // is useless. Otherwise, this code must set safe default values.
    // If bridges are missing, all of the masters and slaves will be
    // on bus 0. If one bridge is present, all masters are on bus 0,
    // all slaves are on bus 1, and the bridge is monodirectional.
    // No more bridges are supported without a configuration file.
    if (!MASTER_CONFIG)
    {
      MASTER_CONFIG = new MASTER_CONF [N_MASTERS];
      for (unsigned short int j = 0; j < N_MASTERS; j ++)
        MASTER_CONFIG[j].binding = 0;
    }
    
    if (!SLAVE_CONFIG)
    {
      SLAVE_CONFIG = new SLAVE_CONF [N_SLAVES];
      for (unsigned short int j = 0; j < N_SLAVES; j ++)
      {
        if (N_BRIDGES == 0)
          SLAVE_CONFIG[j].binding = 0;
        else
          SLAVE_CONFIG[j].binding = 1;
        SLAVE_CONFIG[j].range_set = false;
      }
    }
    
    // FIXME no check for buses isolated from the system due to lack of bridges
    if (N_BRIDGES > 0 && N_BUSES == 1)
    {
      printf ("Fatal error: %hu bus bridges should be instantiated, but only one bus is present!\n", N_BRIDGES);
      exit(1);
    }
    
    if (N_BRIDGES >= 2)
    {
      printf ("Fatal error: %hu bus bridges should be instantiated, but only one is supported without a configuration file!\n", N_BRIDGES);
      exit(1);
    }
    
    if (N_BRIDGES > 0 && !BRIDGE_CONFIG)
    {
      BRIDGE_CONFIG = new BRIDGE_CONF [N_BRIDGES];
      for (unsigned short int j = 0; j < N_BRIDGES; j ++)
      {
        BRIDGE_CONFIG[j].master_binding = 1;
        BRIDGE_CONFIG[j].slave_binding = 0;
        
        BRIDGE_CONFIG[j].n_ranges = 1;
        BRIDGE_CONFIG[j].start_address = new uint32_t [1];
        BRIDGE_CONFIG[j].end_address = new uint32_t [1];
        BRIDGE_CONFIG[j].start_address[0] = 0x00000000;
        BRIDGE_CONFIG[j].end_address[0]   = 0xffffffff;
      }
    }
  }
    
  // If nobody explicitly set frequency scaling, take care of it now
  // by setting safe dividers (i.e. set them to '1') and safe PLL
  // delays
  if (!M_DIVIDER)
  {
     M_DIVIDER = new unsigned short int [N_FREQ_DEVICE];
     for (unsigned short int i = 0; i < N_FREQ_DEVICE; i ++)
       M_DIVIDER[i] = 1;
  }
  
  if (!I_DIVIDER)
  {
    I_DIVIDER = new unsigned short int [N_BUSES];
    for (unsigned short int i = 0; i < N_BUSES; i ++)
      I_DIVIDER[i] = 1;
  }
  if (!PLL_DELAY)
  {
    PLL_DELAY = new unsigned short int [N_FREQ_DEVICE + N_BUSES];
    for (unsigned short int i = 0; i < N_FREQ_DEVICE + N_BUSES; i ++)
      PLL_DELAY[i] = 100;
  }

}
