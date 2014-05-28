/**
 * @file    usci.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Setup for TI's Universal Serial Communication Interface.
 *
 * The USCI is a subsystem on the microcontroller for managing several
 * kinds of serial interfaces. The ones of interest to the iEGAS are
 * the SPI bus and the UART/serial/RS232 protocol that communicates 
 * over the USB. This file contains some management code for both of
 * them.
 *
 */

#include "drivers/usci.h"
#include "drivers/adc.h"
#include "ringq.h"

/**
 * Reconfigure the USCI module for use as a 3-wire SPI master,
 * an RS232 UART, or inactive (held in reset).
 *
 * @param mode  USCI_MODE_SPI, USCI_MODE_RS232, or USCI_MODE_OFF.
 */
void usci_set_mode
  (enum usci_mode mode)
{
  P1SEL   = BIT1 | BIT2 | BIT4;
  P1SEL2  = BIT1 | BIT2 | BIT4;
  /* enable line for DAC, active high */
  P2DIR   = 0x01;
  P2OUT   = 0x00;

  usci_block_tx();
  UCA0CTL1 |= UCSWRST;
  UCA0CTL1 |= UCSSEL_2;

  switch (mode)
  {
    case USCI_MODE_SPI:
    {
      UCA0CTL0 |= UCMSB | UCMST | UCSYNC; 
      UCA0BR0   = SPI_BAUDRATE_REGVAL;
      UCA0BR1   = 0;
      UCA0MCTL  = 0;  
      break;
    }
    case USCI_MODE_RS232:
    {
      UCA0BR0   =  UART_BAUDRATE_REGVAL;
      UCA0CTL0 &= ~(UCMSB | UCMST | UCSYNC); 
      UCA0MCTL  =  UCBRS2 | UCBRS0;     /* RS232 modulation pattern, UG 19-30 */
      break;
    }
    default:
    {
      P1SEL  &= ~(BIT1 | BIT2 | BIT4); 
      P1SEL2 &= ~(BIT1 | BIT2 | BIT4); 
      adc_on();
      return; /* don't turn interrupts back on, hold in reset */
    }
  }
  UCA0CTL1 &= ~UCSWRST;

  UC0IE  |= UCA0RXIE;
}

/**
 * Add a byte to the outgoing data queue. No data will be written until 
 * usci_commit() is called.
 *
 * @param c  The byte to write out.
 */
void usci_write
  (uint8_t c)
{
  RING_QUEUE_PUSH(outgoing_comm_q, c);
}

/**
 * Begin transmitting all enqueued bytes to the USCI using the currently
 * selected mode.
 */
__attribute__((always_inline)) inline void usci_commit
  (void)
{
  UC0IE |= UCA0TXIE;
}

void usci_commit_blocking
  (void)
{
  UC0IE |= UCA0TXIE;
  while (UC0IE & UCA0TXIE);
}

__attribute__((always_inline)) inline void usci_block_tx
  (void)
{
  while(!(UC0IFG & UCA0TXIFG));
}

__attribute__((always_inline)) inline void usci_block_rx
  (void)
{
  while(!(UC0IFG & UCA0RXIFG));
}

