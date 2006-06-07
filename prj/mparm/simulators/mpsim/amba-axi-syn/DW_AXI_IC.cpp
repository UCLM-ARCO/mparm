// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Interconnection (IC) Class
//   - Top level class implementing the IC
//   - Configurable with respect to:
//     . Number of IC lines (Address, Write data, Read data, Response lines)
//     . Access to IC line can be configured for each master/slave individually
//     . Priorities can be set for each master on IC line basis
//   - Master priorites can be changed during run-time
// ============================================================================

#include "DW_AXI_IC.h"

using namespace axi_namespace;

template <bool NegEdge, bool Verbose> DW_AXI_IC<NegEdge, Verbose>::
DW_AXI_IC(sc_module_name Name
      , int IC_ID
      , int IC_Priority
      , int IC_IntrlDepth
      , int NumAlines
      , int NumWlines
      , int NumRlines
      , int NumBlines
      , const sc_string& ConfigFileName
      , int SlaveID
      , int PriorityStartAddr
      , int PriorityEndAddr
      , const sc_string& SlaveFileName
      , bool BigEndian
      , bool DefaultSlave
      , int DataBusBytes         // number of data bus byte lanes
      , int AID_Width
      , double ClockFrequency
      , int MaxFIFO_Depth
      , bool FIFO_MonitorOn
      )
      : sc_module           (Name)
      , SlaveP              (0)
      , MasterP             (0)
      , m_Name              (Name)
      , m_IC_ID             (IC_ID)
      , m_IC_Priority       (IC_Priority)
      , m_IC_IntrlDepth     (IC_IntrlDepth)
      , m_NumAlines         (NumAlines)
      , m_NumWlines         (NumWlines)
      , m_NumRlines         (NumRlines)
      , m_NumBlines         (NumBlines)
      , m_ConfigFileName    (ConfigFileName)
      , m_DataBusBytes      (DataBusBytes)
      , m_AID_Width         (AID_Width)
      , m_ClockFrequency    (ClockFrequency)
      , m_MaxFIFO_Depth     (MaxFIFO_Depth)
      , m_FIFO_MonitorOn    (FIFO_MonitorOn)
      , m_IsInitialized     (false)
      , m_IC_GetSlaveParam  (false)
      , m_Reset             (false)
      , m_Step              (0)
      , m_Aline             (0)
      , m_Wline             (0)
      , m_Rline             (0)
      , m_Bline             (0)
      , m_SPIDtoMID        (0)
      , m_MPIDtoSID        (0)
      , m_SIDtoMPID        (0)
      , m_MIDtoSPID        (0)
{
  sc_assert((0 <= IC_ID) && (IC_ID < AXI_MAX_PORT_SIZE));
  sc_assert(IC_Priority >= 0);
  sc_assert(IC_IntrlDepth == 1);
  sc_assert(NumAlines >= 1);
  sc_assert(NumWlines >= 1);
  sc_assert(NumRlines >= 1);
  sc_assert(NumBlines >= 1);
  sc_assert(DataBusBytes >= 1);
  sc_assert((0 <= AID_Width) && (AID_Width <= 12));

  m_DataBusWords = (DataBusBytes < AXI_WORDSIZE_BYTES) ?
                   1 : DataBusBytes / AXI_WORDSIZE_BYTES;
  m_SlaveIF = new AXI_SlaveIFimpl<NegEdge>("IC_Sl", m_IC_ID,
                  AXI_MAX_PORT_SIZE, AXI_IM_IC, m_DataBusBytes, 
                  m_DataBusWords, m_ClockFrequency);
  m_SlaveIF->Aresetn(Aresetn);
  m_MasterIF = new AXI_MasterIFimpl<NegEdge>("IC_Ma", m_IC_ID,
                   AXI_MAX_PORT_SIZE, AXI_IM_IC, m_DataBusBytes, 
                   m_DataBusWords, m_ClockFrequency);
  m_MasterIF->Aresetn(Aresetn);

  m_HaveSlave = (DefaultSlave || (PriorityEndAddr > PriorityStartAddr)); 
  if (m_HaveSlave) {
    m_Sl = new DW_AXI_SimpleSlave<NegEdge, Verbose>("IC_Slave", SlaveID,
                              PriorityStartAddr, PriorityEndAddr,
                              SlaveFileName, BigEndian, false,
                              DefaultSlave,
                              false, DataBusBytes, ClockFrequency);
    m_Sl->SlaveP(*this);
    m_Sl->Aclk(Aclk);
    m_Sl->Aresetn(Aresetn);
    MasterP(*m_Sl);
    m_Memory = m_Sl->m_MemoryImpl->m_Memory;
  }
  else {
    int Length = AXI_MAX_IC_LINE_SIZE*AXI_MAX_PORT_SIZE * 4 / AXI_WORDSIZE_BYTES;
    m_Memory = new axi_data_bt[Length];
    for (int i = 0; i < Length; i++) m_Memory[i] = 0;
  }

  m_SlaveIF->Aclk(Aclk);
  m_MasterIF->Aclk(Aclk);

  SC_METHOD(Bus);
  sensitive << Aclk.pos();
  dont_initialize();
  SC_METHOD(Areset);
  sensitive << Aresetn;
  dont_initialize();
}

template <bool NegEdge, bool Verbose> DW_AXI_IC<NegEdge, Verbose>::
~DW_AXI_IC() {
  delete m_Step;
  delete m_SlaveIF; delete m_MasterIF;
  delete m_Aline; delete m_Wline; delete m_Rline; delete m_Bline;
  delete [] m_MPIDtoSID; delete [] m_SPIDtoMID;
  delete [] m_MIDtoSPID; delete [] m_SIDtoMPID;
  if (!m_HaveSlave) delete [] m_Memory;
}

template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
Bus()
{
  if (!m_Reset) {
    if (!m_IsInitialized) Initialize();
    m_Step->Request();
    m_Step->Response();
  }
}

// ----------------------------------------------------------------------------
// End of elaboration method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
end_of_elaboration()
{
  sc_module::end_of_elaboration();
  m_NumSports = SlaveP.size();
  m_NumMports = MasterP.size();
  // Get the slave parameters
  if (!m_IC_GetSlaveParam) IC_GetSlaveParam();
  ArbiterP->Initialize();
}

// ----------------------------------------------------------------------------
// Initialization method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
Initialize()
{
  m_IsInitialized = true;

  // Mapping from PID to ID (Master/Slave)
  // m_SIDtoMPID[0] = ID of the IC Master port 0
  // that is connected to Slave with ID = 0
  m_SIDtoMPID =  new int[AXI_MAX_PORT_SIZE];
  m_MPIDtoSID =  new int[AXI_MAX_PORT_SIZE];
  for (int i = 0; i < m_NumMports; i++) {
    m_SIDtoMPID[MasterP.GetSlaveID(i)] = i;
    m_MPIDtoSID[i] = MasterP.GetSlaveID(i);
  }
  m_MIDtoSPID =  new int[AXI_MAX_PORT_SIZE];
  m_SPIDtoMID =  new int[AXI_MAX_PORT_SIZE];
  for (int i = 0; i < m_NumSports; i++) {
    m_MIDtoSPID[SlaveP.GetMasterID(i)] = i;
    m_SPIDtoMID[i] = SlaveP.GetMasterID(i);
  }

  // Get the master parameters
  IC_GetMasterParam();

  // initialize the master/slave IC connections
  bool Mconn[AXI_MAX_IC_LINE_SIZE][AXI_MAX_PORT_SIZE];
  bool Sconn[AXI_MAX_IC_LINE_SIZE][AXI_MAX_PORT_SIZE]; 
  int NumMconn = AXI_MAX_PORT_SIZE;
  int NumSconn = AXI_MAX_PORT_SIZE;
  // initialization
  for (int i = 0; i < AXI_MAX_IC_LINE_SIZE; i++)
    for (int j = 0; j < AXI_MAX_PORT_SIZE; j++) {
      Mconn[i][j] = false; Sconn[i][j] = false;
  }
  // set default connections
  IC_Connections(Mconn, Sconn);

  // initialize priorities
  int RowLengthWords = AXI_MAX_PORT_SIZE * 4 / AXI_WORDSIZE_BYTES;
  // initialize the priority pointer:
  // P[i] = pointer to the priorities of IC line i
  GetPriorityPointer(m_Ap, m_NumAlines, RowLengthWords, 0);
  GetPriorityPointer(m_Wp, m_NumWlines, RowLengthWords, m_NumAlines);
  GetPriorityPointer(m_Rp, m_NumRlines, RowLengthWords,
                     m_NumAlines+m_NumWlines);
  GetPriorityPointer(m_Bp, m_NumBlines, RowLengthWords,
                     m_NumAlines+m_NumWlines+m_NumRlines);
  // set default priorities
  // Priority[i][j] = priority for IC line i of master with MID j
  // copy the same master priorty to all IC lines
  CopyPriorities(m_Ap, m_NumAlines, RowLengthWords);
  CopyPriorities(m_Wp, m_NumWlines, RowLengthWords);
  CopyPriorities(m_Rp, m_NumRlines, RowLengthWords);
  CopyPriorities(m_Bp, m_NumBlines, RowLengthWords);
  if (m_ConfigFileName != "") { // overwrites the default values
    GetConfiguration(Mconn, Sconn);
  }

  // Create IC line configurations (internal format)
  m_Aline = new AXI_ConfigLines("IC Line Configuration"
                , m_NumAlines
                , NumMconn, &Mconn[0] // connected initiators ID
                , NumSconn, &Sconn[0] // connected targets ID
                , m_MIDtoSPID         // mapping MID to SPID
                , m_SIDtoMPID         // mapping SID to MPID
                );
  m_Wline = new AXI_ConfigLines("IC Line Configuration"
                , m_NumWlines
                , NumMconn, &Mconn[m_NumAlines]
                , NumSconn, &Sconn[m_NumAlines]
                , m_MIDtoSPID, m_SIDtoMPID);
  m_Rline = new AXI_ConfigLines("IC Line Configuration"
                , m_NumRlines
                , NumSconn, &Sconn[m_NumAlines+m_NumWlines]
                , NumMconn, &Mconn[m_NumAlines+m_NumWlines]
                , m_SIDtoMPID, m_MIDtoSPID);
  m_Bline = new AXI_ConfigLines("IC Line Configuration"
                , m_NumBlines
                , NumSconn, &Sconn[m_NumAlines+m_NumWlines+m_NumRlines]
                , NumMconn, &Mconn[m_NumAlines+m_NumWlines+m_NumRlines]
                , m_SIDtoMPID, m_MIDtoSPID);

  // Create the main IC class
  m_Step = new AXI_IC_Request<NegEdge, Verbose>(m_Name
              , SlaveP , MasterP , m_SlaveIF , m_MasterIF
              , m_SPIDtoMID , m_MPIDtoSID , m_SIDtoMPID , m_MIDtoSPID
              , m_Aline->Line, m_Wline->Line, m_Rline->Line, m_Bline->Line
              , m_Ap, m_Wp, m_Rp, m_Bp
              , m_NumSports, m_NumMports
              , m_NumAlines, m_NumWlines, m_NumRlines, m_NumBlines
              , DecoderP, ArbiterP
              , m_DataBusBytes, m_DataBusWords, m_AID_Width
              , m_MaxFIFO_Depth, m_IntrlDepth, m_FIFO_MonitorOn
              );
}

// ----------------------------------------------------------------------------
// Get master parameters (ID, priorities and address map)
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
IC_GetMasterParam()
{
  int P;
  // initialize priorites
  for (int i = 0; i < SlaveP.size(); i++) {
    int j = m_SPIDtoMID[i];
    if (!SlaveP.GetMasterParam(i, P)) {
      cerr << "Could not get priority from master" << j << endl;
      sc_assert(0);
    }
    if (P < 0) {
      cerr << "Priority (" << P << ") of master" << j
           << " must not be negative" << endl;
      sc_assert(P >= 0);
    }
    m_Memory[j] = (axi_data_bt)P;
  }
}
// ----------------------------------------------------------------------------
// Get slave parameters (ID, priorities and address map)
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
IC_GetSlaveParam()
{
  // initialize address map
  for (int i = 0; i < MasterP.size(); i++) {
    if (!MasterP.GetSlaveParam(i, m_IntrlDepth[i], m_AddressMap)) {
      cerr << "Could not get address map from slave at port " << i << endl;
      sc_assert(0);
    }
  }
  DecoderP->Initialize(m_AddressMap);
  m_IC_GetSlaveParam = true;
}

// ----------------------------------------------------------------------------
// Configure the master/slave connections to the IC lines
// This methods implements the default configuration, namely
// all masters and slaves have access to all IC lines
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
IC_Connections(
    bool Mconn[][axi_namespace::AXI_MAX_PORT_SIZE],
    bool Sconn[][axi_namespace::AXI_MAX_PORT_SIZE])
{
  // initialize connected master;
  for (int i = 0; i < SlaveP.size(); i++) {
    int j = m_SPIDtoMID[i];
    for (int k = 0; k < AXI_MAX_IC_LINE_SIZE; k++)
      Mconn[k][j] = true;
  }
  // initialize connected slaves;
  for (int i = 0; i < MasterP.size(); i++) {
    int j = m_MPIDtoSID[i];
    for (int k = 0; k < AXI_MAX_IC_LINE_SIZE; k++)
      Sconn[k][j] = true;
  }
}

// ----------------------------------------------------------------------------
// Get priority pointer: P[i] = pointer to priorities of IC line i
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
GetPriorityPointer(axi_data_t* P, int NumLines, int RowLength, int MemoryOffset)
{
  for (int i = 0; i < NumLines; i++)
    P[i] = &m_Memory[(MemoryOffset+i) * RowLength];
}
// ----------------------------------------------------------------------------
// Same priorities on each IC line
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
CopyPriorities(axi_data_t* P, int NumLines, int RowLength)
{
  for (int i = 0; i < NumLines; i++)
   for (int j = 0; j < RowLength; j++)
     P[i][j] = m_Memory[j];
}

// ----------------------------------------------------------------------------
//  AXI IC Configuration Parser
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
GetConfiguration(
        bool Mconn[][axi_namespace::AXI_MAX_PORT_SIZE],
        bool Sconn[][axi_namespace::AXI_MAX_PORT_SIZE]
        )
{
  const int MaxLineSize = 4096;
  char buffer[MaxLineSize+2];
  char *token;
  char seps[] = " ,\t\n";

  int aln = -1;
  int wln = -1;
  int rln = -1;
  int bln = -1;
  int al = 0;
  int wl = 0;
  int rl = 0;
  int bl = 0;
  int idx = -1;
  axi_data_t pr = 0;
  int j, it, NM = 0;
  int Mi[AXI_MAX_PORT_SIZE];
  bool Bu[AXI_MAX_PORT_SIZE];

  const sc_string env_name(axi_expand(m_ConfigFileName));
  FILE* ConfigFile = fopen(env_name.c_str(), "r");
  if (ConfigFile == 0) {
    cerr << "ERROR: opening IC configuration file: " << env_name;
    cerr << " <" << name() << ">" << endl;
    sc_assert(0);
  }

  while (fgets(buffer, MaxLineSize, ConfigFile)) {
    token = strtok(buffer, seps);
    if (token != 0) {
    if (!strcmp(token, "Aline")) {
      al++; 
      if ((token = strtok( 0, seps ))) {
        if (sscanf(token,"%d",&aln) <= 0)
          ErrorMessage(1);
        if ((aln < 0) || (aln >= m_NumAlines)) {
          m_Error = aln; ErrorMessage(7);
        }
      }
      else ErrorMessage(1);
      idx = aln; pr = m_Ap[aln];
      if ((idx < 0) || (idx >= AXI_MAX_IC_LINE_SIZE))
        ErrorMessage(20);
    } else if (!strcmp(token, "Wline")) {
      wl++; 
       if ((token = strtok( 0, seps ))) {
        if (sscanf(token,"%d",&wln) <= 0)
          ErrorMessage(1);
        if ((wln < 0) || (wln >= m_NumWlines)) {
          m_Error = wln; ErrorMessage(8);
        }
      }
      else ErrorMessage(1);
      idx = m_NumAlines + wln; pr = m_Wp[wln];
      if ((idx < 0) || (idx >= AXI_MAX_IC_LINE_SIZE))
        ErrorMessage(20);
    } else if (!strcmp(token, "Rline")) {
      rl++; 
      if ((token = strtok( 0, seps ))) {
        if (sscanf(token,"%d",&rln) <= 0)
          ErrorMessage(1);
        if ((rln < 0) || (rln >= m_NumRlines)) {
          m_Error = rln; ErrorMessage(9);
        }
      }
      else ErrorMessage(1);
      idx = m_NumAlines + m_NumWlines + rln; pr = m_Rp[rln];
      if ((idx < 0) || (idx >= AXI_MAX_IC_LINE_SIZE))
        ErrorMessage(20);
    } else if (!strcmp(token, "Bline")) {
      bl++;
      if ((token = strtok( 0, seps ))) {
        if (sscanf(token,"%d",&bln) <= 0)
          ErrorMessage(1);
        if ((bln < 0) || (bln >= m_NumBlines)) {
          m_Error = bln; ErrorMessage(10);
        }
      }
      else ErrorMessage(1);
      idx = m_NumAlines + m_NumWlines + m_NumRlines + bln; pr = m_Bp[bln];
      if ((idx < 0) || (idx >= AXI_MAX_IC_LINE_SIZE))
        ErrorMessage(20);
    } else if (!strcmp(token, "Master")) {
      if (idx < 0) ErrorMessage(2);
      for (int i = 0; i < AXI_MAX_PORT_SIZE; i++) {// reset
        Bu[i] = Mconn[idx][i];
        Mconn[idx][i] = false;
      }
      j = 0;
      while ((token != 0) && (j < AXI_MAX_PORT_SIZE)) {
        if ((token = strtok( 0, seps ))) {
          if (sscanf(token,"%d",&it) > 0) {
            if ((0 <= it) && (it < AXI_MAX_PORT_SIZE)) {
              if (Bu[it]) {
                Mconn[idx][it] = true;
                Mi[j++] = it;
              } else {m_Error = it; ErrorMessage(5); }
            }
            else { m_Error = it; ErrorMessage(3); }
          }
          else token = 0;
        }
      }
      NM = j;
    } else if (!strcmp(token, "Slave")) {
      if (idx < 0) ErrorMessage(2);
      for (int i = 0; i < AXI_MAX_PORT_SIZE; i++) {// reset
        Bu[i] = Sconn[idx][i];
        Sconn[idx][i] = false;
      }
      while (token != 0) {
        if ((token = strtok( 0, seps ))) {
          if (sscanf(token,"%d",&it) > 0) {
            if ((0 <= it) && (it < AXI_MAX_PORT_SIZE)) {
              if (Bu[it]) {
                Sconn[idx][it] = true;
              } else {m_Error = it; ErrorMessage(6); }
            }
            else { m_Error = it; ErrorMessage(3); }
          }
          else token = 0;
        }
      }
    } else if (!strcmp(token, "Priority")) {
      if (idx < 0) ErrorMessage(2);
      j = 0;
      while ((token != 0) && (j < NM)) {
        if ((token = strtok( 0, seps ))) {
          if (sscanf(token,"%d",&it) > 0) {
            if (it >= 0)
              pr[Mi[j++]] = it;
            else { m_Error = it; ErrorMessage(4); }
          }
          else token = 0;
        }
      }
    }
  }
  }
  fclose(ConfigFile);
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
ErrorMessage(int Id)
{
  cerr << m_Name << " (DW_AXI_IC): " << Id << endl;
  switch (Id) {
    case 1: cerr << "IC ERROR: Configuration file format error" << endl;
            cerr << "AXI line number missing after the "
                 << "Aline/Wline/Rline/Bline keyword"
                 << endl; break;
    case 2: cerr << "IC ERROR: Configuration file format error" << endl;
            cerr << "Master keyword comes before AXI line specification"<< endl;
            cerr << "You must start with the Aline/Wline/Rline/Bline keyword"
                 << endl; break;
    case 3: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Master or slave ID (" << m_Error
                 << ") is outside the allowed range"
                 << endl; break;
    case 4: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Priority (" << m_Error << ") must not be negative"
                 << endl; break;
    case 5: cerr << "IC ERROR: Configuration file setup error" << endl;
            cerr << "A master with the specified ID (" << m_Error
                 << ") does not exist"
                 << endl; break;
    case 6: cerr << "IC ERROR: Configuration file setup error" << endl;
            cerr << "A slave with the specified ID (" << m_Error
                 << ") does not exist"
                 << endl; break;
    case 7: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Aline number (" << m_Error
                 << ") out of range (0 <= Aline < " << m_NumAlines << ")"
                 << endl; break;
    case 8: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Wline number (" << m_Error
                 << ") out of range (0 <= Wline < " << m_NumWlines << ")"
                 << endl; break;
    case 9: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Rline number (" << m_Error
                 << ") out of range (0 <= Rline < " << m_NumRlines << ")"
                 << endl; break;
    case 10: cerr << "IC ERROR: Configuration file range error" << endl;
            cerr << "Bline number (" << m_Error
                 << ") out of range (0 <= Bline < " << m_NumBlines << ")"
                 << endl; break;
    case 20: cerr << "IC ERROR: Configuration file format error" << endl;
             cerr << "AXI line number is outside the allowed range "
                  << endl; break;
    default: break;
  }
  sc_assert(0);
}

// ----------------------------------------------------------------------------
// Reset method
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
Reset()
{
  if (!m_HaveSlave) {
    int Length = AXI_MAX_IC_LINE_SIZE*AXI_MAX_PORT_SIZE * 4 / AXI_WORDSIZE_BYTES;
    for (int i = 0; i < Length; i++) m_Memory[i] = 0;
  }
  m_Aline->Reset(); m_Wline->Reset(); m_Rline->Reset(); m_Bline->Reset();
  m_Step->Reset();
}
// ----------------------------------------------------------------------------
// Reset process
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
Areset()
{
  if (!m_IsInitialized) Initialize();
  if (Aresetn) m_Reset = false;
  else {
    Reset();
    DecoderP->Reset();
    ArbiterP->Reset();
    m_Reset = true;
  }
}


// ----------------------------------------------------------------------------
// Slave IF implementation
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::PutChannel(int Idx, const Rchannel& A)
{
  m_SlaveIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const Bchannel& A)
{
  m_SlaveIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const AreadySig& A)
{
  m_SlaveIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const WreadySig& A)
{
  m_SlaveIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> int DW_AXI_IC<NegEdge, Verbose>::
GetSlaveIdx(int SlaveID, int& MasterID)
{
  return(m_SlaveIF->GetSlaveIdx(SlaveID, MasterID));
}

// ----------------------------------------------------------------------------
// Slave direct IF implementation
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_IC<NegEdge, Verbose>::
PutDirect(int ID, bool Write, axi_addr_t Addr,
           axi_data_t Data, int NumBytes)
{
  if (!m_IsInitialized) Initialize();
  int Idx = m_SIDtoMPID[DecoderP->Decode(Addr)];
  return(MasterP.PutDirect(Idx, ID, Write, Addr, Data, NumBytes));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_IC<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, const Wchannel& W,
           axi_resp_t& Status)
{
  if (!m_IsInitialized) Initialize();
  int Idx = m_SIDtoMPID[DecoderP->Decode(A.Addr)];
  return(MasterP.PutDirect(Idx, ID, BeatNumber, A, W, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_IC<NegEdge, Verbose>::
PutDirect(int ID, int BeatNumber, Achannel &A, Rchannel& R,
           axi_resp_t& Status)
{
  if (!m_IsInitialized) Initialize();
  int Idx = m_SIDtoMPID[DecoderP->Decode(A.Addr)];
  return(MasterP.PutDirect(Idx, ID, BeatNumber, A, R, Status));
}
template <bool NegEdge, bool Verbose> bool DW_AXI_IC<NegEdge, Verbose>::
GetMasterParam(int Idx, int &P) {P = m_IC_Priority; return(true);}

// ----------------------------------------------------------------------------
// Master IF implementation
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const Achannel& A)
{
  m_MasterIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const Wchannel& A)
{
  m_MasterIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const RreadySig& A)
{
  m_MasterIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> void DW_AXI_IC<NegEdge, Verbose>::
PutChannel(int Idx, const BreadySig& A)
{
  m_MasterIF->PutChannel(Idx, A);
}
template <bool NegEdge, bool Verbose> int DW_AXI_IC<NegEdge, Verbose>::
GetMasterIdx(int MasterID, int& SlaveID)
{
  return(m_MasterIF->GetMasterIdx(MasterID, SlaveID));
}

// ----------------------------------------------------------------------------
// Master direct IF implementation
// ----------------------------------------------------------------------------
template <bool NegEdge, bool Verbose> bool DW_AXI_IC<NegEdge, Verbose>::
GetSlaveParam(int Idx, int& ItrlDepth, AXI_AddressMap& M)
{
  if (!m_IC_GetSlaveParam) IC_GetSlaveParam();
  for (int i = 0; i < m_AddressMap.GetSize(); i++)
    M.PushBack(m_AddressMap[i]);
  ItrlDepth = m_IC_IntrlDepth;
  return(true);
}

// ----------------------------------------------------------------------------
//  Explicit instantiation
// ----------------------------------------------------------------------------
template class DW_AXI_IC<true, true >;
template class DW_AXI_IC<false, true >;
template class DW_AXI_IC<true, false >;
template class DW_AXI_IC<false, false >;
