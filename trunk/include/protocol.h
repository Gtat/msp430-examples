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
#include "drivers/flash.h"
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
      enum pc_id
       { 
        HELLO         = 0x0,
        DUMP          = 0x1,
        CAPTURE       = 0x2,
        HALT          = 0x3,
        SET_VOLTAGE   = 0x4,
        SET_RATES     = 0x5,
        SET_MARGIN    = 0x6,
        SET_BIAS_TIME = 0x7,
      } id : 3;
      uint8_t toggle  : 1;
      uint8_t channel : 3;
    };

    union __PACK
    {
      union dac_word dac_setting;
      uint16_t       taccr;     /* will overwrite TACCR[15:0] */
      struct
      {
        uint8_t  reserved : 1;
        uint16_t seconds  : 15;
      }
    } payload;

    uint8_t crc;
  } command;

  uint8_t bytes[sizeof(struct pc_to_mcu_format_t)];
};

/** \union mcu_command
 *  The format for messages from the microcontroller.
 */
union mcu_to_pc
{
  struct mcu_to_pc_format_t
  {
    /* identifying information about the payload
     * e.g. whether the methane readings are valid 
     */
    enum mcu_flags
    {
      AMPEROMETRY_CH_VALID = 0x1,
    } flags : 4;

    enum mcu_id
    {
      DATA     = 0x0,
      RETRY    = 0x1,
      OK       = 0x2,
      PREAMBLE = 0x3,
      STORED   = 0x4,
      ALERT    = 0xF,
    } id : 4;

    /* 6 bytes */
    union __PACK
    {
      uint8_t samples[NUM_SIGNAL_CHS]; /* for DATA packets */
 
      struct __PACK
      {
        uint8_t          flags;
        struct rate_info rates;
      } preamble;

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
      } alert;                        /* for ALERT packets */
  
      uint8_t padding;
    } payload;
    
    /* 1 byte */
    uint8_t crc;    
  } command;
  
  uint8_t bytes[sizeof(struct mcu_to_pc_format_t)];
}; 

enum pc_packet_status
{
  PC_PACKET_HELLO,
  PC_PACKET_OK,
  PC_PACKET_EMPTY,
  PC_PACKET_BAD_CRC,
  PC_PACKET_BEGIN,
  PC_PACKET_DUMP,
  PC_PACKET_HALT,
};

enum packet_options
{
  PACKET_OPT_NONE,
  PACKET_OPT_BLOCK,
};

int build_mcu_packet
  (union mcu_to_pc * const p, enum mcu_id id, ...);

unsigned int send_mcu_packet
  (const union mcu_to_pc * const p,
   enum packet_options opt);

enum pc_packet_status process_pc_packet
  (union pc_to_mcu * const p);

#ifdef  CRC_ENABLED
#define CRC8_INIT 0xFF
uint8_t crc8
  (const uint8_t * data, int len, uint8_t crc);
#endif

#ifdef CONFIG_ENABLE_STORAGE_MODE
  struct flash_record;

  struct storage_cell
  {
    enum mcu_flags flags     : 4;
    uint16_t       timestamp : 12;
    uint8_t        samples[NUM_SIGNAL_CHS];
  };
  extern const struct storage_cell stored_data[CONFIG_MAX_STORED_SAMPLES]
    __attribute__(( section(".flash_storage") ));

  size_t store_packet
    (struct flash_record *r, union mcu_to_pc *p, enum mcu_flags flags);
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

#endif /* __PROTOCOL_H_GUARD */

