#ifndef __ADC_H_GUARD
#define __ADC_H_GUARD

#include "global.h"
#define ADC_CH_MASK ((0xCF) & (~0x40))

void adc_on
  (void);

void adc_off
  (void);

#endif /* __ADC_H_GUARD */
