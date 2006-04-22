
#include <stdio.h>
#include "arm_coproc.h"

int main()
{
	int i,j=0;
	for (i=0; i<1000000; i++) {
		device_write(0, i);
		//fprintf(stderr, "%d", device_read(0));
		j+=device_read(0);
	}
	
	fprintf(stderr, "%d", j);

}
