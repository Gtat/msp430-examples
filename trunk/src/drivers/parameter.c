#include "drivers/parameter.h"
#include "drivers/usci.h"
#include "config.h"


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
  (char flags, uint16_t taccr)
{
  TACCR0 = TACCR1 = parameters.rate.taccr = taccr;
}

/**
 *  Blocking operation to send a 16-bit configuration word over SPI to
 *  configure a new voltage setting on the TI TLV5620 4-channel DAC.
 *
 *  @param setting channel, range, and voltage information to be written.
 */
void set_voltage
  (uint8_t ch, struct dac_word setting)
{
  usci_write(setting.range | ch);
  usci_write(setting.data);
  __bic_SR_register(GIE);
  usci_set_mode(USCI_MODE_SPI);
  usci_commit();
  P2OUT |=  0x01; /* DAC enable line, active high */
  __bis_SR_register(LPM0_bits | GIE);
  P2OUT &= ~0x01;
}

void set_all_voltages
  (void)
{
  unsigned int ch;
//  for (ch = 0; ch < NUM_SIGNAL_CHS; ++ch)
//  {
    set_voltage(0, parameters.voltages[0]);
//  }
}

