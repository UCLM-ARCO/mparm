
#ifndef __ARM_COPROC_H__
#define __ARM_COPROC_H__

void device_addr_write(unsigned id, unsigned data, unsigned addr);
unsigned device_addr_read(unsigned id, unsigned addr);

void device_write(unsigned id, unsigned data);
unsigned device_read(unsigned id);

#endif
