/**
 * @file    main.c
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * The Intelligent Electrochemical Gas Analysis System's microcontroller
 * software platform. The software is interrupt-driven, so this file only
 * performs setup of peripherals and data structures, then falls into a
 * state machine loop. It sleeps between iterations of the state machine
 * and only wakes up to construct outgoing packets or parse incoming packets
 * for configuration information.
 */

#include <stdio.h>

#include "global.h"
#include "ringq.h"
#include "protocol.h"

#include "drivers/flash.h"
#include "drivers/parameter.h"
#include "drivers/usci.h"
#include "processing.h"

#include "drivers/inlines.c"

static struct control_t
{
  enum state
  {
    STATE_IDLE,
    STATE_SETUP,
    STATE_STREAM,
    STATE_FLUSH,
  } state;

  uint16_t seconds;
  volatile uint8_t pc_packets;
  const    uint16_t channels;
} control = 
  { 
    .state      = STATE_IDLE,
    .seconds    = 0,
    .pc_packets = 0,
    .channels   = NUM_SIGNAL_CHS,
  };

/* declare and initialize a ring queue AKA circular buffer AKA fifo */
/* data type is char, depth is 16 elements, identifier is "stringq" */
/* declare it globally so it is shared between main and interrupt */
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, incoming_comm_q);
RING_QUEUE_CREATE_PREDEFINED(uint8_t,       16, outgoing_comm_q);
RING_QUEUE_CREATE_PREDEFINED(sample_buffer,  4, sample_q);
union mcu_to_pc mcu_packet;
union pc_to_mcu  pc_packet;

#ifdef CONFIG_ENABLE_STORAGE_MODE
struct flash_record config_record = { 0 };
struct flash_record data_record   = { 0 };
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

int main
  (void)
{
  enum pc_packet_status packet_status;
  uint8_t build_status;

  setup();                                     /* system setup */
  adc_setup(NUM_SIGNAL_CHS);
  usci_set_mode(USCI_MODE_RS232);
#ifdef CONFIG_ENABLE_STORAGE_MODE
  flash_record_init(&data_record,   (uint8_t *)&stored_data,       0x80);
  flash_record_init(&config_record, (uint8_t *)&stored_parameters, 0x80);
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

  while(1)
  {
    /* TX state machine
     * The state cannot change in response to a TXed packet. 
     */
    switch ((const enum state)control.state)
    {
      case STATE_STREAM:
      {
        __bis_SR_register(LPM0_bits | GIE); /* enter low power mode 0 */
                                            /* with interrupts on */
        while (!RING_QUEUE_EMPTY(sample_q))
        {
          build_status = build_mcu_packet(&mcu_packet, DATA, control.seconds);
          send_mcu_packet(&mcu_packet);
#ifdef CONFIG_ENABLE_ALERTS
          if (build_status)
          {
            build_mcu_packet(&mcu_packet, ALERT, build_status);
            send_mcu_packet(&mcu_packet);
          }
#endif /* #ifdef CONFIG_ALERTS_ACTIVE */
        }
        break;
      }
#ifdef CONFIG_ENABLE_STORAGE_MODE
      case STATE_FLUSH:
      {
        build_mcu_packet(&mcu_packet, PREAMBLE, &config_record);
        send_mcu_packet(&mcu_packet);
        usci_block_tx();

        while (build_mcu_packet(&mcu_packet, STORED, &data_record) >= NUM_SIGNAL_CHS)
        {
          send_mcu_packet(&mcu_packet);
          usci_block_tx();
        }

        build_mcu_packet(&mcu_packet, OK);
        send_mcu_packet(&mcu_packet);
        usci_block_tx();

        control.state = STATE_IDLE;
        break;
      }
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
      default:
      {
        __bis_SR_register(LPM0_bits | GIE); /* enter low power mode 0 */
                                            /* with interrupts on */
        break;
      }
    }

    /* RX state machine */
    for ( ; control.pc_packets > 0; --control.pc_packets)
    {
      packet_status = process_pc_packet(&pc_packet);
      switch (packet_status)
      {
#ifdef CONFIG_ENABLE_STORAGE_MODE
        case PC_PACKET_DUMP:
        {
          control.state = STATE_FLUSH;
          break;
        }
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
        case PC_PACKET_BEGIN:
        {
          set_all_dac_voltages();
          control.state = STATE_STREAM;
          break;
        }
        case PC_PACKET_HALT:
        {
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
          amperometry_off();
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
          control.state = STATE_IDLE;
          break;
        }
        default: 
        {
          break;
        }
      }
    }
  }

  return 0;
}

#include "interrupt.c"

