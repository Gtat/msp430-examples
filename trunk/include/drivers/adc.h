#ifndef __ADC_H_GUARD
#define __ADC_H_GUARD

#include "global.h"
#include "config/channels.h"

#define ADC_CH_MASK   ((CH0_GAS_1 << 0)	\
                     | (CH3_GAS_4 << 3) \
                     | (CH5_GAS_3 << 5) \
                     | (CH6_TEMP  << 6) \
                     | (CH7_HUMID << 7))

#define NUM_SIGNAL_CHS (__builtin_popcount(ADC_CH_MASK))

void adc_on
  (void);

void adc_off
  (void);

#endif /* __ADC_H_GUARD */
