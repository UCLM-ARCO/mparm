// ============================================================================
//  Description : Miscellaneous types for the TLM AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_TYPES_H
#define AHB_TYPES_H

#include <systemc.h>

namespace ahb_namespace {

	// ------------------------------------------------------------------------
	// CONST : NUM_MASTERS, NUM_SLAVES
	// 
	// the maximum number of AHB masters and slaves
	// ------------------------------------------------------------------------

	const int NUM_MASTERS = 16;
	const int NUM_SLAVES  = 16;


	// The dummy master has alway id 0
	const int DUMMY_MASTER_ID  = 0;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_hprot
	//
	//  Type for the protection modus of the bus model.
	// ------------------------------------------------------------------------

	typedef unsigned int ahb_hprot;

	const ahb_hprot DATA       =  0x1;
	const ahb_hprot PRIVILEGED =  0x2;
	const ahb_hprot BUFFERABLE =  0x4;
	const ahb_hprot CACHEABLE  =  0x8;

	// Additional protection features for ARM11
	const ahb_hprot ALLOCATE   = 0x10;
	const ahb_hprot EXCLUSIVE  = 0x20;

	// ------------------------------------------------------------------------
	// TYPEDEF : ahb_addr_t
	//
	// Type declaration for the address data type
	// ------------------------------------------------------------------------

	typedef unsigned int ahb_addr_t;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_data_t
	//
	//  Type declaration for the data data type
	// ------------------------------------------------------------------------

	typedef unsigned int* ahb_data_t;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_hresp
	//
	//  Enumeration type for the status response.
	// ------------------------------------------------------------------------

	typedef enum {
		OKAY  = 0,
		ERROR = 1,
		RETRY = 2,
		SPLIT = 3,
		XFAIL = 4
	} ahb_hresp;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_hwrite
	//
	//  Enumeration type for the read/write opterion specification.
	// ------------------------------------------------------------------------

	typedef enum {
		RD = 0,
		WR = 1
	} ahb_hwrite;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_htrans
	//
	//  Enumeration type for the transfer modus.
	// ------------------------------------------------------------------------

	typedef enum {
		IDLE   = 0,
		BUSY   = 1,
		NONSEQ = 2,
		SEQ    = 3
	} ahb_htrans;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_hburst
	//
	//  Enumeration type for the burst modes.
	// ------------------------------------------------------------------------

	typedef enum {
		SINGLE = 0,
		INCR   = 1,
		WRAP4  = 2,
		INCR4  = 3,
		WRAP8  = 4,
		INCR8  = 5,
		WRAP16 = 6,
		INCR16 = 7
	} ahb_hburst;

	// ------------------------------------------------------------------------
	//  TYPEDEF : ahb_hsize
	//
	//  Enumeration type for the transfer size of the bus model.
	// ------------------------------------------------------------------------

	typedef enum {
		SIZE_8    = 0,
		SIZE_16   = 1,
		SIZE_32   = 2,
		SIZE_64   = 3,
		SIZE_128  = 4,
		SIZE_256  = 5,
		SIZE_512  = 6,
		SIZE_1024 = 7
	} ahb_hsize;

	// ------------------------------------------------------------------------
	//  STRUCT : ahb_request_arb
	//
	//  Structure to share data between bus channel and arbiter.
	// ------------------------------------------------------------------------

	struct ahb_request_arb
	{
		// arbitration control
		bool            request;
		bool            lock;

		bool            grant;

		bool            is_split;

		ahb_request_arb()
			: request            (false)
			, lock               (false)
			, grant              (false)
			, is_split           (false)
		{}
	};

	// ------------------------------------------------------------------------
	//  STRUCT : ahb_address_region
	//
	//  Struct containing address regions of slaves.
	// ------------------------------------------------------------------------

	struct ahb_address_region
	{
	public:
		ahb_addr_t normal_start_addr;
		ahb_addr_t normal_end_addr;
		ahb_addr_t boot_start_addr;
		ahb_addr_t boot_end_addr;

		// default constructor
		ahb_address_region()
			: normal_start_addr  (0)
			, normal_end_addr    (0)
			, boot_start_addr    (0)
			, boot_end_addr      (0)
		{}

		// constructor normal=boot mode
		ahb_address_region(ahb_addr_t start_addr,
		                   ahb_addr_t end_addr)
			: normal_start_addr  (start_addr)
			, normal_end_addr    (end_addr)
			, boot_start_addr    (start_addr)
			, boot_end_addr      (end_addr)
		{}

		// constructor normal and boot mode
		ahb_address_region(ahb_addr_t start_addr,
		                   ahb_addr_t end_addr,
		                   ahb_addr_t boot_start_addr,
		                   ahb_addr_t boot_end_addr)
			: normal_start_addr  (start_addr)
			, normal_end_addr    (end_addr)
			, boot_start_addr    (boot_start_addr)
			, boot_end_addr      (boot_end_addr)
		{}
	};

	// ------------------------------------------------------------------------
	//  Class : ahb_address_map
	//
	//  Simple vector class for address regions.
	// ------------------------------------------------------------------------

	class ahb_address_map
	{
	public:
		ahb_address_map(int alloc = 16)
			: m_alloc (0)
			, m_size  (0)
			, m_data  (0)
		{
			m_resize(alloc);
		}

		~ahb_address_map()
		{
			for(int i=0; i<m_size; i++)
				delete m_data[i];

			if(m_data)
				delete[] m_data;
		}

		void
		push_back(const ahb_address_region& addr_region)
		{
			if(m_size == m_alloc)
				m_resize(m_alloc+16);
			m_data[m_size] = new ahb_address_region(addr_region);
			m_size++;
		}

		unsigned int
		size() const
		{
			return m_size;
		}

		const ahb_address_region&
		operator[](int i) const
		{
			sc_assert((i>=0) && (i<m_size));
			return *m_data[i];
		}

	private:

		void
		m_resize(int new_size)
		{
			ahb_address_region** new_data;

			if( new_size <= m_alloc ) {
				return;
			}
			m_alloc = new_size;
			new_data = new ahb_address_region*[new_size];
			for(int i=0; i<m_size; i++)
				new_data[i] = m_data[i];
			if(m_data)
				delete[] m_data;
			m_data = new_data;
		}

		int                  m_alloc;
		int                  m_size;
		ahb_address_region** m_data;
	};

}; // end namespace ahb_namespace

#endif
