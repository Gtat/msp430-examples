#include "processing.h"

uint8_t execute_truncate_sample
  (uint16_t sample, unsigned int ch, struct processor * p)
{
  return ((sample & 0x3FF) >> 2);
}

uint8_t execute_moving_average
  (uint16_t sample, unsigned int ch, struct processor * p)
{
  uint8_t incoming;
  struct moving_average_state *state;

  state = &p->state.moving_average;
  incoming = execute_truncate_sample(sample, ch, NULL);

  state->sum += incoming;
  state->sum -= state->samples[ch][state->index];

  state->samples[ch][state->index] = incoming;
  if (state->index < MOVING_AVG_LENGTH)
  {
    ++state->index;
  }
  else
  {
    state->index = 0;
  }

  return (state->sum / MOVING_AVG_LENGTH);
}

