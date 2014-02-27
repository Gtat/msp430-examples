#include "drivers/parameter.h"
#include "drivers/usci.h"

inline void update_rates
  (char flags, uint16_t taccr)
{
  TACCR0 = TACCR1 = taccr;
}

void set_voltage
  (union word dac_word)
{
  usci_write(dac_word.bytes[0]);
  usci_write(dac_word.bytes[1]);
  __bic_SR_register(GIE);
  usci_set_mode(USCI_MODE_SPI);
  usci_commit();
  P2OUT |=  0x01; /* DAC enable line, active high */
  __bis_SR_register(LPM0_bits | GIE);
  P2OUT &= ~0x01;
}
