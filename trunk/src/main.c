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


union mcu_to_pc mcu_packet = 
  {
    .command = 
      { 
//        .id = DATA,
      },
  }; 

union pc_to_mcu  pc_packet;

/** The big ugly global data passing structure.
 */
static struct 
{
  volatile uint8_t pc_packets;
  volatile uint8_t mcu_packets;

  const uint8_t channels;
  uint16_t samples[8];
  volatile uint8_t sample_ct;
} control = 
  { 
    .channels = 6,
  };

/* declare and initialize a ring queue AKA circular buffer AKA fifo */
/* data type is char, depth is 16 elements, identifier is "stringq" */
/* declare it globally so it is shared between main and interrupt */
RING_QUEUE_CREATE      (char,                      16, incoming_comm_q);
//RING_QUEUE_CREATE      (uint16_t,                   8, sample_q);
//RING_QUEUE_ARRAY_CREATE(uint8_t, control.channels,  8, sample_q);


int main
  (void)
{
  char i;
  uint8_t ch;
  uint16_t sample;

  setup();                                     /* system setup */
  usci_setup(USCI_CHANNEL_A0, USCI_MODE_RS232, /* channel A, serial/RS232 mode */
             1000000/9600,                     /* 9600 baud using 1 MHz ref clock */
             UCA0RXIE);                        /* receive interrupts enabled */

  putchar('!');

//  timer_setup();
  adc_setup(control.channels, 1);


  while (ADC10CTL1 & BUSY);
  ADC10DTC0  = ADC10CT;
  ADC10DTC1  = control.channels;
  ADC10CTL0 |= ENC;
  ADC10SA    = (uint16_t)control.samples;
  
    while(1)
    {
      __bis_SR_register(LPM0_bits | GIE);       /* enter low power mode 0 */
                                                /* with interrupts on */
      for ( ; control.sample_ct; control.sample_ct--)
      {
        mcu_packet.command.id = i++;

        for (ch = 0; ch < control.channels; ch++)
        {
          mcu_packet.command.payload.samples[ch] = control.samples[ch] >> 2;
        }
        send_packet(&mcu_packet);
      }
    }

//  while(1)
//  {
//    while (incoming_comm_q.length >= sizeof(union pc_to_mcu))
//    {
//      RING_QUEUE_POP_MANY(incoming_comm_q, pc_cmd.bytes, sizeof(union pc_to_mcu));
//    }
//  }
  return 0;
}

#include "interrupt.c"

