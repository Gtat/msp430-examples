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
#include "usci.h"



/**
 * General purpose setup to get the board running.
 */
void setup
  (void)
{
  WDTCTL = WDTPW | WDTHOLD; /* stop watchdog */
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);               /* trap CPU if clock is not calibrated */
  } 
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}

/**
 * Set up the watchdog as an interval timer interrupt.
 */
void timer_setup
  (void)
{
  WDTCTL  =  WDT_ADLY_16; /* interrupt every ~16 ms */
  IE1    &= ~NMIIE;
  IE1    |=  WDTIE;
}

/**
 * Write 1 or 0 bytes to the terminal.
 *
 * @param c The byte to write as an integer.
 *
 * @return  The number of bytes written.
 */
int putchar
  (int c)
{
  if (c)
  {
    while(UCA0STAT & UCBUSY);  /* wait until USCI channel A is not busy */
    UCA0TXBUF = c;             /* write given character to output register */
    return 1;                  /* wrote one byte */
  } 
  else
  {
    return 0;                  /* don't write \0, the null terminator */
  }
}

/* declare and initialize a ring queue AKA circular buffer AKA fifo */
/* data type is char, depth is 16 elements, identifier is "stringq" */
/* declare it globally so it is shared between main and interrupt */
RING_QUEUE_CREATE(char, 16, stringq);

int main
  (void)
{
  setup();                                     /* system setup */
  usci_setup(USCI_CHANNEL_A0, USCI_MODE_RS232, /* channel A, serial/RS232 mode */
             1000000/9600,                     /* 9600 baud using 1 MHz ref clock */
             UCA0RXIE);                        /* receive interrupts enabled */
                    
  timer_setup();

  P1DIR |= 0x01;                               /* set LED pin as output */

  while (1)
  {
    __bis_SR_register(LPM0_bits | GIE);       /* enter low power mode 0 */
                                              /* with interrupts on */
  
    /* flush the buffer */
    while (!RING_QUEUE_EMPTY(stringq))
    {  
      putchar(RING_QUEUE_POP(stringq));
    }
  }

  return 0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_rx_isr
  (void)
{
  /* at each received keystroke: */
  P1OUT ^= 0x01;                              /* blink the LED */
  RING_QUEUE_PUSH(stringq, UCA0RXBUF ^ 0x20); /* change upper <-> lower case */
                                              /* of received character and   */
                                              /* put it in a fifo            */
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_isr
  (void)
{
  if (!RING_QUEUE_EMPTY(stringq))
  {
    __bic_SR_register_on_exit(LPM0_bits);     /* exit low power mode 0 */
  } 
}

