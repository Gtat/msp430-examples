/**
 * @file    main.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * MSP430 interrupt-driven low-power operation skeleton/example
 *
 * This program sets up interrupts for:
 *   1) when a byte is received over the RS232/serial/USB connection 
 *      (from a terminal)
 *   2) approximately every 16 ms using the watchdog timer peripheral
 * It then enters low power mode 0.
 *
 * When a byte is received over the RS232 connection, it will blink the
 * on-board LED to indicate that the keystroke was received and will buffer
 * the received byte in a simple static queue structure (defined generically
 * in ringq.h). The bytes are switched between upper and lower case before being
 * enqueued as a very simple form of processing.
 *
 * When the timer interrupt occurs, it will check if the queue contains bytes 
 * and wake up the processor if it does.
 *
 * When the processor wakes up, it writes all bytes in the queue to the RS232
 * as output. 
 *
 * Therefore any characters typed into a terminal connected to the running 
 * program should be echoed back a short time later with their case reversed.
 */

#include <msp430.h>
#include <stdio.h>

#include "global.h"
#include "ringq.h"
#include "protocol.h"

#include "drivers/usci.h"

#include "drivers/inlines.c"


/**
 * Write 1 or 0 bytes to the terminal.
 *
 * @param c The byte to write as an integer.
 *
 * @return  The number of bytes written.
 */


union mcu_to_pc mcu_packet;
union pc_to_mcu  pc_packet;

/** The big ugly global data passing structure.
 */
struct control_t control = 
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

int main
  (void)
{
  enum pc_packet_status status;

  setup();                                     /* system setup */
  usci_setup(USCI_CHANNEL_A0, USCI_MODE_RS232, /* channel A, serial/RS232 mode */
             1000000/9600,                     /* 9600 baud using 1 MHz ref clock */
             UCA0RXIE);                        /* receive interrupts enabled */
  adc_setup(control.channels, 1);
 
  P1DIR = 0x01;
  P1OUT = 0x00;

  while (ADC10CTL1 & BUSY);
  ADC10DTC0  = 0;
  ADC10DTC1  = control.channels;
  ADC10CTL0 |= ENC;
  ADC10SA    = (uint16_t)&sample_q.data[sample_q.head];
  
  while(1)
  {
    __bis_SR_register(LPM0_bits | GIE);       /* enter low power mode 0 */
                                              /* with interrupts on */
    /* TX state machine
     * The state cannot change in response to a TXed packet. 
     */
    switch((const enum state)control.state)
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
    while (control.pc_packets--)
    {
      status = process_pc_packet(&pc_packet);
      switch(control.state)
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
    }
  }

  return 0;
}

#include "interrupt.c"

