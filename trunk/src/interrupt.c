#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_rx_isr
  (void)
{
  /* at each received keystroke: */
  P1OUT ^= 0x01;
//  RING_QUEUE_PUSH(incoming_q, UCA0RXBUF); /* change upper <-> lower case */
//                                              /* of received character and   */
//                                              /* put it in a fifo            */
//  if (incoming_q.length >= sizeof(union pc_command))
//  {
  __bic_SR_register_on_exit(LPM0_bits);
//  }
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_isr
  (void)
{
  P1OUT ^= 0x01;
  putchar('W');
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_isr
  (void)
{
  P1OUT ^= 0x01;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  TACTL = 0;
  LPM0_EXIT;
}

