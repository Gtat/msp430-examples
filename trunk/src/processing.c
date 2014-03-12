#include "processing.h"

uint8_t truncate_sample
  (uint16_t sample, void * nothing)
{
  return ((sample & 0x3FF) >> 2);
}

