///////////////////////////////////////////////////////////////////////////////
// Copyright 2001 Michael Dales
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
// name   setassoc.cpp
// author Michael Dales (michael@dcs.gla.ac.uk)
// header setassoc.h
// info   Implements a n-way set associative cache. I use n direct map caches
//        to implement this. Note that there is a bit of wastage here, as
//        the set bits will be stored in the tag inside the direct map cache,
//        but this isn't going to effect the behaviour of the cache, so we l
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "swarm.h"
#include "setassoc.h"
#include "direct.h"
#include "stats.h"
#include <string.h>


// martin letis - this is redundant - using CACHE_LINE
// #define LINE_SIZE_W 4  /* (words) */
#define LINE_SIZE_B 16 /* (bytes) */


///////////////////////////////////////////////////////////////////////////////
CSetAssociativeCache::CSetAssociativeCache(uint32_t nSize)
{
  m_nSize = nSize;
  m_nWay = 2;
  InitSets();
}

CSetAssociativeCache::CSetAssociativeCache(uint32_t nSize, int nWay)
{
  m_nSize = nSize;
  m_nWay = nWay;
  InitSets();
}


///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::InitSets()
{

  // Allocate the direct mapped caches
  m_pSets = new CDirectCache*[m_nWay];
  
  // Now Allocate the state to enable us to do the RR on lines
  m_pSetRR = new uint8_t[m_nSize/m_nWay];
  memset(m_pSetRR, 0, sizeof(uint8_t) * (m_nSize/m_nWay));

  // Calc some useful into here
  m_tagSelMask = ((m_nSize/m_nWay) / LINE_SIZE_B) - 1;
  
  uint32_t temp = m_tagSelMask;
  m_tagBits = 0;
  while (temp != 0)
    {
      temp = temp >> 1;
      m_tagBits++;
    }

  m_tagBits += 2;

  //printf("Instantiating setassoc cache of %d bytes - %d ways - m_tagSelMask %u\n", m_nSize, m_nWay, m_tagSelMask);
  for (int i = 0; i < m_nWay; i++)
    m_pSets[i] = new CDirectCache(m_nSize/m_nWay);

}


///////////////////////////////////////////////////////////////////////////////
CSetAssociativeCache::~CSetAssociativeCache()
{
  for (int i = 0; i < m_nWay; i++)
    delete m_pSets[i];
  delete m_pSets;
  delete m_pSetRR;
}


///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::Reset()
{
  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::Reset()\n");
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  for (int i = 0; i < m_nWay; i++)
    m_pSets[i]->Reset();
}


///////////////////////////////////////////////////////////////////////////////
uint32_t* CSetAssociativeCache::Read_dataRAM(uint32_t addr)
{
  uint32_t* dataRAM = 0;
  uint32_t tagRAM;
  int i;
  int tag_sel;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::Read_dataRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
#ifndef SEQUENTIAL_TAG_DATA_CACHE_ACCESS
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READDATA);
#endif
    }

  tag_sel = (addr >> 2) & m_tagSelMask;
  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      dataRAM = m_pSets[i]->Read_dataRAM(addr);
      tagRAM = m_pSets[i]->Read_tagRAM(addr);

      if (tagRAM == addr) // cache HIT
	break;

    }
  if (i == m_nWay)
    {
      dataRAM = m_pSets[m_pSetRR[tag_sel]]->Read_dataRAM(addr);
    }

  if (POWERSTATS) {
#ifdef SEQUENTIAL_TAG_DATA_CACHE_ACCESS
    pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READDATA);
#endif
    statobject->InspectCacheAccess(addr, 1, SETASSOC, pow, ID, ID2);
  }
  return dataRAM;
}

///////////////////////////////////////////////////////////////////////////////
uint32_t CSetAssociativeCache::Read_tagRAM(uint32_t addr)
{
  uint32_t tagRAM = 0;
  int i;
  int tag_sel;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::Read_tagRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READTAGRAM);
    }

  tag_sel = (addr >> 2) & m_tagSelMask;

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      tagRAM = m_pSets[i]->Read_tagRAM(addr);
      
      if (tagRAM == addr) // cache HIT
	break;
    }
  if (i == m_nWay)
    {
      tagRAM = m_pSets[m_pSetRR[tag_sel]]->Read_tagRAM(addr);
    }


  if (POWERSTATS)
    {
      statobject->InspectCacheAccess(addr, 1, SETASSOC, pow, ID, ID2);
    }

  // Then try the read. Don't catch any errors, leave that for higher up
  return tagRAM;
}


///////////////////////////////////////////////////////////////////////////////
uint32_t CSetAssociativeCache::Read_dirtyRAM(uint32_t addr)
{
  uint32_t tagRAM;
  uint32_t dirtyRAM = 0;
  int tag_sel;
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::Read_dirtyRAM(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READDIRTYRAM);
    }

  tag_sel = (addr >> 2) & m_tagSelMask;

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      tagRAM = m_pSets[i]->Read_tagRAM(addr);
      dirtyRAM = m_pSets[i]->Read_dirtyRAM(addr);
      
      if (tagRAM == addr) // cache HIT
	break;
    }
  if (i == m_nWay)
    {
      dirtyRAM = m_pSets[m_pSetRR[tag_sel]]->Read_dirtyRAM(addr);
    }


  if (POWERSTATS)
    {
      statobject->InspectCacheAccess(addr, 1, SETASSOC, pow, ID, ID2);
    }

  // Then try the read. Don't catch any errors, leave that for higher up
  return dirtyRAM;
}

///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::WriteLine(uint32_t addr, uint32_t* pLine)
{
  int tag_sel;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::WriteLine(0x%x, 0x%x)\n", addr, pLine);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    {
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITETAGRAM);
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEDIRTYRAM);
      pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEDATALINE);
      statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
    }

  // Work out which line in a set it would be written to
  tag_sel = (addr >> 2) & m_tagSelMask;


  // Now write the line to that set
  m_pSets[m_pSetRR[tag_sel]]->WriteLine(addr, pLine);

  // Update the RR info
  m_pSetRR[tag_sel]++;
  if (m_pSetRR[tag_sel] == m_nWay)
    m_pSetRR[tag_sel] = 0;
}

///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::WriteWord(uint32_t addr, uint32_t word, uint32_t bw, uint32_t byte_offset)
{
  uint32_t tagRAM;
  uint32_t dirtyRAM;
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::WriteWord(0x%x, 0x%x, 0x%x, 0x%x)\n", addr, word, bw, byte_offset);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  //if (POWERSTATS)
  //  pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READTAGRAM);

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      tagRAM = m_pSets[i]->Read_tagRAM(addr);
      dirtyRAM = m_pSets[i]->Read_dirtyRAM(addr);

      if ( (tagRAM == addr) && ((dirtyRAM&INVALID_BIT)!=INVALID_BIT) ) { // cache HIT
	if (POWERSTATS)
	  pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEDATAWORD);
	m_pSets[i]->WriteWord(addr, word, bw, byte_offset);
	break;
      }
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
}

///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::SetBitByAddr(uint32_t addr, uint32_t bit)
{
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::SetBitByAddr(0x%x, 0x%x)\n", addr, bit);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      if (POWERSTATS)
	pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEBIT);
      m_pSets[i]->SetBitByAddr(addr, bit);
      break;
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
  
}

///////////////////////////////////////////////////////////////////////////////
void CSetAssociativeCache::UnsetBitByAddr(uint32_t addr, uint32_t bit)
{
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::UnsetBitByAddr(0x%x, 0x%x)\n", addr, bit);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      if (POWERSTATS)
	pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEBIT);
      m_pSets[i]->UnsetBitByAddr(addr, bit);
      break;
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
  
}

///////////////////////////////////////////////////////////////////////////////
int CSetAssociativeCache::SetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tagRAM;
  uint32_t dirtyRAM;
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::SetBitByAddr(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      tagRAM = m_pSets[i]->Read_tagRAM(addr);
      dirtyRAM = m_pSets[i]->Read_dirtyRAM(addr);

      if ( (tagRAM == addr) && ((dirtyRAM&INVALID_BIT)!=INVALID_BIT) ) { // cache HIT
	if (POWERSTATS)
	  pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEBIT);
	int ret = m_pSets[i]->SetBit(addr, bit);
	ASSERT (ret == 1);
	return ret;
      }
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
  return 0;

}

///////////////////////////////////////////////////////////////////////////////
int CSetAssociativeCache::UnsetBit(uint32_t addr, uint32_t bit)
{
  uint32_t tagRAM;
  uint32_t dirtyRAM;
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8, "CSetAssociativeCache::UnsetBitByAddr(0x%x)\n", addr);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      tagRAM = m_pSets[i]->Read_tagRAM(addr);
      dirtyRAM = m_pSets[i]->Read_dirtyRAM(addr);

      if ( (tagRAM == addr) && ((dirtyRAM & INVALID_BIT) != INVALID_BIT) ) { // cache HIT
	if (POWERSTATS)
	  pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, WRITEBIT);
	int ret = m_pSets[i]->SetBit(addr, bit);
	m_pSets[i]->UnsetBit(addr, bit);
	ASSERT (ret == 1);
	return ret;
      }
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
  return 0;

}

///////////////////////////////////////////////////////////////////////////////
int CSetAssociativeCache::Update(uint32_t addr, uint32_t data, unsigned char be)
{
  int i;
  double pow = 0.0;

  TRACEX(CACHE_TRACEX, 8,
         "CSetAssociativeCache::Update(0x%x, 0x%x, %d)  - enter  - IDs:%d-%d\n",
         addr, data, be, ID, ID2);
  TRACEX(CACHE_TRACEX, 10, "  IDs %d-%d  time: %10.1f\n", 
         ID, ID2, sc_simulation_time());

  if (POWERSTATS)
    pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, READTAGRAM);

  // Try all our sub caches, and if we don't find one then barf
  for (i = 0; i < m_nWay; i++)
    {
      if (m_pSets[i]->Update(addr, data, be))
	{
	  TRACEX(CACHE_TRACEX, 7,
		 "CSetAssociativeCache::Update done. IDs: %d-%d addr:0x%x "
		 "data:0x%x\n",
		 ID, ID2,
		 addr,
		 data);
	
	  if (POWERSTATS) {
	    pow += powerSetAssocCache(ID, ID2, m_nSize, m_nWay, UPDATELINE);
	    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
	  }
	  return 1;
	}
    }
  
  if (POWERSTATS)
    statobject->InspectCacheAccess(addr, 0, SETASSOC, pow, ID, ID2);
  return 0;
}

