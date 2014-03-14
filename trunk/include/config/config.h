#include "processing.h"

#define bswap16(x) \
  (((x) << 8)|((x) >> 8))

#define DAC_VOLTAGE(ch, volts)                 \
  [(ch)] =                                     \
  {                                            \
    .formatted =                               \
    {                                          \
      .channel = (ch),                         \
      .range   = ((volts) >= 1.0),             \
      .data    = (uint8_t)(((volts) < 1.0)     \
                         ? ((volts) * 256.0)   \
                         : ((volts) * 128.0)), \
    },                                         \
  }

struct parameter_t parameters =
  {
    .process  = INIT_PROCESSOR(truncate_sample),
    .rate     =
    {
      .taccr = 0xF424,
    },
    .voltages =
    {
      #include "config/dac_voltages.h"
    },
  };
