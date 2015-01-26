__flash_segment_size = 0x200;
.flash_storage :
{
  . = ALIGN(__flash_segment_size);
  __start_flash_storage = .;
  parameter.o (.flash_storage)
  . = ALIGN(__flash_segment_size);
  protocol.o  (.flash_storage)
  . = ALIGN(__flash_segment_size);

  __end_flash_storage = .;
} > ROM
