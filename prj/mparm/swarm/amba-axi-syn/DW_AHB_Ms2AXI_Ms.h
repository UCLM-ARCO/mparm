#ifndef _DW_AHB_Ms2AXI_Ms_H
#define _DW_AHB_Ms2AXI_Ms_H

#ifdef __EDG__
#include "ahb_types.h"
using namespace ahb_namespace;
#endif

#include "ahb_bus_if.h"
#include "ahb_monitor_if.h"
#include "ahb_master_port.h"
#include "ahb_dw_misc.h"

#include "axi_types.h"
#include "axi_master_if.h"
#include "axi_master_port.h"
#include "axi_slave_if_impl.h"

#include "ahb_ms2axi_ms_impl.h"

// This module converts the AMBA AHB TL master method calls
// into AXI TL master method calls. One AHB Master TL
// port can be connected to the interface of this adapter.
template<class  T_BUSIF >

class DW_AHB_Ms2AXI_Ms
: public AHB_Ms2AXI_Ms_impl<T_BUSIF >// An AHB Master Transaction-Level-Interface to AXI Master Transaction-Level-Interface Adapter
#ifdef __EDG__
, virtual public sc_interface
#endif
{
public:

	DW_AHB_Ms2AXI_Ms(const char* name_,
						 int ID,
						 int Priority,
						 int DataBusBytes,
						 double ClockFrequency)
		: AHB_Ms2AXI_Ms_impl<T_BUSIF>(name_,
										  ID,
										  Priority,
										  DataBusBytes,
										  ClockFrequency)
		{}
}; // end module DW_AHB_Ms2AXI_Ms
#endif
