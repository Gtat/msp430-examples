/* @file    ram_symbols.h
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * Routines to copy code into RAM and look up RAM pointers
 * from their symbols.
 */

#ifndef __RAM_ROUTINES_H_GUARD
#define __RAM_ROUTINES_H_GUARD

#include <stdint.h>
#include <stddef.h>

#define RAM_CODE_LENGTH 0x80
extern uint8_t ram_code_rom_begin;
extern uint8_t ram_code_rom_end;
extern uint8_t ram_code[RAM_CODE_LENGTH]
  __attribute__ (( section(".ram_symbols_in_ram") ));

/* compute the offset of a given symbol
 * from the start of the reserved RAM region
 * given its static (ROM) address
 */
#define OFFSET_OF_RAM_SYMBOL(ptr) \
  ((size_t)(ptr) - (size_t)&ram_code_rom_begin)
/* produces a reference to an identifier that lives in RAM
 * given its static (ROM) address
 */
#define RAM_CODE_PTR(s)                                                \
  ({                                                                   \
     typeof(&s) __p __attribute__(( unused ));                         \
     __p = (typeof(&s))((size_t)&ram_code + OFFSET_OF_RAM_SYMBOL(&s)); \
  })

void ram_routine_load
  (void);

#endif /* __RAM_ROUTINES_H_GUARD */
