#include "drivers/adc.h"

void adc_on
  (void)
{
  ADC10AE0   = ADC_CH_MASK;
  ADC10CTL0 |= ENC;
}

void adc_off
  (void)
{
  while (ADC10CTL1 & BUSY);
  ADC10AE0   = 0;
  ADC10CTL0 &= ~ENC;
}

