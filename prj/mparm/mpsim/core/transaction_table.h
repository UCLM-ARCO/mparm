 ///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         transaction_table.h
// author       DEIS - Universita' di Bologna
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         Transaction table for a bus traffic generator
//
///////////////////////////////////////////////////////////////////////////////

if (ID == 10)
{
  repeat_loop = -1;
  max_trans = 1;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x0a000000;
  burst_array[0] = 8;
  wr_array[0] = false;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
}
else if (ID == 11)
{
  repeat_loop = -1;
  max_trans = 1;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x0b000000;
  burst_array[0] = 4;
  wr_array[0] = false;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
}
else if (ID == 12)
{
  repeat_loop = -1;
  max_trans = 4;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x0a000000;
  burst_array[0] = 8;
  wr_array[0] = false;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
  
  data_array[1] = 0x00000ffe;
  address_array[1] = 0x20003f00;
  burst_array[1] = 8;
  wr_array[1] = false;
  benable_array[1] = TGEN_WORD;
  idlewait_array[1] = 10;
  
  data_array[2] = 0x00000ffe;
  address_array[2] = 0x19e00000;
  burst_array[2] = 8;
  wr_array[2] = false;
  benable_array[2] = TGEN_WORD;
  idlewait_array[2] = 10;
  
  data_array[3] = 0x00000ffe;
  address_array[3] = 0x06000000;
  burst_array[3] = 8;
  wr_array[3] = false;
  benable_array[3] = TGEN_WORD;
  idlewait_array[3] = 10;
}
else if (ID == 13)
{
  repeat_loop = -1;
  max_trans = 1;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x0b000000;
  burst_array[0] = 4;
  wr_array[0] = false;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
}
else if (ID == 14)
{
  repeat_loop = -1;
  max_trans = 1;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x0b000000;
  burst_array[0] = 4;
  wr_array[0] = false;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
}
/*
if (ID == 0)
{
  repeat_loop = 5;
  max_trans = 2;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x00000020;
  burst_array[0] = 1;
  wr_array[0] = true;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
  
  data_array[1] = 0x00000ffe;
  address_array[1] = 0x00000020;
  burst_array[1] = 1;
  wr_array[1] = false;
  benable_array[1] = TGEN_WORD;
  idlewait_array[1] = 10;
}
else if (ID == 1)
{
  repeat_loop = 5;
  max_trans = 2;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x01000030;
  burst_array[0] = 1;
  wr_array[0] = true;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
  
  data_array[1] = 0x00000ffe;
  address_array[1] = 0x01000030;
  burst_array[1] = 1;
  wr_array[1] = false;
  benable_array[1] = TGEN_WORD;
  idlewait_array[1] = 10;
}
else if (ID == 2)
{
  repeat_loop = 3;
  max_trans = 4;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x02000000;
  burst_array[0] = 1;
  wr_array[0] = true;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
  
  data_array[1] = 0x00000ffe;
  address_array[1] = 0x02000000;
  burst_array[1] = 1;
  wr_array[1] = false;
  benable_array[1] = TGEN_WORD;
  idlewait_array[1] = 10;
  
  data_array[2] = 0x00000ffe;
  address_array[2] = 0x02000020;
  burst_array[2] = 1;
  wr_array[2] = true;
  benable_array[2] = TGEN_WORD;
  idlewait_array[2] = 10;
  
  data_array[3] = 0x00000ffe;
  address_array[3] = 0x02000020;
  burst_array[3] = 1;
  wr_array[3] = false;
  benable_array[3] = TGEN_WORD;
  idlewait_array[3] = 10;
}
else if (ID == 3)
{
  repeat_loop = 5;
  max_trans = 2;
  
  data_array = new uint32_t [max_trans];
  address_array = new uint32_t [max_trans];
  burst_array = new uint [max_trans];
  wr_array = new bool [max_trans];
  benable_array = new TGEN_BENABLE [max_trans];
  idlewait_array = new int [max_trans];
  
  data_array[0] = 0x00000ffe;
  address_array[0] = 0x03000030;
  burst_array[0] = 1;
  wr_array[0] = true;
  benable_array[0] = TGEN_WORD;
  idlewait_array[0] = 10;
  
  data_array[1] = 0x00000ffe;
  address_array[1] = 0x03000030;
  burst_array[1] = 1;
  wr_array[1] = false;
  benable_array[1] = TGEN_WORD;
  idlewait_array[1] = 10;
}
*/
