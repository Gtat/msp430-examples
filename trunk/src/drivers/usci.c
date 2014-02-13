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

#include "drivers/usci.h"

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
      UCA0CTL1 |=  UCSSEL_2;     /* use master clock (1 MHz)            */

      UCA0BR0   =  rateDivider;  /* refer to 19.3.11 in the User Guide, */
      UCA0BR1   =  0;            /* or use the BAUDRATE macro in usci.h */
                                 /* for a rough figure                  */
      P1SEL     =  BIT1 | BIT2;  /* set port 1 bits 1 and 2 to peripheral mode*/
      P1SEL2    =  BIT1 | BIT2; 
      if (mode == USCI_MODE_RS232)
      {
        UCA0MCTL  =  UCBRS0;     /* RS232 modulation pattern, UG 19-30 */
      }
      else  /* SPI mode */
      {
        /* 3-pin, 8-bit SPI master, capture on falling edge */
        UCA0CTL0 |= UCMSB | UCMST | UCSYNC; 
        UCA0MCTL  = 0;

        /* for SPI on channel A, pin 4 functions as the clock */
        P1SEL    |= BIT4;
        P1SEL2   |= BIT4;
      }
      UCA0CTL1 &= ~UCSWRST;      /* de-assert USCI reset for channel   */
      break;
    }
    case USCI_CHANNEL_B0:
    {
      UCB0CTL1 |=  UCSSEL_2;     /* use master clock (1 MHz)            */

      UCB0BR0   =  rateDivider;  /* refer to 19.3.11 in the User Guide, */
      UCB0BR1   =  0;            /* or use the BAUDRATE macro in usci.h */
                                 /* for a rough figure                  */
      P1SEL     =  BIT6 | BIT7;  /* set port 1 bits 6 and 7 to peripheral mode*/
      P1SEL2    =  BIT6 | BIT7; 
      if (mode == USCI_MODE_SPI) /* SPI mode */
      {
        /* 3-pin, 8-bit SPI master, capture on falling edge */
        UCB0CTL0 |= UCMSB | UCMST | UCSYNC; 

        /* for SPI on channel B, pin 5 functions as the clock */
        P1SEL    |= BIT5;
        P1SEL2   |= BIT5;
      }
      UCB0CTL1 &= ~UCSWRST;      /* de-assert USCI reset for channel   */
      break;
    }
  }

  IE2      |=  mask;        /* activate specified interrupts */

  return 0;
}
