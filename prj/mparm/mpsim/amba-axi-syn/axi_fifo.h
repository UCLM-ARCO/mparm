// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 08/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : FIFO class to store the data of open requests
// ============================================================================

#ifndef _AXI_FIFO_H
#define _AXI_FIFO_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template<class Type> class AXI_FIFO
{
public:

AXI_FIFO(const char* Name, int MaxFIFO_Depth);

  ~AXI_FIFO();

  bool IsFree();                   // returns true if FIFO can store value
  bool IsEmpty();                  // returns true if FIFO is empty
  void PutValue(const Type& A);    // put value to FIFO
  bool ChangeValue(const Type& A); // change value in FIFO
  bool GetValue(Type& A);          // get and remove value from FIFO
  bool ShowValue(Type& A);         // get value from FIFO
  bool RemValue();                 // remove value from FIFO
  bool ChangeValue(const Type& A, int Idx); // change value in FIFO[Bot+Idx]
  bool GetValue(Type& A, int Idx); // get and remove FIFO{Bot+Idx] value
  bool ShowValue(Type& A, int Idx);// get FIFO{Bot+Idx] value
  bool RemValue(int Idx);          // remove FIFO{Bot+Idx] value
  void Reset();

  int m_Top;    // index of current input value
  int m_Bot;    // index of current output value
  int m_Depth;  // current size of the FIFO
  Type* m_FIFO; // FIFO pointer

private:
  inline int GetIndex(int Idx);

  // Parameter
  const char* m_Name;
  int m_MaxFIFO_Depth;
};

template<class Type> class AXI_FIFO_P
{
public:

AXI_FIFO_P(const char* Name, int MaxFIFO_Depth,
         int DataBusBytes, int DataBusWords);

  ~AXI_FIFO_P();

  bool IsFree();                   // returns true if FIFO can store value
  bool IsEmpty();                  // returns true if FIFO is empty
  void PutValue(const Type& A);    // put value to FIFO
  bool ChangeValue(const Type& A); // change value in FIFO
  bool GetValue(Type& A);          // get and remove value from FIFO
  bool ShowValue(Type& A);         // get value from FIFO
  bool RemValue();                 // remove value from FIFO
  bool ChangeValue(const Type& A, int Idx); // change value in FIFO[Bot+Idx]
  bool GetValue(Type& A, int Idx); // get and remove FIFO{Bot+Idx] value
  bool ShowValue(Type& A, int Idx);// get FIFO{Bot+Idx] value
  bool RemValue(int Idx);          // remove FIFO{Bot+Idx] value
  void Reset();

  int m_Top;    // index of current input value
  int m_Bot;    // index of current output value
  int m_Depth;  // current size of the FIFO
  Type** m_FIFO; // FIFO array pointer

private:
  inline int GetIndex(int Idx);

  // Parameter
  const char* m_Name;
  int m_MaxFIFO_Depth;
};

template<class Type> class AXI_FIFO_M
{
public:

AXI_FIFO_M(const char* Name, int MaxFIFO_Depth, bool MonitorOn);

  ~AXI_FIFO_M();

  bool IsFree();                   // returns true if FIFO can store value
  bool IsEmpty();                  // returns true if FIFO is empty
  void PutValue(const Type& A);    // put value to FIFO
  bool ChangeValue(const Type& A); // change value in FIFO
  bool GetValue(Type& A);          // get and remove value from FIFO
  bool ShowValue(Type& A);         // get value from FIFO
  bool RemValue();                 // remove value from FIFO
  bool ChangeValue(const Type& A, int Idx); // change value in FIFO[Bot+Idx]
  bool GetValue(Type& A, int Idx); // get and remove FIFO{Bot+Idx] value
  bool ShowValue(Type& A, int Idx);// get FIFO{Bot+Idx] value
  bool RemValue(int Idx);          // remove FIFO{Bot+Idx] value
  void Reset();

  int m_Top;    // index of current input value
  int m_Bot;    // index of current output value
  int m_Depth;  // current size of the FIFO
  Type* m_FIFO; // FIFO pointer

private:
  inline int GetIndex(int Idx);

  // Parameter
  const char* m_Name;
  int m_MaxFIFO_Depth;
  bool m_MonitorOn;

  // data
  int m_MaxDepth;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_FIFO_H
