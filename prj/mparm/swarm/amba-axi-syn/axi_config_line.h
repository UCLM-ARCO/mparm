// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI IC Line Configuration Class
//    - Configures the lines of an IC into a format such that they
//      can be used by AXI_IC_Arbiter
// ============================================================================

#include "axi_globals.h"

#ifndef _AXI_CONFIG_LINES
#define _AXI_CONFIG_LINES

#ifndef __EDG__
namespace axi_namespace {
#endif

class AXI_ConfigLines {

public:

  AXI_ConfigLines(const char* Name
                  , int  NumLines
                  , int  NumInitiators
                  , bool Iconn[][AXI_MAX_PORT_SIZE] // connected initiators ID
                  , int  NumTargets
                  , bool Tconn[][AXI_MAX_PORT_SIZE] // connected targets ID
                  , int* IID_PID_Map // mapping ID to PID
                  , int* TID_PID_Map // mapping ID to PID
                  )
  {
    int j, k;
    Line = new AXI_IC_Line[NumLines];

    Reset();

    for (int i = 0; i < NumLines; i++) {
      for (j = 0, k = 0; j < NumInitiators; j++) {
        if (Iconn[i][j]) {
          Line[i].IPortID[k++] = IID_PID_Map[j];
        }
      }
      Line[i].Size = k;
      for (j = 0; j < NumTargets; j++) {
        if (Tconn[i][j]) Line[i].TPortID[TID_PID_Map[j]] = true;
      }
    }
  };

  ~AXI_ConfigLines() { delete [] Line; };
  void Reset(){};

  // data
  AXI_IC_Line* Line;
};

#ifndef __EDG__
}; // end namespace axi_namespace
#endif

#endif // _AXI_CONFIG_LINES
