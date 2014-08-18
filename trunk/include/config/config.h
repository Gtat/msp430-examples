#include "processing.h"
#include "config/options.h"

#define bswap16(x) \
  (((x) << 8)|((x) >> 8))

#define DAC_V_REF 2.5

#define FORMAT_DAC_VOLTAGE(ch, volts)                          \
  {                                                            \
    .formatted =                                               \
      {                                                        \
        .channel = (ch),                                       \
        .range   = ((volts) >= DAC_V_REF),                     \
        .data    = (uint8_t)(((volts) < DAC_V_REF)             \
                           ? ((volts) * 256.0 / (DAC_V_REF))   \
                           : ((volts) * 128.0 / (DAC_V_REF))), \
      },                                                       \
  }

#define SET_DAC_VOLTAGE_ARRAY(ch, volts) \
  [(ch)] = FORMAT_DAC_VOLTAGE(ch, volts)

struct parameter_t parameters =
  {
    .process  = INIT_PROCESSOR(truncate_sample),
    .alarm    = { 0 },
    .rates    =
      {
        .scan_rate = 0x1000,
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
        .hi_volts = FORMAT_DAC_VOLTAGE(2, 3.2),
        .lo_volts = FORMAT_DAC_VOLTAGE(2, 1),
        .hi_seconds = 1,
        .lo_seconds = 1,
      },
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
  };

