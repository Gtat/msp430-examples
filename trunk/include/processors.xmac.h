EXPAND_PROCESSOR(truncate_sample, {})

//#define include_moving_average
#ifdef include_moving_average
  #define MOVING_AVG_LENGTH 16
  EXPAND_PROCESSOR(moving_average, 
                    {
                      uint8_t  samples[NUM_SIGNAL_CHS][MOVING_AVG_LENGTH];
                      uint16_t index;
                      uint16_t sum[NUM_SIGNAL_CHS];
                    }
                  )
#endif

