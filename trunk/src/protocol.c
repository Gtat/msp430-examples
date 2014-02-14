#include "protocol.h"
#include "drivers/usci.h"

int send_packet
  (union mcu_to_pc *p)
{
  int i;
  for (i = 0; i < sizeof(union mcu_to_pc); ++i)
  {
    putchar(p->bytes[i]);
  }

  return i;
}

