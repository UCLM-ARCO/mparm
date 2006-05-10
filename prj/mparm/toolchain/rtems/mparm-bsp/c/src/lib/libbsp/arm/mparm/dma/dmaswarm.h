

/*
 POLETTI FRANCESCO
 */

#ifndef EXT_BSPDMA_H
#define EXT_BSPDMA_H
inline int newobject(uint dma, uint sizerow,uint numberofrow,uint rl1,uint rl2,uint l1add,uint
		l2add,uint state, uint waitdma);
		
inline void newsrow(register uint dma,register uint parameter,register int object);					

inline void newnrow(register uint dma,register uint parameter,register int object);

inline void newrl1(register uint dma, register uint parameter,register int object);

inline void newrl2(register uint dma, register uint parameter,register int object);

inline void newl1add(register uint dma,register  uint parameter,register int object);

inline void newl2add(register uint dma, register uint parameter,register int object);

inline void newstate(register uint dma, register uint state,register int object, rtems_boolean waitdma);
inline void newstate1(register uint dma, register uint state,register int object);

inline void newstateint(register uint dma,register uint state,register int object, rtems_boolean waitdma);
inline void newstateint1(register uint dma,register uint state,register int object);

inline void wait(register uint dma,register  int object);
inline void waitint(register uint dma,register int object);
inline uint return_state(register uint dma,register int object);

inline void free_dma(uint dma, int object);

/*Initialize the variable ext_dma_base */
inline void dma_driver_initialize();

/*Set the interrupt*/
void ext_Setdma();
#endif /* EXT_BSPDMA_H */



