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

#pragma vector=TIMER1_A0_VECTOR
__interrupt void timer1_a0_isr
  (void)
{
  if (control.seconds == parameters.amperometry.hi_seconds)
  {
    usci_set_mode(USCI_MODE_SPI);
    set_dac_voltage(parameters.amperometry.hi_volts);
    usci_set_mode(USCI_MODE_RS232);
  }
  
  if (control.seconds == parameters.amperometry.lo_seconds)
  {
    usci_set_mode(USCI_MODE_SPI);
    set_dac_voltage(parameters.amperometry.lo_volts);
    usci_set_mode(USCI_MODE_RS232);
    control.seconds = 0;
  }
  else
  {
    ++control.seconds;
  }
  __bic_SR_register_on_exit(LPM0_bits);
}
