/**
 * @file    main.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Simple MSP430 examples to send a "hello" message to the PC.
 */

#include <stdio.h>

#include "global.h"
#include "ringq.h"

#include "protocol.h"
#include "drivers/usci.h"

#include "drivers/inlines.c"

struct control_t control =
  {
    .interval = 3, /* send a msg every 3 seconds */
  };

/* declare and initialize a ring queue AKA circular buffer AKA fifo */
/* data type is char, depth is 16 elements, identifier is "stringq" */
/* declare it globally so it is shared between main and interrupt */
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, incoming_comm_q);
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_CREATE_PREDEFINED(enum_event,     4, event_q);
RING_QUEUE_CREATE_PREDEFINED(sample_buffer,  4, sample_q);

char hello_message[] = "hello\n";

int main
  (void)
{
  uint8_t i;

  setup();                         /* system setup */
  usci_set_mode(USCI_MODE_RS232);

  P1DIR = 0x01;
  P1OUT = 0x00;

  __bis_SR_register(GIE);
  while(1);
  /*
  while(1)
  {
    while (!RING_QUEUE_EMPTY(event_q))
    {
      RING_QUEUE_POP(event_q);
      for (i=0; i < sizeof(hello_message); ++i)
      {
        usci_write(hello_message[i]);
      }
      usci_commit();
    }
    __bis_SR_register(LPM0_bits | GIE);
  }
  */

  return 0;
}

#include "interrupt.c"
