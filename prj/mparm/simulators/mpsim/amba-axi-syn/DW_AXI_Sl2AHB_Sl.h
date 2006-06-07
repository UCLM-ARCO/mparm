#ifndef _AXI_TLSL2AHB_TLSL_H
#define _AXI_TLSL2AHB_TLSL_H

#ifdef __EDG__
#include "ahb_types.h"
using namespace ahb_namespace;
#endif

#include "axi_sl2ahb_sl_impl.h"


template<class T_SIF = ahb_namespace::ahb_slave_if  >
class DW_AXI_Sl2AHB_Sl
	: public AXI_Sl2AHB_Sl_impl<T_SIF >
#ifdef __EDG__
	, virtual public sc_interface
#endif
{

public:

	DW_AXI_Sl2AHB_Sl(const char* Name,
						 int ID,
						 axi_namespace::axi_addr_t StartAddress,
						 axi_namespace::axi_addr_t EndAddress,
						 bool MonitorOn,
						 int DataBusBytes,
						 double ClockFrequency)
		: AXI_Sl2AHB_Sl_impl<T_SIF>(Name,
										ID,
										StartAddress,
										EndAddress,
										MonitorOn,
										DataBusBytes,
										ClockFrequency)
		{}

}; // end module DW_AXI_Sl2AHB_Sl
#endif
