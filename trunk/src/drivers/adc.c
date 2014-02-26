#include "drivers/adc.h"

void adc_on
  (void)
{
  ADC10AE0   = ADC_CH_MASK;
  ADC10CTL0 |= ENC;
  ADC10SA    = (uint16_t)&sample_q.data[sample_q.head];
}

void adc_off
  (void)
{
  ADC10AE0   = 0;
  ADC10CTL0 &= ~ENC;
}

