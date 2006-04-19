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
// name   direct.cpp
// author Michael Dales (michael@dcs.gla.ac.uk)
// header direct.h
// info   Implements a direct mapped cache. Note that, rather confusingly,
//        the size is specified in bytes, but the address given for reads
//        and writes is in words.
//
///////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include "globals.h"

#include "swarm.h"
#include "direct.h"
#include "stats.h"

// martin letis - this is redundant - using CACHE_LINE
// #define CACHE_LINE 4  /* (words) */
// INVALID_BIT defined in cache.h
#define LINE_SIZE_B 16 /* (bytes) */


///////////////////////////////////////////////////////////////////////////////
// CDirectCache - Constructor
//
CDirectCache::CDirectCache(uint32_t nSize)
{
  // XXX: Gross hack - must go...sometime
  m_nSize = nSize;
  m_nLines = nSize / LINE_SIZE_B;

  m_pDataRAM = new uint32_t[nSize / sizeof(uint32_t)];
  m_pTagRAM = new uint32_t[m_nLines];
  m_pDirtyRAM = new uint32_t[m_nLines];

  m_tagSelMask = m_nLines - 1;
  
  uint32_t temp = m_tagSelMask;
  m_tagBits = 0;
  while (temp != 0)
    {
      temp = temp >> 1;
      m_tagBits++;
    }

  /* dummy values... to avoid ambiguity in setassociative caches */
  ID = ID2 = 0x0F0F;  

  m_tagMask = ~((m_tagSelMask<<2)|0x00000003);

  //printf ("Instantiating direct cache of %d bytes - %d words of dataRAM - %d lines of tagRAM and dirtyRAM - m_tagMask %u - m_tagSelMask %u\n", m_nSize, nSize / sizeof(uint32_t), m_nLines, m_tagMask, m_tagSelMask);


  Reset();
}


///////////////////////////////////////////////////////////////////////////////
// ~CDirectCache - Destructor
//
CDirectCache::~CDirectCache()
{
#if 0
  int fd = open("/tmp/cache", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  write(fd, m_pDataRAM, m_nSize);
  close(fd);
#endif

  delete[] m_pTagRAM;
  delete[] m_pDataRAM;
  delete[] m_pDirtyRAM;
}


///////////////////////////////////////////////////////////////////////////////
uint32_t* CDirectCache::Read_dataRAM(uint32_t addr)
{
  uint32_t word_sel, tag_sel, tag;
  double pow = 0.0;
 
  TRACEX(CACHE_TRACEX, 8, "CDirectCache::Read_dataRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Spilt the address up into the bits we want 
  word_sel = addr & 0x00000003;
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;
  
  if (POWERSTATS)
    { 
      pow += powerDirectCache(ID, ID2, m_nSize, READDATA);
      statobject->InspectCacheAccess(addr, 1, DIRECT, pow, ID, ID2);
    }
  
  return &(m_pDataRAM[(tag_sel << 2)]);
}

///////////////////////////////////////////////////////////////////////////////
// BIG HACK: non returning only tag but (address)
uint32_t CDirectCache::Read_tagRAM(uint32_t addr)
{
  uint32_t word_sel, tag_sel, tag;
  double pow = 0.0;
 
  TRACEX(CACHE_TRACEX, 8, "CDirectCache::Read_tagRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Spilt the address up into the bits we want 
  word_sel = addr & 0x00000003;
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;
  
  if (POWERSTATS)
    { 
      pow += powerDirectCache(ID, ID2, m_nSize, READTAGRAM);
      statobject->InspectCacheAccess(addr, 1, DIRECT, pow, ID, ID2);
    }
  
  ASSERT ( m_pTagRAM[tag_sel] == (m_pTagRAM[tag_sel] & m_tagMask) ) ;

  return m_pTagRAM[tag_sel] | ( addr & ~m_tagMask);
}

///////////////////////////////////////////////////////////////////////////////
uint32_t CDirectCache::Read_dirtyRAM(uint32_t addr)
{
  uint32_t word_sel, tag_sel, tag;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CDirectCache::Read_dirtyRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
	 ID, ID2, sc_simulation_time());

  // Spilt the address up into the bits we want 
  word_sel = addr & 0x00000003;
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;
  
  if (POWERSTATS)
    { 
      pow += powerDirectCache(ID, ID2, m_nSize, READDIRTYRAM);
      statobject->InspectCacheAccess(addr, 1, DIRECT, pow, ID, ID2);
    }

  return m_pDirtyRAM[tag_sel];

}

///////////////////////////////////////////////////////////////////////////////
void CDirectCache::WriteLine(uint32_t addr, uint32_t* pLine)
{
  uint32_t /*word_sel,*/ tag_sel, tag;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CDirectCache::WriteLine(0x%x, 0x%x)\n", addr, pLine);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Split the address up into the bits we want 
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;

#if 0
  if ((m_pDirtyRAM[tag_sel] & INVALID_BIT) == 0)
    fprintf(stderr, "\t--- line %03d for 0x%08x\n", tag_sel, addr);

  fprintf(stderr, "\t+++ line %03d for 0x%08x\n", tag_sel, addr);
#endif

  // cant refill a dirty line
  // armproc must have written it to memory first
  if (CACHE_WRITE_POLICY != WT)
    ASSERT ( (m_pDirtyRAM[tag_sel] & DIRTY_BIT) != DIRTY_BIT );

  m_pTagRAM[tag_sel] = tag;
  m_pDirtyRAM[tag_sel] = 0x0;
  for (int i = 0; i < CACHE_LINE; i++)
    m_pDataRAM[(tag_sel << 2) + i] = pLine[i];

  if (POWERSTATS)
    {
      pow += powerDirectCache(ID, ID2, m_nSize, WRITETAGRAM);
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEDIRTYRAM);
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEDATALINE);
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    }
}


///////////////////////////////////////////////////////////////////////////////
void CDirectCache::WriteWord(uint32_t addr, uint32_t word, uint32_t bw, uint32_t byte_offset)
{
  uint32_t word_sel, tag_sel, tag;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CDirectCache::WriteWord(0x%x, 0x%x, 0x%x, 0x%x)\n", addr, word, bw, byte_offset);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Spilt the address up into the bits we want 
  word_sel = addr & 0x00000003;
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;
  
  //if (POWERSTATS)
  //  pow += powerDirectCache(ID, ID2, m_nSize, READTAG);

  // writing into a valid location
  ASSERT (m_pTagRAM[tag_sel] == tag);

  // arrange word according to bw
  switch (bw)
    {
    case 0:		// Writing a word
      {
	m_pDataRAM[(tag_sel << 2) + word_sel] = word;
      }
      break;
    case 1:		// Write a byte
      {
	uint32_t mask = ~(0xFF << (byte_offset * 8));
	m_pDataRAM[(tag_sel << 2) + word_sel] &= mask;
	m_pDataRAM[(tag_sel << 2) + word_sel] |= ( (word<<(byte_offset*8)) & (~mask) );
      }
      break;
    case 2:		// Writing a half word
      {
	if ((byte_offset & 0x00000002) == 0)
	  {
	    // Modify low half
	    m_pDataRAM[(tag_sel << 2) + word_sel] &= 0xFFFF0000;
	    m_pDataRAM[(tag_sel << 2) + word_sel] |= (word & 0x0000FFFF);
	  }
	else
	  {
	    // Modify high half
	    m_pDataRAM[(tag_sel << 2) + word_sel] &= 0x0000FFFF;
	    m_pDataRAM[(tag_sel << 2) + word_sel] |= (word << 16);
	  }
      }
      break;
    }
  
  m_pDirtyRAM[tag_sel] |= DIRTY_BIT;

  if (POWERSTATS)
    {
      if (CACHE_WRITE_POLICY != WT)
	pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEDATAWORD);
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CDirectCache::SetBitByAddr(uint32_t addr, uint32_t bit)
{
  uint32_t tag_sel;
  double pow = 0.0;
 
  TRACEX(CACHE_TRACEX, 8,
         "CDirectCache::SetBitByAddr(0x%x, 0x%x)\n", addr, bit);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  tag_sel = (addr >> 2) & m_tagSelMask;

  m_pDirtyRAM[tag_sel] |= bit;

  if (POWERSTATS)
    {
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CDirectCache::UnsetBitByAddr(uint32_t addr, uint32_t bit)
{
  uint32_t tag_sel;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8,
         "CDirectCache::UnsetBitByAddr(0x%x, 0x%x)\n", addr, bit);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  tag_sel = (addr >> 2) & m_tagSelMask;

  m_pDirtyRAM[tag_sel] &= ~bit;

  if (POWERSTATS)
    {
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    }
}

///////////////////////////////////////////////////////////////////////////////
int CDirectCache::SetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tag_sel, tag;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8,
         "CDirectCache::SetBit(0x%x, 0x%x)  - enter  - IDs:%d-%d\n",
         addr, bit, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;

  if (POWERSTATS)
    pow += powerDirectCache(ID, ID2, m_nSize, READTAGRAM);

  if (m_pTagRAM[tag_sel] != tag) {
    // cache miss: don't set bit
    if (POWERSTATS)
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    return 0;
  }

  m_pDirtyRAM[tag_sel] |= bit;

  TRACEX(CACHE_TRACEX, 6,
	 "CDirectCache::Setting bit done. IDs: %d-%d addr:0x%x\n",
	 ID, ID2,
	 addr);

  if (POWERSTATS) {
    pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
    statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
  }
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
int CDirectCache::UnsetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tag_sel, tag;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8,
         "CDirectCache::UnsetBit(0x%x, 0x%x)  - enter  - IDs:%d-%d\n",
         addr, bit, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;

  if (POWERSTATS)
    pow += powerDirectCache(ID, ID2, m_nSize, READTAGRAM);

  if (m_pTagRAM[tag_sel] != tag) {
    // cache miss: don't set bit
    if (POWERSTATS)
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    return 0;
  }

  m_pDirtyRAM[tag_sel] &= ~bit;

  TRACEX(CACHE_TRACEX, 6,
	 "CDirectCache::Unsetting bit done. IDs: %d-%d addr:0x%x\n",
	 ID, ID2,
	 addr);

  if (POWERSTATS) {
    pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
    statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
  }
  return 1;
}






///////////////////////////////////////////////////////////////////////////////
void CDirectCache::Reset()
{
  TRACEX(CACHE_TRACEX, 8, "CDirectCache::Reset()\n");
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  for (uint32_t i = 0; i < m_nLines; i++) {
    m_pDirtyRAM[i] = INVALID_BIT;
    m_pTagRAM[i] = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
int CDirectCache::Update(uint32_t addr, uint32_t data, unsigned char be)
{
  uint32_t word_sel, tag_sel, tag;
  double pow = 0.0;

  //ASSERT(be==0xF);  // 32-bit cache: data must be 4 byte

  TRACEX(CACHE_TRACEX, 8,
         "CDirectCache::Update(0x%x, 0x%x, %d)  - enter  - IDs:%d-%d\n",
         addr, data, be, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  word_sel = addr & 0x00000003;
  tag_sel = (addr >> 2) & m_tagSelMask;
  tag = addr & m_tagMask;

  if (POWERSTATS)
    pow += powerDirectCache(ID, ID2, m_nSize, READTAGRAM);

  if (m_pTagRAM[tag_sel] != tag) {
    // cache miss
    if (POWERSTATS)
      statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
    return 0;
  }

  int mask = (((be&0x8)>>3)*0xFF)<<24 |
             (((be&0x4)>>2)*0xFF)<<16 |
             (((be&0x2)>>1)*0xFF)<<8  |
             ((be&0x1)*0xFF);
  data = ( m_pDataRAM[(tag_sel << 2) + word_sel] & ~mask) | (data & mask);
  m_pDataRAM[(tag_sel << 2) + word_sel] = data;
  
  m_pDirtyRAM[tag_sel] |= DIRTY_BIT;

  TRACEX(CACHE_TRACEX, 6,
	 "CDirectCache::Update done. IDs: %d-%d addr:0x%x data:0x%x\n",
	 ID, ID2,
	 addr,
	 data);

  if (POWERSTATS) {
    if (CACHE_WRITE_POLICY != WT)
      pow += powerDirectCache(ID, ID2, m_nSize, WRITEBIT);
    pow += powerDirectCache(ID, ID2, m_nSize, UPDATELINE);
    statobject->InspectCacheAccess(addr, 0, DIRECT, pow, ID, ID2);
  }
  return 1;
}

