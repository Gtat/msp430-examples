/**
 * @file    usci.h
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * Macros, types and function prototypes for the Universal Serial 
 * Communication Interface (USCI).
 */
#ifndef __DRIVERS_USCI_H_GUARD
#define __DRIVERS_USCI_H_GUARD

#include "global.h"

#define UART_BAUDRATE        9600
#define UART_BAUDRATE_REGVAL (1000000 / UART_BAUDRATE)
#define SPI_BAUDRATE_REGVAL  0x06

enum usci_mode
{
  USCI_MODE_RS232,
  USCI_MODE_SPI,
  USCI_MODE_OFF,
};

void usci_set_mode
  (enum usci_mode mode);

void usci_write
  (uint8_t c);

inline void usci_commit
  (void);
void usci_commit_blocking
  (void);
inline void usci_block_tx
  (void);
inline void usci_block_rx
  (void);

#endif /* __DRIVERS_USCI_H_GUARD */

