#pragma vector=USCIAB0TX_VECTOR
__interrupt void usci_tx_isr
  (void)
{
  UCA0TXBUF = RING_QUEUE_POP(outgoing_comm_q); 
  if (RING_QUEUE_EMPTY(outgoing_comm_q))
  {
    IE2 &= ~UCA0TXIE;
   if (!(UCA0CTL0 & UCSYNC))
   {
      usci_set_mode(USCI_MODE_OFF);
    }
  }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void usci_rx_isr
  (void)
{
  char dummy;
  if (UCA0CTL0 & UCSYNC) /* SPI mode */
  {
    dummy = UCA0RXBUF;
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

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_isr
  (void)
{
  RING_QUEUE_PUSH_NO_DATA(sample_q);

  if (!RING_QUEUE_EMPTY(outgoing_comm_q))
  {
    usci_set_mode(USCI_MODE_RS232);
    usci_commit();
  }
  else
  {
    ADC10SA = (uint16_t)&sample_q.data[sample_q.head];
  }

  __bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  TACTL = 0;
  LPM0_EXIT;
}

