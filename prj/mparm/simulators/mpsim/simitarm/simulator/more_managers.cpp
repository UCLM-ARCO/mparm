/*************************************************************************
    Copyright (C) 2002,2003,2004 Wei Qin
    See file COPYING for more information.

    This program is free software; you can redistribute it and/or modify    
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*************************************************************************/

#include "more_managers.hpp"
#include "armsim.hpp"
#include "emu_device.hpp"

using namespace simulator;

void fetch_manager::update_on_clock()
{
	/* a new OSM comes in */
	if (trigger) {
		current_pc = mRF->get_pc().val();
		mRF->set_pc(current_pc+4);
		trigger = false;
	}

	if (fetch_status==1) {
		pte = itlb->read(current_pc);
		fetch_status = 0;
	}
}

void mem_ctrl_port::update_on_clock() 
{
	if (trigger) {

		assert(status==0);
		status = 1;
		virtual_addr = mMemAddr->get_value().val();
		mergeable = count > 0;
		trigger = false;
	}
	else if (count>0) {

		if (status==0 && --count>0) {
			status = 1;
			virtual_addr += 4;
		}
	}

	if (status==1) {
		pte = dtlb->read(virtual_addr);
		uint32_t physical_addr = dtlb->get_physical_addr(pte->physical_tag,virtual_addr);
		if (isRead)	
		{
			mMemRead->set_value(dcache->read(physical_addr,size,pte->cacheable,pte->bufferable));
		}
		else
		{	
			dcache->write(physical_addr,size,mergeable,(uint32_t)(mMemWrite->get_value().val()),pte->cacheable,pte->bufferable);
		}
		status = 0;
	}
}

void coproc_manager::do_read()
{
	// r0 contains the device id
	dev_id_t id = mRF->get_value(0).val();

	// r1 contains the address
	dev_addr_t addr = mRF->get_value(1).val();

	dev_data_t data;

	if (has_addr) {
		if (dev_emul->receive(id, data, addr)) {
			state = 0;
			mRF->set_value(0, data);
		}
	}
	else if (dev_emul->receive(id, data)) {
		state = 0;
		mRF->set_value(0, data);
	}
}

void coproc_manager::do_write()
{
	// r0 contains the device id
	dev_id_t id = mRF->get_value(0).val();

	// r2 contains the address
	dev_addr_t addr = mRF->get_value(2).val();

	// r1 contains the value to write
	dev_data_t data = mRF->get_value(1).val();

	if (has_addr) {
		if (dev_emul->send(id, data, addr))
			state = 0;
	}
	else if (dev_emul->send(id, data)) {
		state = 0;
	}
}
