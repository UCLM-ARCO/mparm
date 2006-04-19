// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Master Port Classes (specialized port)
//    - Single port (faster) and multi port version
//    - Gets the Port ID from the IF 
//    - Includes the Port ID into the IF calls
// ============================================================================

#ifndef AXI_MASTER_PORT_H
#define AXI_MASTER_PORT_H

#include "axi_master_if.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_MasterPort : public sc_port<AXI_MasterIF, 1>
{
public:
  AXI_MasterPort(int MasterID) 
  : m_Idx (0)
  , m_MasterID (MasterID)
  {}

  int GetIF_Idx() { return (m_Idx); }
  int GetSlaveID() { return (m_SlaveID); }

  // Master IF methods
  void PutChannel(const Achannel& A)  { m_P->PutChannel(m_Idx, A); }
  void PutChannel(const Wchannel& A)  { m_P->PutChannel(m_Idx, A); }
  void PutChannel(const RreadySig& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(const BreadySig& A) { m_P->PutChannel(m_Idx, A); }
  // Methods to have the same signature as the multi port version
  // The index is ignored
  void PutChannel(int i, const Achannel& A)  { m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const Wchannel& A)  { m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const RreadySig& A) { m_P->PutChannel(m_Idx, A); }
  void PutChannel(int i, const BreadySig& A) { m_P->PutChannel(m_Idx, A); }
  bool GetSlaveParam(int& IntrlDepth, AXI_AddressMap& A){
    return(m_P->GetSlaveParam(m_Idx, IntrlDepth, A));
  }

  bool PutDirect(int ID, bool Write, axi_addr_t Addr,
                 axi_data_t Data, int NumBytes)
  {
    return(m_P->PutDirect(ID, Write, Addr, Data, NumBytes));
  }
  bool PutDirect(int ID, int BeatNumber,
                 Achannel &A, const Wchannel& W, axi_resp_t& Status)
  {
    return(m_P->PutDirect(ID, BeatNumber, A, W, Status));
  }
  bool PutDirect(int ID, int BeatNumber,
                 Achannel &A, Rchannel& R, axi_resp_t& Status)
  {
    return(m_P->PutDirect(ID, BeatNumber, A, R, Status));
  }

private:
  int m_Idx;
  int m_SlaveID;
  int m_MasterID;
  AXI_MasterIF* m_P;
  void end_of_elaboration() {
    m_P = dynamic_cast<AXI_MasterIF*>(get_interface());
    assert(m_P != 0);
    m_Idx = m_P->GetMasterIdx(m_MasterID, m_SlaveID);
  }
  // disable the [] and -> operator (use always operator '.')
  AXI_MasterIF* operator -> ();
  const AXI_MasterIF* operator -> () const;
  AXI_MasterIF* operator [] ( int index_ );
  const AXI_MasterIF* operator [] ( int index_ ) const;
};

template <int NumPorts> class AXI_MasterMport
	: public sc_port<AXI_MasterIF, NumPorts>
{
public:
  AXI_MasterMport(int MasterID) : m_MasterID (MasterID) {}

  int GetIF_Idx(int PID) { return (m_Idx[PID]); }
  int GetSlaveID(int PID) { return (m_SlaveID[PID]); }

  // Master IF methods
  void PutChannel(int PID, const Achannel& A) {
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const Wchannel& A) {
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const RreadySig& A){
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  void PutChannel(int PID, const BreadySig& A){
    m_P[PID]->PutChannel(m_Idx[PID], A);
  }
  bool GetSlaveParam(int PID, int& IntrlDepth, AXI_AddressMap& A){
    return(m_P[PID]->GetSlaveParam(m_Idx[PID], IntrlDepth, A));
  }

  bool PutDirect(int PID, int ID, bool Write, axi_addr_t Addr,
                 axi_data_t Data, int NumBytes)
  {
    return(m_P[PID]->PutDirect(ID, Write, Addr, Data, NumBytes));
  }
  bool PutDirect(int PID, int ID, int BeatNumber,
                 Achannel &A, const Wchannel& W, axi_resp_t& Status)
  {
    return(m_P[PID]->PutDirect(ID, BeatNumber, A, W, Status));
  }
  bool PutDirect(int PID, int ID, int BeatNumber,
                 Achannel &A, Rchannel& R, axi_resp_t& Status)
  {
    return(m_P[PID]->PutDirect(ID, BeatNumber, A, R, Status));
  }

private:
  int m_Idx[NumPorts];
  int m_MasterID;
  int m_SlaveID[NumPorts];
  AXI_MasterIF* m_P[NumPorts];
  void end_of_elaboration() {
    for (int i = 0; i < this->size(); i++) {
      m_P[i] = sc_port<AXI_MasterIF, NumPorts>::operator[](i);
      assert(m_P[i] != 0);
      m_Idx[i] = m_P[i]->GetMasterIdx(m_MasterID, m_SlaveID[i]);
    }
  }

  // disable the [], -> operator
  AXI_MasterIF* operator -> ();
  const AXI_MasterIF* operator -> () const;
  AXI_MasterIF* operator [] ( int index_ );
  const AXI_MasterIF* operator [] ( int index_ ) const;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // AXI_MASTER_PORT_H
