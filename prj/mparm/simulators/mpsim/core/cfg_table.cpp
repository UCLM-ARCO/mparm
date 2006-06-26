#include <iostream>

using namespace std;

static struct {
  const char* sym;
  const char* value;
} cfg_table[] = {
#define DO_INTERNAL_SYMBOL_TABLE
#include "config.h"
};

void
cfg_table_c_dump()
{
  cout << "#ifndef __MPSIM_CFG_TABLE\n"
       << "#define __MPSIM_CFG_TABLE\n" << endl;
  for (unsigned i=0; i<sizeof(cfg_table)/sizeof(cfg_table[0]); ++i)
    cout << "# define " << cfg_table[i].sym
	 << " " << cfg_table[i].value << endl;
  cout << "#endif" << endl;
}

void
cfg_table_dump()
{
  for (unsigned i=0; i<sizeof(cfg_table)/sizeof(cfg_table[0]); ++i)
    cout << cfg_table[i].sym << "\t" << cfg_table[i].value << endl;
}

