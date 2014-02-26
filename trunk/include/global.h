#ifndef __GLOBAL_H_GUARD
#define __GLOBAL_H_GUARD

#define NUM_SIGNAL_CHS (6)
#define NUM_TOTAL_CHS  (8)
#define ADC_CH_MASK    (0xCF)

#include <msp430.h>
#include "ringq.h"

union word
{
  int  word;
  char bytes[2];
};

typedef uint16_t sample_buffer[NUM_SIGNAL_CHS];
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, incoming_comm_q);
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_DECLARE_GLOBAL(sample_buffer,  4, sample_q);

#endif /* __GLOBAL_H_GUARD */
