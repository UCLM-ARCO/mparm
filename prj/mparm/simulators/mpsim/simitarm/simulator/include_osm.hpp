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

#ifndef __INCLUDE_OSM_H__
#define __INCLUDE_OSM_H__

#if _MAX_OPER_ID<256
#define _OPER_ID_T uint8_t
#elif _MAX_OPER_ID<65536
#define _OPER_ID_T uint16_t
#else
#define _OPER_ID_T uint32_t
#endif

#define _MACH_ID_T uint32_t

namespace simulator {
	class arm_simulator;
}

#define _SIMULATOR simulator::arm_simulator
#define _OPTIMIZE_OSM

/* include libosm stuff */
#include "libosm.hpp"

#endif
