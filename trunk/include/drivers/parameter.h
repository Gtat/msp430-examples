#ifndef __DRIVERS_PARAMETER_H_GUARD
#define __DRIVERS_PARAMETER_H_GUARD

#include <msp430.h>
#include "protocol.h"

/** \union dac_word
 *  Configuration format for the TI TLV5620 Quadruple DAC.
 */
union dac_word
{
  struct dac_format_t
  {
    unsigned char channel : 2;
    unsigned char range   : 1;
    char                  : 5; /* not used */

    uint8_t       data;
  } command;

  char bytes[sizeof(struct dac_format_t)];
};

#define DEFAULT_DAC_WORD \
  ((union dac_word){     \
    .command = {         \
      .channel = 2,      \
      .range = 0,        \
      .data  = 0xA5,     \
    },                   \
  })

void update_rates
  (char flags, uint16_t taccr);

void set_voltage
  (union dac_word setting);

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

