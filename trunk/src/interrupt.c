/**
 * @file   interrupt.c
 * @author Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Interrupt routines for the iEGAS software platform. These govern (most of)
 * the system's interactions with hardware.
 */

/**
 * Issued when the USCI is ready to write another byte. Has the same operation
 * in both SPI and RS232 modes.
 */
void __attribute__(( interrupt(USCIAB0TX_VECTOR) )) usci_tx_isr
  (void)
{
  UCA0TXBUF = RING_QUEUE_POP(outgoing_comm_q);
  if (RING_QUEUE_EMPTY(outgoing_comm_q))
  {
    UC0IE &= ~UCA0TXIE;
  }
}

/**
 * Issued when the USCI has receieved a byte. In SPI mode, this indicates that
 * a byte has finished sending. In RS232 mode, this indicates that a byte was
 * received from the PC. In either case we must wake up the processor if a
 * packet has been completed.
 */
void __attribute__(( interrupt(USCIAB0RX_VECTOR) )) usci_rx_isr
  (void)
{
  P1OUT ^= 0x01;
  (volatile char)UCA0RXBUF;
#if 0
  if (UCA0CTL0 & UCSYNC) /* SPI mode */
  {
    (volatile char)UCA0RXBUF; /* dummy read to clear interrupt */
    if (RING_QUEUE_EMPTY(outgoing_comm_q))
    {
      __bic_SR_register_on_exit(LPM0_bits);
    }
  }
  else
  {
    RING_QUEUE_PUSH(incoming_comm_q, UCA0RXBUF);
    if (incoming_comm_q.length >= sizeof(union pc_to_mcu))
    {
      control.pc_packets++;
      __bic_SR_register_on_exit(LPM0_bits);
    }
  }
#endif
}

/**
 * Issued when an ADC conversion is complete. The ADC peripheral is
 * configured to sample all channels and automatically transfer results
 * into memory, so this indicates that a sample has been taken from
 * each channel and is ready for processing, storage, or transferral.
 */
void __attribute__(( interrupt(ADC10_VECTOR) )) adc_isr
  (void)
{
/* add these back, along with appropriate ring queue,
 * if you want ADC functionality
 */
/*
  RING_QUEUE_PUSH_NO_DATA(sample_q);
  ADC10SA = (uint16_t)&sample_q.data[sample_q.head];
*/
  __bic_SR_register_on_exit(LPM0_bits);
}

void __attribute__(( interrupt(TIMER1_A0_VECTOR) )) timer1_a0_isr
  (void)
{
  if (control.seconds <= 0)
  {
    control.seconds = control.interval;
    RING_QUEUE_PUSH(event_q, SET_LO_VOLTS);
    __bic_SR_register_on_exit(LPM0_bits);
  }
  control.seconds--;
}
