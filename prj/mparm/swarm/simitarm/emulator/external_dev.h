
#ifndef __EXTERNAL_DEV_H_
#define __EXTERNAL_DEV_H_

#include "emu_device.hpp"

using emulator::device_master;
using emulator::emu_device;

/* stub functions. */
void init_devices(device_master *dm);
void close_devices(device_master *dm);

/* This is an example device. To define your own device,
   1. derive you device class from emu_device.
   2. define your own write/read functions.
   3. create device instances and register them in 
      external_dev_stub.cpp::init_devices.
   4. unregister devices and destroy then instances
      in external_dev_stub.cpp::close_devices.

   Note: The device can block the execution of the processor core
      by returning a false in its write/read methods. For devices
      featuring non-blocking read/write, a true value should always
      be returned.

   When programming your code to simulate, you can use the functions
   defined in device/arm_coproc.h to access coprocessors.
   The function implementation is in device/arm_coproc.s. So remember to
   assemble and link the file with your application.
   A sample file device/arm_test.c is given.
*/

#ifdef __TEST_DEV_

namespace emulator {

  class test_device : public emu_device {


	public:
		test_device() : emu_device() {}

		~test_device() {}

		bool write(dev_data_t data, dev_addr_t addr) {
			buf = data;
			return true;
		}

		bool read(dev_data_t& data, dev_addr_t addr) {
			data = buf;
			return true;
		}
	private: 
		int buf;
  };

}
#endif

#endif

