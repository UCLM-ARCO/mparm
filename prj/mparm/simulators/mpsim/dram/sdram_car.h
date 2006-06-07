//File con le caratteristiche della SDRAM
#include "config.h"

#ifndef __SDRAM_CAR_H__
#define __SDRAM_CAR_H__



//indirizzo in cui la SDRAM e' mappata in memoria definito in config.h 
//#define DRAM_BASE

//Dimensione della SDRAM in Megabit
/* 
SDRAM supportate:64,128,256,512 Megabit
SDRAM da 64 Megabit:8 bit colonna,2 bit banco,11 bit riga,pagine da 256 locazioni
SDRAM da 128 Megabit:8 bit colonna,2 bit banco,12 bit riga,pagine da 256 locazioni
SDRAM da 256 Megabit:9 bit colonna,2 bit banco,12 bit riga,pagine da 512 locazioni
SDRAM da 512 Megabit:9 bit colonna,2 bit banco,13 bit riga,pagine da 512 locazioni
*/
#define SDRAM_SIZE ((DRAM_SIZE*8)/(1024*1024))
// definisco la size della ram in parole da 32 bit
// ad esempio 64 megabit corrispondono a 2 megaword
// moltiplico per 1024 e poi per 1024/32=32

#define SDRAM_SIZE_WORD SDRAM_SIZE*1024*32


//Definizione delle tempistiche in cicli di clock
// TRCD da ACTIVE a READ/WRITE: 3,4,...
#define TRCD_SDRAM 3

// TRRD da ACTIVE BX a ACTIVE BY
// Questo tempo alla fine non viene usato perchè non capita mai con questo LMI
// di avere due comandi ACTIVE molto vicini
#define TRRD_SDRAM 2


// TRAS da ACTIVE a PRECHARGE
#define TRAS_SDRAM 5

// Neanche questo tempo viene modellato perchè prima di ogni active c'è un precharge
// TRC da ACTIVE BX a ACTIVE BX
#define TRC_SDRAM 11

// TCAS per la READ:da READ a DATA_OUT: 1,2,3
// Il controllo sul TCAS va fatto solo la prima volta dopo ACTIVE o dopo la ripresa
// perche' se ci sono i dati sufficienti l'operazione viene fatta in modo continuato
#define TCAS_SDRAM 3

// TWR per la write: da WRITE a PRECHARGE: 1 o 2
#define TWR_SDRAM 2

// TRP da PRECHARGE a ACTIVE
#define TRP_SDRAM 3

// Definizione della dimensione del burst:2,4,8
#define DIMBURST 2


#endif //__SDRAM_CAR_H__
