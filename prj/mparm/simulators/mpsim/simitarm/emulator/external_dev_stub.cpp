
#include "external_dev.h"

//create and register your devices here
void init_devices(device_master *dm)
{
#ifdef __TEST_DEV_
	dm->register_device(0, new emulator::test_device());
#endif
}

//unregister and delete your devices here
void close_devices(device_master *dm)
{
#ifdef __TEST_DEV_
	delete dm->unregister_device(0);
#endif
}
