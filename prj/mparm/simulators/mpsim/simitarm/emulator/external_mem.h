#ifndef EXTERNAL_MEM_H
#define EXTERNAL_MEM_H

byte_t     ext_read_byte     (int);
halfword_t ext_read_half_word(int);
word_t     ext_read_word     (int);
dword_t    ext_read_dword    (int);

void ext_write_byte     (int, byte_t    );
void ext_write_half_word(int, halfword_t);
void ext_write_word     (int, word_t    );
void ext_write_dword    (int, dword_t   );


#endif
