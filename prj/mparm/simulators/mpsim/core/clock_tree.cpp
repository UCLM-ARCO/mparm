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
															
  unsigned int PLL_TABLE_DELAY[MAX_DIVIDER][MAX_DIVIDER] = {{0, 41, 61, 80, 82, 101},
							    {60, 0, 21, 40, 41, 60},
							    {61, 21, 0, 16, 24, 40},
							    {81, 40, 10, 0, 20, 21},
							    {94, 54, 32, 24, 0, 24},
							    {108, 64, 40, 30, 24, 0}};
	
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
				//delay[i] = PLL_DELAY[i] * 2; // Sensitive on both clock edges...
				if (Div[i].read() > MAX_DIVIDER || divider[i] > MAX_DIVIDER)
					delay[i] = 0;
				else
					delay[i] = PLL_TABLE_DELAY[divider[i]-1][Div[i].read()-1] * 2; // Sensitive on both clock edges...
				counter[i] = 0;
        divider[i] = Div[i].read();
      }
    }
				
  }
}
