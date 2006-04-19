// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI AID FIFO Classes
//   - Contains the following classes:
//     . AXI_ID_FIFO (RID, BID reordering)
//     . AXI_WID_FIFO (WID reordering)
//     . AXI_Lock (Slave locking)
//   - Request/response reordering inside the IC
//   - Slave locking
// ============================================================================

#ifndef _AXI_AID_FIFO_H
#define _AXI_AID_FIFO_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

// --------------------------------------------------------------------------
// RID/BID ordering class, base class for WID ordering
// --------------------------------------------------------------------------
class AXI_ID_FIFO {

public:
  AXI_ID_FIFO(const char* Name
              , int  MaxFIFO_Depth
              , bool MonitorOn
              );
  ~AXI_ID_FIFO();

  void Reset();
  void PutValue(int ID, int MPID);
  int GetMPID(int ID);
  bool Accept(int ID, int MPID);
  void RemValue(int ID);

  // data
  // number of submitted addresses per ID
  int ID_Depth[AXI_MAX_EID_SIZE];

private:
  // parameter
  const char* m_Name;
  int m_MaxFIFO_Depth;
  bool m_MonitorOn;

  // data
  int* m_ID_FIFO[AXI_MAX_EID_SIZE];
  int m_ID_Top[AXI_MAX_EID_SIZE];
  int m_ID_Bot[AXI_MAX_EID_SIZE];
  int m_MaxDepth[AXI_MAX_EID_SIZE];
};

// --------------------------------------------------------------------------
// WID ordering class
// --------------------------------------------------------------------------
class AXI_WID_FIFO {

public:
  AXI_WID_FIFO(const char* Name
              , int  MaxFIFO_Depth
              , int* ItrlDepth
              , bool MonitorOn
              );

  ~AXI_WID_FIFO();

  void Reset();
  void PutValue(int SPID, int WID, int MPID);
  int GetMPID(int WID);
  bool Accept(int SPID, int WID, int MPID);
  void RemValueF(int WID);
  void RemValueL(int SPID, int WID);

  // data
  //number of submitted addresses per WID
  int* WID_Depth;

private:
  // parameter
  const char* m_Name;
  int m_MaxFIFO_Depth;
  int* m_ItrlDepth;

  // data
  AXI_ID_FIFO* m_WID_FIFO;
  int* m_MPID_FIFO[AXI_MAX_PORT_SIZE];
  int* m_SPID_FIFO[AXI_MAX_PORT_SIZE];
  // number of active data transfers per slave
  int m_MPID_Depth[AXI_MAX_PORT_SIZE];
  // flag if first transfer in a transaction
  int m_MPID_First[AXI_MAX_EID_SIZE][AXI_MAX_PORT_SIZE];
  int m_WID_Top[AXI_MAX_EID_SIZE];
  int m_WID_Bot[AXI_MAX_EID_SIZE];
  int m_MPID_Top[AXI_MAX_PORT_SIZE];
  int m_MPID_Bot[AXI_MAX_PORT_SIZE];
  int m_SPID_Top[AXI_MAX_PORT_SIZE];
  int m_SPID_Bot[AXI_MAX_PORT_SIZE];
};

// --------------------------------------------------------------------------
// Locking class
// --------------------------------------------------------------------------
class AXI_Lock
{

public: 
  AXI_Lock();
  ~AXI_Lock();

  bool AcceptA(int SPID, int MPID);
  bool AcceptD(int SPID, int MPID);
  void Update(axi_alock_t Alock, int SPID, int MPID);
  void UpdateL(int MPID);
  void RemLock(int MPID);
  void Reset();

private:
  axi_lockstate_t m_Lock[AXI_MAX_PORT_SIZE];
  int m_LockSPID[AXI_MAX_PORT_SIZE];
  int m_NumAddr[AXI_MAX_PORT_SIZE];
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_AID_FIFO_H
