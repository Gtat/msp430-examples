#include "drivers/adc.h"

/**
 * Turn off digital pin functions and start conversions, assuming the
 * peripheral has already been set up with adc_setup().
 */
void adc_on
  (void)
{
  ADC10AE0   = ADC_CH_MASK;
  ADC10CTL0 |= (ENC | ADC10IE);

  ADC10DTC1  = NUM_TOTAL_CHS;
  ADC10SA    = (uint16_t)&sample_q.data[sample_q.head];
}

/**
 * Stop conversions and turn on digital pin functions.
 */
void adc_off
  (void)
{
  ADC10CTL0 &= ~(ENC | ADC10IE);
  while (ADC10CTL1 & BUSY);
  ADC10DTC1  = 0;
  ADC10AE0   = 0;
}

