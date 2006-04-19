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

#include "emu_device.hpp"
#include <cassert>

using std::map;
using namespace emulator;

device_master::device_master() : last_id(0), last_dev(NULL)
{
}

device_master::~device_master()
{
}

bool device_master::register_device(dev_id_t id, emu_device *dev)
{
	if (devs.find(id)!=devs.end()) return false;
	devs[id] = dev;
	return true;
}

emu_device *device_master::unregister_device(dev_id_t id)
{
	if (devs.find(id)==devs.end()) return NULL;
	if (last_id==id) last_dev=NULL;
	emu_device *ret = devs[id];
	devs.erase(id);
	return ret;
}

bool device_master::send(dev_id_t id, dev_data_t val, dev_addr_t addr)
{
	/* check cached result*/
	if (last_dev && id==last_id) return last_dev->write(val, addr);

	map<dev_id_t, emu_device *>::iterator dev_it;

	dev_it = devs.find(id);
	assert(dev_it!=devs.end());

	last_id = id;
	last_dev = (*dev_it).second;

	return last_dev->write(val, addr);
}

bool device_master::receive(dev_id_t id, dev_data_t &val, dev_addr_t addr)
{
	/* check cached device*/
	if (last_dev && id==last_id) return last_dev->read(val, addr);

	map<dev_id_t, emu_device *>::iterator dev_it;

	dev_it = devs.find(id);
	assert(dev_it!=devs.end());

	last_id = id;
	last_dev = (*dev_it).second;

	return last_dev->read(val, addr);
}
