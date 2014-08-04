#include "processing.h"
#include "config/options.h"

#define bswap16(x) \
  (((x) << 8)|((x) >> 8))

#define FORMAT_DAC_VOLTAGE(ch, volts)            \
  {                                              \
    .formatted =                                 \
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
    .process  = INIT_PROCESSOR(moving_average),
    .alarm    = { 0 },
    .rates    =
      {
        .scan_rate = 0xF424,
      },
#ifdef CONFIG_ENABLE_DAC_BIASING
    .voltages =
      {
        #include "config/dac_voltages.h"
      },
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */
#ifdef CONFIG_ENABLE_STORAGE_MODE
    .data_record = { 0 },
#endif /* CONFIG_ENABLE_STORAGE_MODE */
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
    .amperometry =
      {
        .hi_volts = FORMAT_DAC_VOLTAGE(0, 1.5),
        .lo_volts = FORMAT_DAC_VOLTAGE(0, 0.5),
        .hi_seconds = 6,
        .lo_seconds = 10,
      },
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
  };

