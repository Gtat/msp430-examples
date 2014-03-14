/**
 * @file    global.h
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Global definitions and configuration parameters. Ring queues are
 * declared here using a special macro so they will be visible to every
 * source file.
 */
#ifndef __GLOBAL_H_GUARD
#define __GLOBAL_H_GUARD

#define NUM_TOTAL_CHS  (8)

#include <msp430.h>
#include "ringq.h"

/** \union word
 *  Data type for prettying up bytewise accesses to an object of 
 *  native word size.
 */
union word
{
  unsigned int word;
  uint8_t      bytes[sizeof(unsigned int)];
};

/** \typdef sample_buffer
 *  The type of an array of captured ADC samples. Capable of holding a sample
 *  from every channel.
 */
typedef uint16_t sample_buffer[NUM_TOTAL_CHS];
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, incoming_comm_q);
RING_QUEUE_DECLARE_GLOBAL(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_DECLARE_GLOBAL(sample_buffer,  4, sample_q);

#endif /* __GLOBAL_H_GUARD */
