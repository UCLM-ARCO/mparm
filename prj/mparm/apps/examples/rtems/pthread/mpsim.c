#include "mpsim.h"

void scale_this_core_frequency(unsigned short divider) {
  MPSIM_FREQ[get_proc_id() - 1] = divider;
}

void scale_device_frequency(unsigned short int divider, int id) {
  MPSIM_FREQ[id] = divider;
}

unsigned short get_this_core_frequency() {
  return MPSIM_FREQ[get_proc_id() - 1];
}

unsigned short int get_device_frequency(int id) {
  return MPSIM_FREQ[id];
}

void send_interrupt(int id) {
  ASSERT(id != 0);
  MPSIM_INT[id] = 1;
}

void WAIT_FOR_INITIALIZATION() {
  while (*MPSIM_INIT_FLAG != 0xdadbebad) ;
}

void INITIALIZATION_DONE() {
  *(MPSIM_INIT_FLAG) = 0xdadbebad;
}

void BARINIT(int id) {
  MPSIM_BARRIER(id)[0] = 0;
  MPSIM_BARRIER(id)[1] = 0;
}

void BARRIER(int id, int n_proc) {
  while (MPSIM_BARRIER(id)[1] != 0);
  WAIT(-2);
  ++MPSIM_BARRIER(id)[0];
  SIGNAL(-2);
  while (MPSIM_BARRIER(id)[0] != n_proc);
  WAIT(-2);
  ++MPSIM_BARRIER(id)[1];
  if (MPSIM_BARRIER(id)[1] == n_proc) {
    MPSIM_BARRIER(id)[0] = 0;
    MPSIM_BARRIER(id)[1] = 0;
  }
  SIGNAL(-2);
}
