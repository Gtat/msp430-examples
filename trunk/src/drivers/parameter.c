#include "drivers/parameter.h"

inline void update_rates
  (char flags, uint16_t taccr)
{
  TACCR0 = TACCR1 = taccr;
}
