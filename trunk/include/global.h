#ifndef __GLOBAL_H_GUARD
#define __GLOBAL_H_GUARD

#define NUM_SIGNAL_CHS 6

#include "ringq.h"

typedef uint16_t sample_buffer[NUM_SIGNAL_CHS];
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, incoming_comm_q);
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_DECLARE_GLOBAL(sample_buffer,  4, sample_q);

extern struct control_t
{
  enum state
  {
    STATE_IDLE,
    STATE_SETUP,
    STATE_STREAM,
  } state;

  volatile uint8_t pc_packets;
  volatile uint8_t mcu_packets;

  const    uint8_t channels;
} control;

#endif /* __GLOBAL_H_GUARD */
