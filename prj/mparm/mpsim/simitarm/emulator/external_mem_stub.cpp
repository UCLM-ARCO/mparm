#include "misc.h"
#include "external_mem.h"
#include "emumem.h"

byte_t     ext_read_byte     (int) {
  return 0;
}

halfword_t ext_read_half_word(int) {
  return 0;
}

word_t     ext_read_word     (int) {
  return 0;
}

dword_t    ext_read_dword    (int) {
  return 0;
}

void ext_write_byte     (int, byte_t    ) {}
void ext_write_half_word(int, halfword_t) {}
void ext_write_word     (int, word_t    ) {}
void ext_write_dword    (int, dword_t   ) {}
