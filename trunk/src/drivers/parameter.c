#include "drivers/parameter.h"
#include "drivers/usci.h"
#include "config/config.h"

#ifdef CONFIG_ENABLE_STORAGE_MODE
const struct parameter_t stored_parameters;
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

/**
 * Set new timer values in order to change the ADC sampling rate and the
 * frequency of storage and output.
 *
 * @param flags  bit-packed flags indicating the frequencies for storing and
 *               outputting data.
 * @param taccr  the new value for the timer A registers used to determine the
 *               rate at which ADC conversions are triggered.
 */
inline void update_rates
  (enum rate_flags flags, uint16_t taccr)
{
  switch (flags)
  {
    case RATE_FLAGS_ADC:
    {
      TACCR0 = TACCR1 = CONFIGURATION.rates.scan_rate = taccr;
      break;
    }
    default:
    {
      break;
    }
  }
}

#ifdef CONFIG_ENABLE_DAC_BIASING
/**
 *  Blocking operation to send a 16-bit configuration word over SPI to
 *  configure a new voltage setting on the TI TLV5620 4-channel DAC.
 *
 *  @param setting channel, range, and voltage information to be written.
 */
void set_dac_voltage
  (union dac_word setting)
{
  usci_write(setting.bytes[0]);
  usci_write(setting.bytes[1]);
  P2OUT |=  0x01; /* DAC enable line, active high */
  usci_commit_blocking();
  P2OUT &= ~0x01;
}

void set_all_dac_voltages
  (void)
{
  unsigned int ch;

#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
  amperometry_off();
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */

  usci_set_mode(USCI_MODE_SPI);
  for (ch = 0; ch < NUM_DAC_CHS; ++ch)
  {
    set_dac_voltage(parameters.voltages[ch]);
  }
  usci_set_mode(USCI_MODE_RS232);

#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
  amperometry_on();
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
}

#ifdef CONFIG_ENABLE_DYNAMIC_BIASING 
void amperometry_on
  (void)
{
  TA1CCR0   = 0x8000;
  TA1CCTL0 |= CCIE;
}

void amperometry_off
  (void)
{
  TA1CCTL0 &= ~CCIE;
}
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */

