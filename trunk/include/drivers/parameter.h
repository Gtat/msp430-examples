#ifndef __DRIVERS_PARAMETER_H_GUARD
#define __DRIVERS_PARAMETER_H_GUARD

#include <stdint.h>
#include "global.h"
#include "control.h"
#include "processing.h"
#include "drivers/flash.h"
#include "ram_symbols.h"

#define NUM_DAC_CHS 4

/** \union dac_word
 *  Configuration format for the TI TLV5620 Quadruple DAC.
 */
union __attribute__((packed)) dac_word
{
  struct __attribute__((packed)) dac_format_t
  {
    uint8_t range   : 1;
    uint8_t channel : 2;
    char    padding : 5;
    uint8_t data;
  } formatted;
  uint8_t bytes[sizeof(struct dac_format_t)];
};

struct rate_info
{
  uint16_t scan_rate;
#ifdef CONFIG_ENABLE_STORAGE_MODE
  uint16_t storage_rate;
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
};
enum rate_flags
{
  RATE_FLAGS_ADC,
  RATE_FLAGS_STIMULUS,
};

#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
struct amperometry_info
{
  union dac_word hi_volts;
  union dac_word lo_volts;
  uint16_t hi_seconds;
  uint16_t lo_seconds;
  uint16_t start_recording;
};
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */

#ifdef CONFIG_ENABLE_STORAGE_MODE
struct flash_record;
#endif

struct parameter_t
{
#ifdef CONFIG_ENABLE_DAC_BIASING
  union  dac_word         voltages[NUM_DAC_CHS];
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */
  uint8_t                 flags;
  struct rate_info        rates;
#ifdef CONFIG_ENABLE_STORAGE_MODE
  struct flash_record     data_record;
#endif
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
  struct amperometry_info amperometry;
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
  struct processor        process;
  struct processor        alarm;
};

void update_rates
  (enum rate_flags flags, uint16_t taccr);

#ifdef CONFIG_ENABLE_DAC_BIASING
void set_dac_voltage
  (union dac_word setting);
void set_all_dac_voltages
  (void);

#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
inline void amperometry_on
  (void);
void amperometry_off
  (void);
void set_dynamic_voltage
  (enum event event_type);
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */

#ifdef CONFIG_ENABLE_FLASH_PARAMS
//extern const struct parameter_t stored_parameters
//  __attribute__ (( section(".flash_storage") ));
  extern struct parameter_t parameters
    __attribute__ (( section(".ram_symbols") ));
# define CONFIGURATION (*RAM_CODE_PTR(parameters))
#else  /* #ifdef CONFIG_ENABLE_FLASH_PARAMS */
  extern struct parameter_t parameters;

# define CONFIGURATION parameters
#endif /* #ifdef CONFIG_ENABLE_FLASH_PARAMS */

#endif  /* __DRIVERS_PARAMETER_H_GUARD */

