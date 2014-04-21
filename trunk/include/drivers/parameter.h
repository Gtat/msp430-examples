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
  uint8_t bytes[sizeof(struct dac_format_t)];
};

struct rateinfo
{
  uint16_t taccr;
};
enum rate_flags
{
  RATE_FLAGS_ADC,
  RATE_FLAGS_STIMULUS,
};

extern struct parameter_t
{
  union  dac_word voltages[NUM_DAC_CHS];
  struct rateinfo rate;
  struct processor process;
  struct processor alarm;
} parameters;

struct amperometry_config
{
  union dac_word hi_volts;
  union dac_word lo_volts;
  uint8_t hi_seconds;
  uint8_t lo_seconds;
};

void update_rates
  (enum rate_flags flags, uint16_t taccr);

void set_voltage
  (union dac_word setting);

void set_all_voltages
  (void);

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

