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
 * SPI setup is not yet implemented.
 */

#include "usci.h"

/**
 * Set up the universal serial communication interface (USCI) as
 * an RS232 or SPI link.
 *
 * @param channel     Either USCI_CHANNEL_A0 or USCI_CHANNEL_B0.
 * @param mode        Either USCI_MODE_RS232 or USCI_MODE_SPI.
 * @param rateDivider The division factor against the master clock.
 * @param mask        The bits to set in interrupt register IE2.
 *
 * @return        Zero on success.
 */
int usci_setup
  (enum usci_channel channel, enum usci_mode mode, 
   int rateDivider, int mask)
{

  switch (channel)
  {
    case USCI_CHANNEL_A0:
    {
      UCA0CTL1 |=  UCSSEL_2;    /* use master clock (1 MHz)            */

      UCA0BR0   =  rateDivider; /* refer to 19.3.11 in the User Guide, */
      UCA0BR1   =  0;           /* or use the BAUDRATE macro in usci.h */
                                /* for a rough figure                  */
      if (mode == USCI_MODE_RS232)
      {
        UCA0MCTL  =  UCBRS0;    /* RS232 modulation pattern, UG 19-30 */
      }
      UCA0CTL1 &= ~UCSWRST;     /* de-assert USCI reset for channel   */
      break;
    }
    case USCI_CHANNEL_B0:
    {
      UCB0CTL1 |=  UCSSEL_2;    /* use master clock (1 MHz)            */

      UCB0BR0   =  rateDivider; /* refer to 19.3.11 in the User Guide, */
      UCB0BR1   =  0;           /* or use the BAUDRATE macro in usci.h */
                                /* for a rough figure                  */
      UCB0CTL1 &= ~UCSWRST;     /* de-assert USCI reset for channel   */
      break;
    }
  }

  P1SEL     =  BIT1 | BIT2; /* set port 1 bits 1 and 2 to peripheral mode*/
  P1SEL2    =  BIT1 | BIT2; 

  IE2      |=  mask;        /* activate specified interrupts */

  return 0;
}

