/**
 * @file    main.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 */

#include <msp430.h>
#include <stdio.h>

#include "global.h"
#include "ringq.h"
#include "protocol.h"

#include "drivers/usci.h"
#include "drivers/parameter.h"

#include "drivers/inlines.c"

static struct control_t
{
  enum state
  {
    STATE_IDLE,
    STATE_SETUP,
    STATE_STREAM,
  } state;

  volatile uint8_t pc_packets;

  const    uint16_t channels;
} control = 
  { 
    .state    = STATE_IDLE,
    .channels = NUM_SIGNAL_CHS,
  };

/* declare and initialize a ring queue AKA circular buffer AKA fifo */
/* data type is char, depth is 16 elements, identifier is "stringq" */
/* declare it globally so it is shared between main and interrupt */
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, incoming_comm_q);
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_CREATE_PREDEFINED(sample_buffer,  4, sample_q);
union mcu_to_pc mcu_packet;
union pc_to_mcu  pc_packet;

int main
  (void)
{
  enum pc_packet_status status;

  setup();                                     /* system setup */

  adc_setup(NUM_SIGNAL_CHS);

  set_voltage(DEFAULT_DAC_WORD);
  usci_set_mode(USCI_MODE_RS232);

  while(1)
  {
    __bis_SR_register(LPM0_bits | GIE);       /* enter low power mode 0 */
                                              /* with interrupts on */
    /* TX state machine
     * The state cannot change in response to a TXed packet. 
     */
    switch ((const enum state)control.state)
    {
      case STATE_STREAM:
      {
        while (!RING_QUEUE_EMPTY(sample_q))
        {
          build_mcu_packet(&mcu_packet, DATA, control.channels);
          send_mcu_packet(&mcu_packet);
        }
        break;
      }
      default:
      {
        break;
      }
    }

    /* RX state machine */
    for ( ; control.pc_packets > 0; --control.pc_packets)
    {
      status = process_pc_packet(&pc_packet);
      switch (status)
      {
        case PC_PACKET_BEGIN:
        {
          control.state = STATE_STREAM;
          break;
        }
        case PC_PACKET_HALT:
        {
          control.state = STATE_IDLE;
          break;
        }
        default: 
        {
          control.state = STATE_IDLE;
          break;
        }
      }
#if 0
      switch (control.state)
      {
        case STATE_IDLE:
        {
          if (status == PC_PACKET_BEGIN)
          {
            control.state = STATE_STREAM;
          }
          break;
        }
        case STATE_STREAM:
        {
          if (status == PC_PACKET_HALT)
          {
            control.state = STATE_IDLE;
          }
          break;
        }
        default:
        {
          break;
        }
      }
#endif
    }
  }

  return 0;
}

#include "interrupt.c"

