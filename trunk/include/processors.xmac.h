EXPAND_PROCESSOR(nothing, {}) 

EXPAND_PROCESSOR(truncate_sample, {})
#define truncate_sample_initializer {}

#define include_moving_average
#ifdef include_moving_average
  #define MOVING_AVG_LENGTH 16
  EXPAND_PROCESSOR(moving_average, 
                   {
                     uint8_t  samples[NUM_SIGNAL_CHS][MOVING_AVG_LENGTH];
                     uint16_t index;
                     uint16_t sum[NUM_SIGNAL_CHS];
                   },
                  )
  #define moving_average_initializer \
     (struct moving_average_state)   \
     {                               \
       .samples = { { 0 } },         \
       .index   = 0,                 \
       .sum     = { 0 },             \
     }
#endif

//#define include_threshold
#ifdef include_threshold
  EXPAND_PROCESSOR(threshold, 
                   {
                     uint16_t thresholds[NUM_SIGNAL_CHS];
                   },
                   {
                     .thresholds = { [NUM_SIGNAL_CHS-1] = 0x200 }
                   }
                  )
  #define threshold_initializer                     \
     (struct threshold_state)                       \
     {                                              \
       .thresholds = {                              \
                       [0]                = 0x200,  \
                       [1]                = 0x3ff,  \
                       [2]                = 0x3ff,  \
                       [3]                = 0x3ff,  \
                       [NUM_SIGNAL_CHS-1] = 0x3ff,  \
                     },                              \
     }
#endif

