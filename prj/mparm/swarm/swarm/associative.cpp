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
// name   associative.cpp
// author Michael Dales (michael@dcs.gla.ac.uk)
// header direct.h
// info   Implements a fully associative cache. Note that, rather confusingly,
//        the size is specified in bytes, but the address given for reads
//        and writes is in words.
//
///////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

#include <stdlib.h>
#include "swarm.h"
#include "associative.h"
#include "stats.h"

// martin letis - this is redundant - using CACHE_LINE
// #define LINE_SIZE_W 4  /* (words) */

#define LINE_SIZE_B 16 /* (bytes) */


///////////////////////////////////////////////////////////////////////////////
// Constructor
//
CAssociativeCache::CAssociativeCache(uint32_t nSize)
{
  m_nSize = nSize;
  m_nLines = nSize / LINE_SIZE_B;

  m_pDataRAM = new uint32_t[nSize / sizeof(uint32_t)];
  m_pTagCAM = new uint32_t[m_nLines];
  m_pDirtyCAM = new uint32_t[m_nLines];

  //printf ("core %hu - Instantiating associative cache of %d bytes - %d words of dataRAM - %d lines of tagRAM and dirtyRAM\n", ID, m_nSize, nSize / sizeof(uint32_t), m_nLines);

  m_pLastAccess = m_nLines;

  Reset();
}


///////////////////////////////////////////////////////////////////////////////
// Destructor
//
CAssociativeCache::~CAssociativeCache()
{
#if 0
  int fd = open("/tmp/cache", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  write(fd, m_pDataRAM, m_nSize);
  close(fd);
#endif

  delete[] m_pDataRAM;
  delete[] m_pTagCAM;
  delete[] m_pDirtyCAM;
}


///////////////////////////////////////////////////////////////////////////////
void CAssociativeCache::Reset()
{
  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::Reset()\n");
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

  // Mark all the tags as invalid
  for (uint32_t i = 0; i < m_nLines; i++) {
    m_pTagCAM[i] = 0x0;
    m_pDirtyCAM[i] = INVALID_BIT;
  }
}


///////////////////////////////////////////////////////////////////////////////
uint32_t* CAssociativeCache::Read_dataRAM(uint32_t addr)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  unsigned int i;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::Read_dataRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS) {
    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READDATA);
    statobject->InspectCacheAccess(addr, 1, DIRECT, pow, ID, ID2);
  }
    
  for (i = 0; i < m_nLines; i++)
    {
      // If it's not this line then continue
      if (m_pTagCAM[i] == tag) {
	m_pLastAccess = i;
	return &m_pDataRAM[i * CACHE_LINE];
      }
    }
  
  // cache miss
  
  // Look for a free line in the cache
  for (i = 0; i < m_nLines; i++)
    {
      // If it's not this line then continue
      if (m_pDirtyCAM[i] == INVALID_BIT)
	break;
    }
  
  if ( i < m_nLines )
    m_pLastAccess = i;
  else {
    // no free space

    uint64_t temp = rand();
    temp *= m_nLines;
    temp /= RAND_MAX;
    m_pLastAccess = temp;
  }

  ASSERT ( m_pLastAccess < m_nLines );
  return &m_pDataRAM[m_pLastAccess * CACHE_LINE];
}

///////////////////////////////////////////////////////////////////////////////
uint32_t CAssociativeCache::Read_tagRAM(uint32_t addr)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::Read_tagRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READTAGRAM);
      statobject->InspectCacheAccess(addr, 1, FASSOC, pow, ID, ID2);
    }

  for (unsigned int i = 0; i < m_nLines; i++)
    {
      // If it's not this line then continue
      if (m_pTagCAM[i] == tag)
	return m_pTagCAM[i];
    }

  if ( m_pLastAccess < m_nLines )
    return m_pTagCAM[m_pLastAccess];
  else
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
uint32_t CAssociativeCache::Read_dirtyRAM(uint32_t addr)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::Read_dirtyRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READDIRTYRAM);
      statobject->InspectCacheAccess(addr, 1, FASSOC, pow, ID, ID2);
    }

  for (unsigned int i = 0; i < m_nLines; i++)
    {
      // If it's not this line then continue
      if (m_pTagCAM[i] == tag)	
	return m_pDirtyCAM[i];
    }

  // cache miss
  if ( m_pLastAccess < m_nLines )
    return m_pDirtyCAM[m_pLastAccess];
  else
    return INVALID_BIT;
}




///////////////////////////////////////////////////////////////////////////////
void CAssociativeCache::WriteLine(uint32_t addr, uint32_t* pLine)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::WriteLine(0x%x, 0x%x)\n", addr, pLine);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());


  // must have had a miss
  ASSERT ( m_pLastAccess != m_nLines );
  
  if (CACHE_WRITE_POLICY != WT)
    ASSERT ( (m_pDirtyCAM[m_pLastAccess] & DIRTY_BIT) != DIRTY_BIT );
  
  m_pTagCAM[m_pLastAccess] = tag;
  m_pDirtyCAM[m_pLastAccess] = 0x0;
  for (int j = 0; j < CACHE_LINE; j++)
    m_pDataRAM[((m_pLastAccess * CACHE_LINE) + j)] = pLine[j];
  
  // last miss is not valid any more because line has been filled
  m_pLastAccess = m_nLines;

  if (POWERSTATS)
    {
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITETAGRAM);
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEDIRTYRAM);
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEDATALINE);
      statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
    }
  
}


///////////////////////////////////////////////////////////////////////////////
void CAssociativeCache::WriteWord(uint32_t addr, uint32_t word, uint32_t bw, uint32_t byte_offset)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  uint32_t word_sel = addr & 0x00000003;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::WriteWord(0x%x, 0x%x, 0x%x, 0x%x)\n", addr, word, bw, byte_offset);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

  //if (POWERSTATS)
  //  pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READTAG);

  // Search for line in the tag CAM
  for (unsigned int i = 0; i < m_nLines; i++)
    if (m_pTagCAM[i] == tag) {
      switch (bw)
	{
	case 0:		// Writing a word
	  {
	    m_pDataRAM[(i * CACHE_LINE) + word_sel] = word;
	  }
	  break;
	case 1:		// Write a byte
	  {
	    uint32_t mask = ~(0xFF << (byte_offset * 8));
	    m_pDataRAM[(i * CACHE_LINE) + word_sel] &= mask;
	    m_pDataRAM[(i * CACHE_LINE) + word_sel] |= ( (word<<(byte_offset*8)) & (~mask) );
	  }
	  break;
	case 2:		// Writing a half word
	  {
	    if ((byte_offset & 0x00000002) == 0)
	      {
		// Modify low half
		m_pDataRAM[(i * CACHE_LINE) + word_sel] &= 0xFFFF0000;
		m_pDataRAM[(i * CACHE_LINE) + word_sel] |= (word & 0x0000FFFF);
	      }
	    else
	      {
		// Modify high half
		m_pDataRAM[(i * CACHE_LINE) + word_sel] &= 0x0000FFFF;
		m_pDataRAM[(i * CACHE_LINE) + word_sel] |= (word << 16);
	      }
	  }
	  break;
	}
      
      m_pDirtyCAM[i] |= DIRTY_BIT;
      break;
    }

  if (POWERSTATS)
    {
      if (CACHE_WRITE_POLICY != WT)
	pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEBIT);
      pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEDATAWORD);
      statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CAssociativeCache::SetBitByAddr(uint32_t addr, uint32_t bit)
{

  // nonsense in an associative cache

  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::SetBitByAddr(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

}

///////////////////////////////////////////////////////////////////////////////
void CAssociativeCache::UnsetBitByAddr(uint32_t addr, uint32_t bit)
{

  // nonsense in an associative cache

  TRACEX(CACHE_TRACEX, 8, "CAssociativeCache::UnsetBitByAddr(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

}

///////////////////////////////////////////////////////////////////////////////
int CAssociativeCache::SetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8,
         "CAssociativeCache::SetBit(0x%x, 0x%x)  - enter  - IDs:%d-%d\n",
         addr, bit, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READTAGRAM);
  
  for (unsigned int i = 0; i < m_nLines; i++)
    {
      if (m_pTagCAM[i] == tag)
	{
	  m_pDirtyCAM[i] |= bit;
	  TRACEX(CACHE_TRACEX, 6,
		 "CAssociativeCache::Setting bit done. IDs: %d-%d addr:0x%x\n",
		 ID, ID2,
		 addr);

	  if (POWERSTATS) {
	    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEBIT);
	    statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
	  }
	  return 1;
	}
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CAssociativeCache::UnsetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  double pow = 0.0;
  
  TRACEX(CACHE_TRACEX, 8,
         "CAssociativeCache::UnsetBit(0x%x, 0x%x)  - enter  - IDs:%d-%d\n",
         addr, bit, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READTAGRAM);
  
  for (unsigned int i = 0; i < m_nLines; i++)
    {
      if (m_pTagCAM[i] == tag)
	{
	  m_pDirtyCAM[i] &= ~bit;
	  TRACEX(CACHE_TRACEX, 6,
		 "CAssociativeCache::Unsetting bit done. IDs: %d-%d addr:0x%x\n",
		 ID, ID2,
		 addr);

	  if (POWERSTATS) {
	    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEBIT);
	    statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
	  }
	  return 1;
	}
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update a word into a cacheline
//
int CAssociativeCache::Update(uint32_t addr, uint32_t data, unsigned char be)
{
  uint32_t tag = addr & 0xFFFFFFFC;
  uint32_t word = addr & 0x00000003;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8,
         "CAssociativeCache::Update(0x%x, 0x%x, %d)  - enter  - IDs:%d-%d\n",
         addr, data, be, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n",
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, READTAGRAM);

  for (unsigned int i = 0; i < m_nLines; i++)
    {
      if (m_pTagCAM[i] != tag)
        continue;

      // Got a hit, so update it
      int mask = (((be&0x8)>>3)*0xFF)<<24 |
             (((be&0x4)>>2)*0xFF)<<16 |
             (((be&0x2)>>1)*0xFF)<<8  |
             ((be&0x1)*0xFF);
      data = (m_pDataRAM[(i * CACHE_LINE) + word] & ~mask) | (data & mask);
      m_pDataRAM[(i * CACHE_LINE) + word] = data;
      m_pDirtyCAM[i] |= DIRTY_BIT;
      TRACEX(CACHE_TRACEX, 6,
	     "CAssociativeCache::Update done. IDs: %d-%d addr:0x%x data:0x%x\n",
	     ID, ID2,
	     addr,
	     data);
      
      if (POWERSTATS) {
	if (CACHE_WRITE_POLICY != WT)
	  pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, WRITEBIT);
        pow += powerAssocCache(ID, ID2, m_nSize, LINE_SIZE_B, UPDATELINE);
        statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
      }
      return 1;
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, FASSOC, pow, ID, ID2);
  return 0;
}
