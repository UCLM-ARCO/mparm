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
#ifndef __PARMS_H__
#define __PARMS_H__

/********************************************
* High level microarchitecture parameters  *
********************************************/

const int memoryWriteLatency = 0; /* Memory Write Latency 18*/
const int memoryReadLatency = 0;  /* Memory Read Latency   32*/
const int tlbLoadLatency = 0;	  /* Memory Read Latency   49*/

//const int nITLBBlocks   = 32;
//const int nITLBAssoc    = 32;
const int IPageSize     = 4096;

//const int nDTLBBlocks   = 32;
//const int nDTLBAssoc    = 32;
const int DPageSize     = 4096;

//const int nICacheBlocks = 512;
//const int nICacheAssoc  = 32;
const int ICacheLineSize= 32;

//const int nDCacheBlocks = 256;
//const int nDCacheAssoc  = 32;
const int DCacheLineSize= 32;

//const int nWriteBufferEntries = 8;
const int writeBufferEntrySize=DCacheLineSize/2;


#endif
