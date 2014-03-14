EXPAND_PROCESSOR(truncate_sample, {})

#define MOVING_AVG_LENGTH 64
EXPAND_PROCESSOR(moving_average, 
                  {
                    uint8_t  samples[NUM_SIGNAL_CHS][MOVING_AVG_LENGTH];
                    uint16_t index;
                    uint16_t sum;
                  }
                )

