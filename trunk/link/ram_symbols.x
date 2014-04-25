  __ram_code_length = 0x100;
  /* Provide a section for continuous storage of code that must be executed
   * from RAM
   */
  .ram_symbols_in_rom :
  {
    . = ALIGN(2);
    ram_code_rom_begin = .;
    *(.ram_symbols)
    ram_code_rom_end = .;
  } > rom 
  /* Provide a RAM buffer where this code will be copied */
  .ram_symbols_in_ram :
  {
    . = ALIGN(2);
    ram_symbols.o (.ram_symbols_in_ram)
  } > ram

