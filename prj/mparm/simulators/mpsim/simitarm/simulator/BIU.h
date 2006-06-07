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

#ifndef __BUI_H__
#define __BUI_H__

#include <cstdio>
#include <string>
#include "misc.h"

namespace simulator {

typedef class biu {

	public:
		biu(const std::string& name) : name(name) {
			naccess=nbusy=nfree=0;
			busy=false;
			busyTill = 0;
		}

		void reset() {
			naccess=nbusy=nfree=0;
			busy=false;
			busyTill = 0;
		}

		uint32_t access(int latency) {
			naccess++;
			busy = true;
			busyTill += latency;
			return busyTill;
		}

		void updateOnClockTick() {
			if (busy) {
				busy = busyTill>0;
				if (busyTill>0) --busyTill;
				nbusy++;
			}
			else nfree++;
		}

		void PrintStats(FILE *fp) {
			fprintf(fp, "Total %s accesses: ", name.c_str());
			dump_int64(naccess, fp);
			fprintf(fp, "\n%s activity:	   %.3f%%\n",
				name.c_str(), 100.0*nbusy/(nbusy+nfree));
		}

	private:
		const std::string name;

		uint64_t naccess;
		uint64_t nbusy;	/*busy cycles*/
		uint64_t nfree;	/*free cycles*/

		bool busy;
		uint64_t busyTill;
} BIU;

}

#endif
