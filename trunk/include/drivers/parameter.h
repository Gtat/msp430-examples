#ifndef __DRIVERS_PARAMETER_H_GUARD
#define __DRIVERS_PARAMETER_H_GUARD

#include <stdint.h>
#include "global.h"
#include "processing.h"


#define NUM_DAC_CHS 4

/** \union dac_word
 *  Configuration format for the TI TLV5620 Quadruple DAC.
 */
union __attribute__((packed)) dac_word
  
{
  struct __attribute__((packed)) dac_format_t
  {
    uint8_t range   : 1;
    uint8_t channel : 2;
    char            : 5;
    uint8_t data;
  } formatted;
  uint8_t bytes[sizeof(uint16_t)];
};

struct rateinfo
{
  uint16_t taccr;
};

extern struct parameter_t
{
  union  dac_word voltages[NUM_DAC_CHS];
  struct rateinfo rate;
  struct processor process;
  struct processor alarm;
} parameters;

void update_rates
  (char flags, uint16_t taccr);

void set_voltage
  (union dac_word setting);

void set_all_voltages
  (void);

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

