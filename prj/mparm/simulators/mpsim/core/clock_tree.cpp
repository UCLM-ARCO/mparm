///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         clock_tree.cpp
// author       DEIS - Universita' di Bologna
//              José L. Ayala - jayala@die.upm.es
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// info         Module implementing a clock tree with parametric dividers
//
///////////////////////////////////////////////////////////////////////////////

#include "clock_tree.h"

void clock_tree::clock_tree_process()
{
  for (unsigned short int i = 0; i < outputs; i ++)
  {
    if (Reset.read())
    {
      counter[i] = 0;
      currentvalue[i] = true;
      delay[i] = 0;
      Clock_out[i].write(true);
      divider[i] = Div[i].read();
      if (divider[i] == 0)
      {
        printf("Fatal error: Clock tree generator cannot divide by 0 on port %hu!\n", i);
        exit(1);
      }
    }
    else
    {
      if (delay[i] == 0)
      {
        if (++ counter[i] == divider[i])
        {
          currentvalue[i] = !currentvalue[i];
          Clock_out[i].write(currentvalue[i]);
          counter[i] = 0;
          if (divider[i] == 0)
          {
            printf("Fatal error: Clock tree generator cannot divide by 0 on port %hu!\n", i);
            exit(1);
          }
        }
      }
      else
        delay[i] --;
      
      if ((unsigned short int) Div[i].read() != divider[i])
      {
        delay[i] = PLL_DELAY[i] * 2; // Sensitive on both clock edges...
        counter[i] = 0;
        divider[i] = Div[i].read();
      }
    }
  }
}
