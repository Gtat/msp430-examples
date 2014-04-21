#include "processing.h"

#define bswap16(x) \
  (((x) << 8)|((x) >> 8))

#define FORMAT_DAC_VOLTAGE(ch, volts)          \
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

#define SET_DAC_VOLTAGE_ARRAY(ch, volts)       \
  [(ch)] = FORMAT_DAC_VOLTAGE(ch, volts)

struct parameter_t parameters =
  {
    .process  = INIT_PROCESSOR(truncate_sample),
    .alarm    = { 0 },
    .rate     =
    {
      .taccr = 0xF424,
    },
    .voltages =
    {
      #include "config/dac_voltages.h"
    },
    .amperometry =
    {
      .hi_volts = FORMAT_DAC_VOLTAGE(0, 1.5),
      .lo_volts = FORMAT_DAC_VOLTAGE(0, 0.5),
      .hi_seconds = 5,
      .lo_seconds = 7,
    },
  };
