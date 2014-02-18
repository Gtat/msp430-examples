/** @file    protocol.h
 *  @author  Sam Boling <charles.samuel.boling@gmail.com>
 *  @version 0.1
 *
 *  Explicit definition of iEGAS command formats. 
 *  Note that bitfields are listed in reverse 
 *  order from the spec -- this is how the 
 *  toolchain chooses to interpret their ordering.
 *  The iEGAS System Operating Specifications is
 *  definitive and uses unambiguous Verilog-style
 *  bit formatting.
 */
#ifndef __PROTOCOL_H_GUARD
#define __PROTOCOL_H_GUARD

#include <stdint.h>
#include "global.h"

#define __PACK __attribute__((packed))
#define CRC_ENABLED

/** \union pc_command
 * The format for input commands from the user.
 */
union pc_to_mcu
{
  struct __PACK pc_to_mcu_format_t
  {
    char         : 4; /* always zero / unimplemented */

    /* 3-bit IDs */
    enum pc_id
    { DUMP        = 0x0,
      CAPTURE     = 0x1,
      HALT        = 0x2,
      SET_VOLTAGE = 0x5,
      SET_RATES   = 0x6,
      SET_MARGIN  = 0x7,
    } id         : 3; 

    char dest    : 1; /* 0 for setting MCU parameters */
                      /* 1 for bits [11:0] to go directly */
                      /* out the SPI */

    union
    {
      uint8_t voltage[2];
      struct
      {
        char            : 2; /* always zero / unimplemented */
        enum storage_rate
        {
          NONE,
          SOME, 
          ALL,
        } storage       : 2;
        char            : 2; /* always zero / unimplemented */
        enum
        {
          SLOW,
          MED,
          FAST,
        } report        : 2;
        uint8_t adcctl1;     /* will overwrite ADC10CTL1[7:3] */
      } rates;
    } payload;

   uint8_t crc;
  } command;

  uint8_t bytes[sizeof(struct pc_to_mcu_format_t)];
} __attribute__((packed));

/** \union mcu_command
 *  The format for messages from the microcontroller.
 */
union mcu_to_pc
{
  struct mcu_to_pc_format_t
  {
    /* 1 byte */
    enum mcu_id
    {
      DATA  = 0x00,
      RETRY = 0x01,
      OK    = 0x02,
      ALERT = 0xFF,
    } id;

    /* 6 bytes */
    union __PACK
    {
      uint8_t samples[NUM_SIGNAL_CHS];

      struct __PACK
      {
        char channel  : 3;

        enum __PACK alert_type
        {
          BATTERY   = 0x0,
          EXPLOSIVE = 0x1,
          TOXIC     = 0x2,
        } type        : 3;

        enum __PACK alert_severity
        {
          NOTICE   = 0x0,
          WARNING  = 0x1,
          DANGER   = 0x2,
          CRITICAL = 0x3,
        } severity    : 2;
        
        uint16_t timestamp;
        uint8_t  data[3];
      } alert;
  
      char empty[0];
    } payload;
    
    /* 1 byte */
    uint8_t crc;    
  } command;
  
  uint8_t bytes[sizeof(struct mcu_to_pc_format_t)];
}; 

int build_mcu_packet
  (union mcu_to_pc *p, enum mcu_id id, ...);

int send_mcu_packet
  (union mcu_to_pc *p);

#ifdef  CRC_ENABLED
#define CRC8_INIT 0xFF
uint8_t crc8
  (uint8_t *data, int len, uint8_t crc);
#endif

#endif /* __PROTOCOL_H_GUARD */

