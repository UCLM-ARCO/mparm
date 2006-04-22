///////////////////////////////////////////////////////////////////////////////
// Copyright 2000 Michael Dales
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// file   swarm_types.h
// author Michael Dales (michael@dcs.gla.ac.uk)
// header n/a
// info   Defines some general types used in swarm. Note - I've provided
//        versions for ix86, sun4, and alpha.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SWARM_TYPES_H__
#define __SWARM_TYPES_H__

#include "globals.h"

#ifdef sun4
#define __BIG_ENDIAN__
#else /* ix86 / alpha / arm32 */
#define __LITTLE_ENDIAN__
#endif

#ifdef WIN32
#include <windows.h>
#endif

#endif /* __SWARM_TYPES_H__ */
