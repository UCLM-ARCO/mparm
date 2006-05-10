

/*
 POLETTI FRANCESCO
 */

#ifndef INT_BSPDMA_H
#define INT_BSPDMA_H
inline int int_newobject(uint sizerow,uint numberofrow,uint rl1,uint rl2,uint l1add,uint
		l2add,uint state, rtems_boolean waitdma);
		
inline void int_newsrow(register uint parameter,register int object);					

inline void int_newnrow(register uint parameter,register int object);

inline void int_newrl1(register uint parameter,register int object);

inline void int_newrl2(register uint parameter,register int object);

inline void int_newl1add(register uint parameter,register int object);

inline void int_newl2add(register uint parameter,register int object);

inline void int_newstate(register uint state,register int object,register rtems_boolean waitdma);
inline void int_newstate1(register uint state,register int object);
inline void int_wait(register int object);
inline uint int_return_state(register int object);

inline void int_free_dma(int object);
#endif /* INT_BSPDMA_H */



