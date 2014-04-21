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
#pragma vector=USCIAB0TX_VECTOR
__interrupt void usci_tx_isr
  (void)
{
  UCA0TXBUF = RING_QUEUE_POP(outgoing_comm_q); 
  if (RING_QUEUE_EMPTY(outgoing_comm_q))
  {
    IE2 &= ~UCA0TXIE;
  }
}

/**
 * Issued when the USCI has receieved a byte. In SPI mode, this indicates that
 * a byte has finished sending. In RS232 mode, this indicates that a byte was
 * received from the PC. In either case we must wake up the processor if a 
 * packet has been completed.
 */
#pragma vector=USCIAB0RX_VECTOR
__interrupt void usci_rx_isr
  (void)
{
  if (UCA0CTL0 & UCSYNC) /* SPI mode */
  {
    UCA0RXBUF;           /* dummy read to clear interrupt */
                         /* this works because TI's header file defines every */
                         /* register as volatile, grumble grumble */
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
}

/**
 * Issued when an ADC conversion is complete. The ADC peripheral is
 * configured to sample all channels and automatically transfer results
 * into memory, so this indicates that a sample has been taken from 
 * each channel and is ready for processing, storage, or transferral.
 */
#pragma vector=ADC10_VECTOR
__interrupt void adc_isr
  (void)
{
  RING_QUEUE_PUSH_NO_DATA(sample_q);
  ADC10SA = (uint16_t)&sample_q.data[sample_q.head];
  __bic_SR_register_on_exit(LPM0_bits);
}

/**
 * This timer is configured as the conversion trigger for the ADC, but
 * it will only have an interrupt during setup to inform us that the
 * reference settling time is complete.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer_isr(void)
{
  TA0CTL = 0;
  LPM0_EXIT;
}
