#include "TestCase.hh"
#include "MPS/Config.h"

volatile int* const TestCase::_stamp = MPS::ShMem::memory_base;
