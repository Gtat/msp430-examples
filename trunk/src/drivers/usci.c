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

void usci_set_mode
  (enum usci_mode mode)
{
  UC0IE &= ~(UCA0TXIE | UCA0RXIE);
  adc_off();
  switch (mode)
  {
    case USCI_MODE_SPI:
    {
      UCA0BR0   = SPI_BAUDRATE_REGVAL;
      UCA0CTL0 |= UCMSB | UCMST | UCSYNC; 
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
      P1SEL  &= ~(BIT1 | BIT2); 
      P1SEL2 &= ~(BIT1 | BIT2); 

      adc_on();
      return; /* don't turn interrupts back on */
    }
  }
  P1SEL  |= BIT1 | BIT2 | BIT4;
  P1SEL2 |= BIT1 | BIT2 | BIT4;
  UC0IE  |= UCA0RXIE;
}

void usci_write
  (uint8_t c)
{
  RING_QUEUE_PUSH(outgoing_comm_q, c);
}

__attribute__((always_inline)) inline void usci_commit
  (void)
{
  UC0IE |= UCA0TXIE;
}

