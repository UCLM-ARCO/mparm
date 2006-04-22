// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Slave Port Classes (specialized port)
//    - Single port (faster) and multi port version
//    - Gets the Port ID from the IF 
//    - Includes the Port ID into the IF calls
// ============================================================================

#ifndef AXI_SLAVE_PORT_H
#define AXI_SLAVE_PORT_H

#include "axi_slave_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_SlavePort
	: public sc_port<AXI_SlaveIF, 1>
{
public:
  AXI_SlavePort(int SlaveID) 
  : m_Idx (0)
  , m_SlaveID (SlaveID)
  {}

  int GetIF_Idx() { return (m_Idx); }
  int GetMasterID() { return (m_MasterID); }

  // Slave IF methods
  void PutChannel(const Rchannel& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(const Bchannel& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(const AreadySig& A){ m_P->PutChannel(m_Idx, A); }
  void PutChannel(const WreadySig& A){ m_P->PutChannel(m_Idx, A); }
  // Methods to have the same signature as the multi port version
  // The index is ignored
  void PutChannel(int i, const Rchannel& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const Bchannel& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const AreadySig& A){ m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const WreadySig& A){ m_P->PutChannel(m_Idx, A); }
  bool GetMasterParam(int &P) { return(m_P->GetMasterParam(m_Idx, P)); }


private:
  int m_Idx;
  int m_SlaveID;
  int m_MasterID;
  AXI_SlaveIF* m_P;
  void end_of_elaboration() {
    m_P = dynamic_cast<AXI_SlaveIF*>(get_interface());
    assert(m_P != 0);
    m_Idx = m_P->GetSlaveIdx(m_SlaveID, m_MasterID);
  }
  // disable the [] and -> operator (use always operator '.')
  AXI_SlaveIF* operator -> ();
  const AXI_SlaveIF* operator -> () const;
  AXI_SlaveIF* operator [] ( int index_ );
  const AXI_SlaveIF* operator [] ( int index_ ) const;
};

template <int NumPorts> class AXI_SlaveMport
	: public sc_port<AXI_SlaveIF, NumPorts>
{
public:
  AXI_SlaveMport(int SlaveID) : m_SlaveID (SlaveID) {}

  int GetIF_Idx(int PID) { return (m_Idx[PID]); }
  int GetMasterID(int PID) { return (m_MasterID[PID]); }

  // Slave IF methods
  void PutChannel(int PID, const Rchannel& A) {
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const Bchannel& A) {
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const AreadySig& A){
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const WreadySig& A){
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  bool GetMasterParam(int PID, int &P) {
    return(m_P[PID]->GetMasterParam(m_Idx[PID], P));
  }


private:
  int m_Idx[NumPorts];
  int m_SlaveID;
  int m_MasterID[NumPorts];
  AXI_SlaveIF* m_P[NumPorts];
  void end_of_elaboration() {
    for (int i = 0; i < this->size(); i++) {
      m_P[i] = sc_port<AXI_SlaveIF, NumPorts>::operator[](i);
      assert(m_P[i] != 0);
      m_Idx[i] = m_P[i]->GetSlaveIdx(m_SlaveID, m_MasterID[i]);
    }
  }

  // disable the [], -> operator
  AXI_SlaveIF* operator -> ();
  const AXI_SlaveIF* operator -> () const;
  AXI_SlaveIF* operator [] ( int index_ );
  const AXI_SlaveIF* operator [] ( int index_ ) const;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // AXI_SLAVE_PORT_H
