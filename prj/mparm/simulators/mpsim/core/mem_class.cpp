///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         mem_class.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a generic memory class
//
///////////////////////////////////////////////////////////////////////////////

#include "mem_class.h"
#include "address.h"
#include <stdlib.h>
#include <fcntl.h>
#ifdef WIN32
#include <IO.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////////////
// hexstringtonumber - Translates a string representing a hex into a number.
long Mem_class::hexstringtonumber(char *str, int start, int len)
{
  long iValue;
  char sValue[256];
  int iCur;

  for(iCur = 0; iCur < len; iCur++)
    sValue[iCur] = str[start+iCur];
  sValue[len] = '\0';
  iValue = strtol(sValue, NULL, 16);
  //printf("String: %s Value: %d\n", sValue, iValue);
  return iValue;
}


///////////////////////////////////////////////////////////////////////////////
// load_program - Tries to load a binary image onto the simulated memory.
int Mem_class::load_program(char * image_filename)
{
  int fd;
  struct stat s;

  if (image_filename == "")
    return EXIT_FAILURE;

  fd = open(image_filename, O_RDONLY);
  if (fd == -1)
  {
    cerr << "Error Uploading Program Binary: " << image_filename << endl;
    return EXIT_FAILURE;
  }

  fstat(fd, &s);
  read(fd, myMemory, s.st_size);
  close(fd);
  cout << "Uploaded Program Binary: " << image_filename << endl;
  return EXIT_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// load_srec_program - Tries to load an s-record onto the simulated memory.
//                     supports only 32bit addresses and transfer:
//                     S3   Data record with 32 bit load address
//                     S7   Termination record with 32 bit transfer address
//
//                     Stnnaaaaaaaa[dddd...dddd]cc
//                     t     record type field (0,1,2,3,6,7,8,9).
//                     nn    record length field, number of bytes in record
//                           excluding record type and record length.
//                     a...a load address field, can be 16, 24 or 32 bit address
//                           for data to be loaded.
//                     d...d data field, actual data to load, each byte is
//                           encoded in 2 characters.
//                     cc    checksum field, 1's complement of the sum of all
//                           bytes in the record length, load address and data
//                           fields.
int Mem_class::load_srec_program(char * srec_filename)
{
  FILE *fd;
  char str[4096];
  int CountBytes, iCur, iValue;
  unsigned long Address, MinAddress, MaxAddress;

  if (srec_filename == "")
    {
      cerr << "Note: No Program SRec to Upload" << endl;
      return EXIT_SUCCESS;
    }
  MinAddress = 0xffffffff;
  MaxAddress = 0;

  fd = fopen(srec_filename, "r");
  if (fd == NULL)
  {
    cerr << "Error Uploading Program SRec: " << srec_filename << endl;
    return EXIT_FAILURE;
  }

  while(fgets(str,4096,fd) != NULL)
    {
      if(str[0] != 'S')
	{
	  fprintf(stderr, "Error: Not a SRec line\n");
	  continue;
	}
      switch(str[1])
	{
	case '2':
	  CountBytes = hexstringtonumber(str,2,2);
	  Address = hexstringtonumber(str,4,6);
	  if(MinAddress > Address) MinAddress = Address;
	  if(MaxAddress < Address) MaxAddress = Address;
	  for(iCur = 10; iCur < (CountBytes-4)*2+10; iCur+=2)
	    {
	      iValue = hexstringtonumber(str,iCur,2);
	      myMemory[Address++] = iValue;
	      //printf("Just read: %x is %x\n", pMemory[Address-1], iValue);
	    }
	  break;
	case '3':
	  CountBytes = hexstringtonumber(str,2,2);
	  Address = hexstringtonumber(str,4,8);
	  for(iCur = 12; iCur < (CountBytes-5)*2+12; iCur+=2)
	    {
	      iValue = hexstringtonumber(str,iCur,2);
	      myMemory[Address++] = iValue;
	      //printf("Just read: %x is %x\n", pMemory[Address-1], iValue);
	    }
	  break;
	case '7':
	  break;
	}
    }
  fclose(fd);
  printf("Uploaded Program SRec %s between addresses[hex]: %lx to %lx\n", srec_filename, MinAddress, MaxAddress);
  return EXIT_SUCCESS;
}

