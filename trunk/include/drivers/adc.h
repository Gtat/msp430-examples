#ifndef __ADC_H_GUARD
#define __ADC_H_GUARD

#include "global.h"
#define ADC_CH_MASK    0xE9
#define NUM_SIGNAL_CHS (__builtin_popcount(ADC_CH_MASK))

void adc_on
  (void);

void adc_off
  (void);

#endif /* __ADC_H_GUARD */
