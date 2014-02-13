/**
 * @file    usci.h
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * Macros, types and function prototypes for the Universal Serial 
 * Communication Interface (USCI).
 */
#ifndef __USCI_H_GUARD
#define __USCI_H_GUARD

#include <msp430.h>

#define BAUDRATE(clk, baud) ((clk) / (baud))

enum usci_channel
{
  USCI_CHANNEL_A0,
  USCI_CHANNEL_B0,
};

enum usci_mode
{
  USCI_MODE_RS232,
  USCI_MODE_SPI,
};

int usci_setup
  (enum usci_channel channel, enum usci_mode mode, 
   int rateDivider, int mask);

#endif /* __USCI_H_GUARD */

