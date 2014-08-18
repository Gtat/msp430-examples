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
#include "ram_symbols.h"

struct control_t control = 
  { 
    .state      = STATE_IDLE,
    .toggle     = 0,
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
RING_QUEUE_CREATE_PREDEFINED(enum_event,     4, event_q);
union mcu_to_pc mcu_packet;
union pc_to_mcu  pc_packet;

int main
  (void)
{
  enum pc_packet_status packet_status;
  enum event event_type;
  uint8_t build_status;

  setup();                                     /* system setup */
  adc_setup(NUM_SIGNAL_CHS);
  usci_set_mode(USCI_MODE_RS232);

#ifdef CONFIG_ENABLE_STORAGE_MODE
  ram_routine_load();
  if (!parameters.data_record.length)
  {
    flash_record_init
      (&parameters.data_record,   
       (uint8_t *)&stored_data,
       sizeof(stored_data));
  }
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

  while(1)
  {
    /* TX state machine
     * The state cannot change in response to a TXed packet. 
     */
    switch ((const enum state_t)control.state)
    {
      case STATE_STREAM:
      {
        __bis_SR_register(LPM0_bits | GIE); /* enter low power mode 0 */
                                            /* with interrupts on */
        while (!RING_QUEUE_EMPTY(sample_q))
        {
          build_status = build_mcu_packet(&mcu_packet, DATA, control.seconds);
          send_mcu_packet(&mcu_packet, PACKET_OPT_NONE);
#ifdef CONFIG_ENABLE_STORAGE_MODE
          store_packet(&parameters.data_record, &mcu_packet, 0);
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
#ifdef CONFIG_ENABLE_ALERTS
          if (build_status)
          {
            build_mcu_packet(&mcu_packet, ALERT, build_status);
            send_mcu_packet(&mcu_packet, PACKET_OPT_BLOCK);
          }
#endif /* #ifdef CONFIG_ALERTS_ACTIVE */
        }
        break;
      }
#ifdef CONFIG_ENABLE_STORAGE_MODE
      case STATE_FLUSH:
      {
        build_mcu_packet(&mcu_packet, PREAMBLE);
        send_mcu_packet(&mcu_packet, PACKET_OPT_BLOCK);

        while (build_mcu_packet(&mcu_packet, 
                                STORED, 
                                &parameters.data_record) > 0)
        {
          send_mcu_packet(&mcu_packet, PACKET_OPT_BLOCK);
        }

        build_mcu_packet(&mcu_packet, OK);
        send_mcu_packet(&mcu_packet, PACKET_OPT_BLOCK);

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
        case PC_PACKET_HELLO:
        {
          build_mcu_packet(&mcu_packet, OK);
          send_mcu_packet(&mcu_packet, PACKET_OPT_BLOCK);
          usci_break();
          break;
	}
#ifdef CONFIG_ENABLE_STORAGE_MODE
        case PC_PACKET_DUMP:
        {
          control.state = STATE_FLUSH;
          break;
        }
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */
        case PC_PACKET_BEGIN:
        {
#ifdef CONFIG_ENABLE_DAC_BIASING
          set_all_dac_voltages();
#endif /* #ifdef CONFIG_ENABLE_DAC_BIASING */

          control.state = STATE_STREAM;
          break;
        }
        case PC_PACKET_HALT:
        {
#ifdef CONFIG_ENABLE_DYNAMIC_BIASING
          amperometry_off();
#endif /* #ifdef CONFIG_ENABLE_DYNAMIC_BIASING */
          usci_break();
          control.state = STATE_IDLE;
          break;
        }
        default: 
        {
          break;
        }
      }
    }

    while (!RING_QUEUE_EMPTY(event_q))
    {
      event_type = RING_QUEUE_POP(event_q);
      switch (event_type)
      {
        case SET_LO_VOLTS:
        {      
          usci_set_mode(USCI_MODE_SPI);
          set_dac_voltage(parameters.amperometry.lo_volts);
          usci_set_mode(USCI_MODE_RS232);
          break;
        }
        case SET_HI_VOLTS:
        {
          usci_set_mode(USCI_MODE_SPI);
          set_dac_voltage(parameters.amperometry.hi_volts);
          usci_set_mode(USCI_MODE_RS232);
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

