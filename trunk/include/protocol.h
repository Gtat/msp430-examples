/** @file    protocol.h
 *  @author  Sam Boling <charles.samuel.boling@gmail.com>
 *  @version 0.1
 *
 *  Explicit definition of iEGAS command formats. 
 */
#ifndef __PROTOCOL_H_GUARD
#define __PROTOCOL_H_GUARD

#include <stdint.h>

/** \union user_command
 * The format for input commands from the user.
 */
union pc_command
{
  struct 
  {
    char destination : 1; /**< Cleared for commands that alter microcontroller
                           *   values, set for commands that will have their
                           *   payloads sent off-chip.
                           */
    char id;          : 3; 
    char flags       : 4;

    char payload;
    char crc;
  } command;

  char raw_bytes[sizeof(command)];
};

union mcu_command
{

  struct
  {
  } command;
  
  char raw_bytes[sizeof(command)];
};

#endif /* __PROTOCOL_H_GUARD */

