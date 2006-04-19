// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI Channel Class
//    - Basic data structure of the Master/Slave IF implementation
//    - Implements the double buffering + toggeling scheme
// ============================================================================

#ifndef _AXI_CHANNEL_H
#define _AXI_CHANNEL_H

#include "axi_globals.h"

#ifndef __EDG__
namespace axi_namespace {
#endif

template<class ChannelType> class AXI_Channel
{
public:

  // constructor
  AXI_Channel(int NumChannels);
  AXI_Channel(int NumChannels, int DataBusBytes, int DataBusWords);

  // destructor
  ~AXI_Channel();

  void PutChannel(int Idx, const ChannelType&);  // write access methods
  const ChannelType& GetChannel(int Idx);        // read access methods
  bool GetValid(int Idx);                        // get valid signal
  void Toggle();                          // switch current Idx to next Idx
  void Reset();                           // initialization

private:
  void Instantiate(int NumChannels);

  // Parameter
  int m_NumChannels; // Number of channels

  bool *m_Change;
  int *m_CurrIdx;
  int *m_NextIdx;

  ChannelType **m_Channel[2];
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CHANNEL_H
