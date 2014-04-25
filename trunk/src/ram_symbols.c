#include "ram_symbols.h"

uint8_t ram_code[RAM_CODE_LENGTH];

void ram_routine_load
  (void)
{
  uint8_t *p;
  size_t   i, length;

  length = (size_t)&ram_code_rom_end - (size_t)&ram_code_rom_begin;
  for (i = 0, p = &ram_code_rom_begin; i < length; ++i)
  {
    ram_code[i] = *p++;
  }
}

