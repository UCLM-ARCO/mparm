#ifndef _AXI_MEMORY_MONITOR_IMPL_H
#define _AXI_MEMORY_MONITOR_IMPL_H

#include "axi_globals.h"
class ccss_monitor_table;

class AXI_MemoryMonitorImpl
{
public:

	AXI_MemoryMonitorImpl(const char* Name,
						  axi_namespace::axi_addr_t StartAddress,
						  axi_namespace::axi_data_t Memory,
						  int MemoryLength,
						  int NumMonitorCols,
						  int DataBusBytes);

	~AXI_MemoryMonitorImpl();

	void Reset();
	void Update(int Offset, int NumWords);

private:

	const char                 *m_Name;
	axi_namespace::axi_addr_t  m_StartAddress;
	axi_namespace::axi_data_t  m_Memory;
	int                        m_MemoryLength;
	int                        m_NumMonitorCols;
	int                        m_DataBusBytes;

	ccss_monitor_table         *m_Monitor;
};
#endif //_AXI_MEMORY_MONITOR_IMPL_H
