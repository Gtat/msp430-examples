#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_rx_isr
  (void)
{
  /* at each received keystroke: */
//  RING_QUEUE_PUSH(incoming_comm_q, UCA0RXBUF); /* change upper <-> lower case */
//                                              /* of received character and   */
//                                              /* put it in a fifo            */
//  if (incoming_comm_q.length >= sizeof(union pc_to_mcu))
//  {
//    control.pc_packets++;
//    __bic_SR_register_on_exit(LPM0_bits);
//  }
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_isr
  (void)
{
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_isr
  (void)
{
//  RING_QUEUE_PUSH(sample_q, ADC10MEM);
  control.sample_ct++;
  __bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  TACTL = 0;
  LPM0_EXIT;
}

