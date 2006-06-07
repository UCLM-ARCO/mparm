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

#ifndef __DEVICE_EMULATE_HPP_
#define __DEVICE_EMULATE_HPP_

#include <map>
#include "misc.h"

typedef uint32_t dev_id_t;
typedef uint32_t dev_addr_t;
typedef uint32_t dev_data_t;

namespace emulator {

  /** Base class for device.
   *  This class corresponds to a device driver. */
  class emu_device {

	public:
		/** Constructor. */
		emu_device() {}

		/** Virtual destructor of base class. */
		virtual ~emu_device() {}

		/** Write a value to the device.
		 *  @param addr The address to write to.
		 *  @param data The data to write.
		 *  @return False to block.
		 */
		virtual bool write(dev_data_t data, dev_addr_t addr) = 0;

		/** Read a value from a device.
		 *  @param addr The address to write to.
		 *  @param data The data to write.
		 *  @return False to block.
		 */
		virtual bool read(dev_data_t& data, dev_addr_t addr) = 0;
  };


  /** Manager of all devices.
   *  This corresponds to the device table in OS.
   */
  class device_master {


	public:
		/** Constructor. */
		device_master();

		/** Destructor. */
		~device_master();

		/** Register a device.
		 *  @param id  The id of the device.
		 *  @param dev A pointer to the device.
		 *  @return True if register successful, false if id already in use.
		 */
		bool register_device(dev_id_t id, emu_device *dev);

		/** Unregister a device.
		 *  @param id The id of the device.
		 *  @return Pointer to a device structure if successfully unregistered,
		 *        false if no such device.
		 */
		emu_device *unregister_device(dev_id_t id);

		/** Send a value to a device.
		 *  @param id   The id of the device.
		 *  @param val  The value.
		 *  @param addr The address within the device.
		 *  @return False to block.
		 */
		bool send(dev_id_t id, dev_data_t val, dev_addr_t addr=0);

		/** Receive a value from a device.
		 *  @param id   The id of the device.
		 *  @param val  Reference to the value to be read.
		 *  @param addr The address within the device.
		 *  @return False to block.
		 */
		bool receive(dev_id_t id, dev_data_t &val, dev_addr_t addr=0);

	private:

		/* The last device accessed */
		dev_id_t last_id;
		emu_device *last_dev;

		/* registered devices. */
		std::map<dev_id_t, emu_device *> devs;

  };

}

#endif
