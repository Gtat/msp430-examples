#define bswap16(x) \
  (((x) << 8)|((x) >> 8))

#if 0
#define DAC_VOLTAGE(ch, volts)                                  \
  [(ch)] =                                                      \
  {                                                             \
    .word = bswap16((uint16_t)((volts) * 256.0)) | ((ch) << 1), \
  }
#endif
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
    .process  = NULL,
    .processing_option = NULL,
    .rate     =
    {
      .taccr = 0xF424,
    },
    .voltages =
    {
      #include "config/dac_voltages.h"
#if 0
      [0] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0x01,
        },
      },
      [1] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0x23,
        }
      },
      [2] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0x45,
        }
      },
      [3] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0x67,
        }
      },
      [4] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0x89,
        },
      },
      [5] = 
      {
        .payload = 
        {
          .range = 0,
          .data  = 0xAB,
        },
      },
#endif
    },
  };
