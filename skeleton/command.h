/** @file    command.h
 *  @author  Sam Boling <charles.samuel.boling@gmail.com>
 *  @version 0.1
 *
 *  Explicit definition of iEGAS command formats. 
 */
#ifndef __COMMAND_H_GUARD
#define __COMMAND_H_GUARD

#include <stdint.h>

/** \union user_command
 * The format for input commands from the user.
 */
union user_command
{
  struct 
  {
    char id;   /**< One-byte code specifying the command action. 
                *   The high-order bit is cleared for commands that
                *   set microcontroller registers and set for commands
                *   that will have their payloads sent over the SPI.
                */
    union
    {
      uint16_t as_16bit;   /**< _little-endian_ to match the microcontroller */
      uint8_t  as_8bit[2];
    } payload; /**< Value associated with the command. */
  } command;

  char raw_bytes[sizeof(command)];
};

#endif /* __COMMAND_H_GUARD */

