#ifndef __DRIVERS_PARAMETER_H_GUARD
#define __DRIVERS_PARAMETER_H_GUARD

#include <msp430.h>
#include "protocol.h"

/** 
 *  Configuration format for the TI TLV5620 Quadruple DAC.
 */
struct dac_word
{
  unsigned char range   : 1;
  char                  : 5; /* not used */

  uint8_t       data;
} command;

struct rateinfo
{
  uint16_t taccr;
};

extern struct parameter_t
{
  struct dac_word voltages[NUM_SIGNAL_CHS];
  struct rateinfo rate;
  uint8_t (*process)
    (uint16_t sample, void * option);
  void * processing_option;
} parameters;

void update_rates
  (char flags, uint16_t taccr);

void set_voltage
  (uint8_t ch, struct dac_word setting);

void set_all_voltages
  (void);

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

