#ifndef __CONTROL_H_GUARD
#define __CONTROL_H_GUARD

/** \union word
 *  Data type for prettying up bytewise accesses to an object of 
 *  native word size.
 */
union word
{
  unsigned int word;
  uint8_t      bytes[sizeof(unsigned int)];
};

/** \typdef sample_buffer
 *  The type of an array of captured ADC samples. Capable of holding a sample
 *  from every channel.
 */
typedef uint16_t sample_buffer[NUM_TOTAL_CHS];
enum event
{
  SET_LO_VOLTS,
  SET_HI_VOLTS,
};

struct control_t
{
  enum state_t
  {
    STATE_IDLE,
    STATE_SETUP,
    STATE_STREAM,
    STATE_FLUSH,
  } state : 2;

  uint8_t           toggle  : 1;
  int16_t           seconds;
  volatile uint8_t  pc_packets;
  const    uint16_t channels;
};

#endif /* #ifndef __CONTROL_H_GUARD */

