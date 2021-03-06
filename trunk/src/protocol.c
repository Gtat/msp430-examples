/**
 * @file    protocol.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Protocol-level routines for communicating with the PC.
 */
#include <stdarg.h>

#include "protocol.h"
#include "processing.h"
#include "drivers/adc.h"
#include "drivers/flash.h"
#include "drivers/parameter.h"
#include "drivers/usci.h"

#ifdef CONFIG_ENABLE_STORAGE_MODE
const struct storage_cell stored_data[CONFIG_MAX_STORED_SAMPLES];
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

/**
 * Construct a packet to send to the PC of the given type in-place at the
 * given address.
 *
 * @param p   where the packet is located
 * @param id  the type/ID of the packet
 * @param ... additional parameters specific to the packet type
 */
int build_mcu_packet
  (union mcu_to_pc * const p, enum mcu_id id, ...)
{
  va_list ap;
  unsigned int index;
  uint8_t ret;

  p->command.id = id;
  ret = 0;

  va_start(ap, id);
  switch (id)
  {
    case DATA:
    {
      unsigned int ch;
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
      uint16_t t;

      /* TI GCC generates illegal instruction for uint16_t here */
      t = va_arg(ap, unsigned int);
      if ((t >= parameters.amperometry.start_recording)
           &&
          (t <= parameters.amperometry.hi_seconds))
      {
        p->command.flags = AMPEROMETRY_CH_VALID;
      }
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */

      for (ch = index = 0;
           (index < NUM_TOTAL_CHS) && (ch < NUM_SIGNAL_CHS);
           ++index)
      {
        /* if this channel is among the bits marked as in-use */
        if ((0x80 >> index) & ADC_CH_MASK)
        {
          /* populate the next byte of the outgoing DATA packet. */
          /* first perform the currently configured processing, or */
          /* truncate to 8 bits by default. */
          p->command.payload.samples[ch] =
            (parameters.process.execute ? : &execute_truncate_sample)
              (sample_q.data[sample_q.tail][index], /* the channel's sample */
               ch,
               &parameters.process);                /* optional argument */
          ret |=
            (parameters.alarm.execute ? : &execute_nothing)
              (sample_q.data[sample_q.tail][index],
               ch,
               &parameters.alarm);

          ++ch;
        }
      }
      RING_QUEUE_POP_NO_DATA(sample_q);

      break;
    }
#ifdef CONFIG_ENABLE_STORAGE_MODE
    case PREAMBLE:
    {
      p->command.payload.preamble.flags = parameters.flags;
      p->command.payload.preamble.rates = parameters.rates;
      break;
    }
    case STORED:
    {
      struct flash_record *r;

      r = va_arg(ap, struct flash_record *);
      /* overwrite the ID field to use all the packet space we can.
       * stored flags are now the high order bits, and it should be
       * guaranteed that these are never equal to the ID code for the
       * OK packet, which signals the end of the dump.
       */
      ret = flash_record_destructive_read
              (r,
               (uint8_t * const)&p->bytes,
               sizeof(union mcu_to_pc)-1);
      break;
    }
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
    case OK:
    {
      p->command.payload.preamble.flags = ADC_CH_MASK;
      break;
    }
    case ALERT:
    {
      break;
    }
    case RETRY:
    {
      char * ptr = va_arg(ap, char *);

      for (index = 0; index < sizeof(union pc_to_mcu); ++index)
      {
        p->command.payload.samples[index] = ptr[index];
      }
      for ( ; index < sizeof(p->command.payload.samples); ++index)
      {
        p->command.payload.samples[index] = 0;
      }
      break;
    }
    default:
    {
      break;
    }
  }
  va_end(ap);

#ifdef CRC_ENABLED
  p->command.crc = crc8(p->bytes,
                        sizeof(union mcu_to_pc)-1, /* don't CRC the CRC */
                        CRC8_INIT);
#endif
  return ret;
}

/**
 * Transmit a given packet to the USCI. The mode should be previously selected
 * as USCI_MODE_RS232 for this to behave as expected.
 *
 * @param p  address of the packet to be sent
 * @returns  the number of bytes committed
 */
unsigned int send_mcu_packet
  (const union mcu_to_pc * const p,
   enum packet_options opt)
{
  unsigned int i;
  for (i = 0; i < sizeof(union mcu_to_pc); ++i)
  {
    usci_write(p->bytes[i]);
  }
  if (opt == PACKET_OPT_BLOCK)
  {
    usci_commit_blocking();
  }
  else
  {
    usci_commit();
  }

  return i;
}

/**
 * Parse a receieved packet from the PC and use its contents to set new
 * operation parameters for the system.
 */
enum pc_packet_status process_pc_packet
  (union pc_to_mcu * const p)
{
  RING_QUEUE_POP_MANY(incoming_comm_q, p->bytes, sizeof(union pc_to_mcu));
  if (p->command.crc != crc8(p->bytes,
                             sizeof(union pc_to_mcu)-1, /* don't CRC the CRC */
                             CRC8_INIT))
  {
    return PC_PACKET_BAD_CRC;
  }

  switch (p->command.id)
  {
    case HELLO:
    {
      return PC_PACKET_HELLO;
    }
    case DUMP:
    {
      return PC_PACKET_DUMP;
    }
    case CAPTURE:
    {
      return PC_PACKET_BEGIN;
    }
    case HALT:
    {
      return PC_PACKET_HALT;
    }
#ifdef CONFIG_ENABLE_DAC_BIASING
    case SET_VOLTAGE:
    {
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
      if (p->command.toggle)
      {
        CONFIGURATION.amperometry.hi_volts = p->command.payload.dac_setting;
      }
      else
      {
        CONFIGURATION.voltages[p->command.payload.dac_setting.formatted.channel] =
          p->command.payload.dac_setting;
        CONFIGURATION.amperometry.lo_volts = p->command.payload.dac_setting;
      }
#else
      CONFIGURATION.voltages[p->command.payload.dac_setting.formatted.channel] =
        p->command.payload.dac_setting;
#endif
      break;
    }
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */
    case SET_RATES:
    {
      update_rates(RATE_FLAGS_ADC, p->command.payload.taccr);
      break;
    }
    case SET_MARGIN:
    {
      break;
    }
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
    case SET_BIAS_TIME:
    {
      if (p->command.toggle)
      {
        CONFIGURATION.amperometry.hi_seconds = p->command.payload.seconds;
      }
      else
      {
        CONFIGURATION.amperometry.lo_seconds = p->command.payload.seconds;
      }
      break;
    }
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
    default:
    {
      return PC_PACKET_EMPTY;
    }
  }

  return PC_PACKET_OK;
}

#ifdef CRC_ENABLED
/**
 *  Automatically generated by the crcmod Python module to generate a 8-bit
 *  cyclic redundancy check (CRC). Polynomial = 0x107, bit reverse algorithm.
 *
 *  @param data  The buffer to compute a CRC digest for.
 *  @param len   The length of the input buffer.
 *  @param crc   The input CRC value, typically use 0xFF
 */
uint8_t crc8
  (const uint8_t * data, int len, uint8_t crc)
{
  static const uint8_t table[256] =
  {
    0x00U,0x91U,0xE3U,0x72U,0x07U,0x96U,0xE4U,0x75U,
    0x0EU,0x9FU,0xEDU,0x7CU,0x09U,0x98U,0xEAU,0x7BU,
    0x1CU,0x8DU,0xFFU,0x6EU,0x1BU,0x8AU,0xF8U,0x69U,
    0x12U,0x83U,0xF1U,0x60U,0x15U,0x84U,0xF6U,0x67U,
    0x38U,0xA9U,0xDBU,0x4AU,0x3FU,0xAEU,0xDCU,0x4DU,
    0x36U,0xA7U,0xD5U,0x44U,0x31U,0xA0U,0xD2U,0x43U,
    0x24U,0xB5U,0xC7U,0x56U,0x23U,0xB2U,0xC0U,0x51U,
    0x2AU,0xBBU,0xC9U,0x58U,0x2DU,0xBCU,0xCEU,0x5FU,
    0x70U,0xE1U,0x93U,0x02U,0x77U,0xE6U,0x94U,0x05U,
    0x7EU,0xEFU,0x9DU,0x0CU,0x79U,0xE8U,0x9AU,0x0BU,
    0x6CU,0xFDU,0x8FU,0x1EU,0x6BU,0xFAU,0x88U,0x19U,
    0x62U,0xF3U,0x81U,0x10U,0x65U,0xF4U,0x86U,0x17U,
    0x48U,0xD9U,0xABU,0x3AU,0x4FU,0xDEU,0xACU,0x3DU,
    0x46U,0xD7U,0xA5U,0x34U,0x41U,0xD0U,0xA2U,0x33U,
    0x54U,0xC5U,0xB7U,0x26U,0x53U,0xC2U,0xB0U,0x21U,
    0x5AU,0xCBU,0xB9U,0x28U,0x5DU,0xCCU,0xBEU,0x2FU,
    0xE0U,0x71U,0x03U,0x92U,0xE7U,0x76U,0x04U,0x95U,
    0xEEU,0x7FU,0x0DU,0x9CU,0xE9U,0x78U,0x0AU,0x9BU,
    0xFCU,0x6DU,0x1FU,0x8EU,0xFBU,0x6AU,0x18U,0x89U,
    0xF2U,0x63U,0x11U,0x80U,0xF5U,0x64U,0x16U,0x87U,
    0xD8U,0x49U,0x3BU,0xAAU,0xDFU,0x4EU,0x3CU,0xADU,
    0xD6U,0x47U,0x35U,0xA4U,0xD1U,0x40U,0x32U,0xA3U,
    0xC4U,0x55U,0x27U,0xB6U,0xC3U,0x52U,0x20U,0xB1U,
    0xCAU,0x5BU,0x29U,0xB8U,0xCDU,0x5CU,0x2EU,0xBFU,
    0x90U,0x01U,0x73U,0xE2U,0x97U,0x06U,0x74U,0xE5U,
    0x9EU,0x0FU,0x7DU,0xECU,0x99U,0x08U,0x7AU,0xEBU,
    0x8CU,0x1DU,0x6FU,0xFEU,0x8BU,0x1AU,0x68U,0xF9U,
    0x82U,0x13U,0x61U,0xF0U,0x85U,0x14U,0x66U,0xF7U,
    0xA8U,0x39U,0x4BU,0xDAU,0xAFU,0x3EU,0x4CU,0xDDU,
    0xA6U,0x37U,0x45U,0xD4U,0xA1U,0x30U,0x42U,0xD3U,
    0xB4U,0x25U,0x57U,0xC6U,0xB3U,0x22U,0x50U,0xC1U,
    0xBAU,0x2BU,0x59U,0xC8U,0xBDU,0x2CU,0x5EU,0xCFU,
  };

  while (len > 0)
  {
    crc = table[*data ^ (uint8_t)crc];
    data++;
    len--;
  }
  return crc;
}
#endif

#ifdef CONFIG_ENABLE_STORAGE_MODE

size_t store_packet
  (struct flash_record *r, union mcu_to_pc *p, enum mcu_flags flags)
{
  static uint16_t timestamp = 0;
  uint16_t tag;
  size_t   ret;

  tag  = (timestamp & ((1 << 12)-1)); /* mask down to LO 12 bits */
  tag |= flags;
  timestamp++;

  ret  = flash_record_append(r, (uint8_t * const)&tag, sizeof(tag));
  flash_record_append
           (r,
            (uint8_t * const)&p->command.payload.samples,
            sizeof(p->command.payload.samples));
  return ret;
}

#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
