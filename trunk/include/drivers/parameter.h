#ifndef __DRIVERS_PARAMETER_H_GUARD
#define __DRIVERS_PARAMETER_H_GUARD

#include <msp430.h>
#include "protocol.h"

#define DEFAULT_DAC_WORD ((union word){.bytes = {0x12, 0x34}})

void update_rates
  (char flags, uint16_t taccr);

void set_voltage
  (union word dac_word);

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

