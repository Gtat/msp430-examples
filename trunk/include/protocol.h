/** @file    protocol.h
 *  @author  Sam Boling <charles.samuel.boling@gmail.com>
 *  @version 0.1
 *
 *  Explicit definition of iEGAS command formats. Note that bitfields are listed
 *  in reverse order from the spec -- this is how the toolchain chooses to 
 *  interpret their ordering. The iEGAS System Operating Specifications document
 *  is definitive and uses unambiguous Verilog-style bit formatting.
 */
#ifndef __PROTOCOL_H_GUARD
#define __PROTOCOL_H_GUARD

#include <stdint.h>
#include "global.h"
#include "drivers/adc.h"
#include "drivers/parameter.h"

#define __PACK __attribute__((packed))
#define CRC_ENABLED

/** \union pc_command
 * The format for input commands from the user.
 */
union __PACK pc_to_mcu
{
  struct __PACK pc_to_mcu_format_t
  {
    /* header byte -- little-endian so leftmost bit "dest" is LSB */
    struct __PACK
    {
      unsigned int flags : 4; 
      /* 3-bit IDs */
      enum pc_id
      { 
        HELLO       = 0x0,
        DUMP        = 0x1,
        CAPTURE     = 0x2,
        HALT        = 0x3,
        SET_VOLTAGE = 0x4,
        SET_RATES   = 0x5,
        SET_MARGIN  = 0x6,
      } id : 4; 
    };

    union __PACK
    {
      union dac_word dac_setting;
      uint16_t       taccr;     /* will overwrite TACCR[15:0] */
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
    /* 4 bits */
    enum mcu_id
    {
      DATA  = 0x0,
      RETRY = 0x1,
      OK    = 0x2,
      ALERT = 0xF,
    } id : 4;

    /* identifying information about the payload
     * e.g. whether the methane readings are valid 
     */
    enum mcu_flags
    {
      AMPEROMETRY_CH_VALID = 0x1,
    } flags : 4;

    /* 6 bytes */
    union __PACK
    {
      uint8_t samples[NUM_SIGNAL_CHS];

      struct __PACK
      {
        char channel : 3;

        enum __PACK alert_type
        {
          BATTERY   = 0x0,
          EXPLOSIVE = 0x1,
          TOXIC     = 0x2,
        } type : 3;

        enum __PACK alert_severity
        {
          NOTICE   = 0x0,
          WARNING  = 0x1,
          DANGER   = 0x2,
          CRITICAL = 0x3,
        } severity : 2;
        
        uint16_t timestamp;
        uint8_t  data[3];
      } alert;
  
      uint8_t empty;
    } payload;
    
    /* 1 byte */
    uint8_t crc;    
  } command;
  
  uint8_t bytes[sizeof(struct mcu_to_pc_format_t)];
}; 

uint8_t build_mcu_packet
  (union mcu_to_pc * const p, enum mcu_id id, ...);

unsigned int send_mcu_packet
  (const union mcu_to_pc * const p);

enum pc_packet_status
{
  PC_PACKET_OK,
  PC_PACKET_EMPTY,
  PC_PACKET_BAD_CRC,
  PC_PACKET_BEGIN,
  PC_PACKET_HALT,
};

enum pc_packet_status process_pc_packet
  (union pc_to_mcu * const p);

#ifdef  CRC_ENABLED
#define CRC8_INIT 0xFF
uint8_t crc8
  (const uint8_t * data, int len, uint8_t crc);
#endif

#endif /* __PROTOCOL_H_GUARD */

