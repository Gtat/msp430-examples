
//******************************************************************************
#include <msp430.h>

short ADC_Data[99];
int DataCount = 0;
short UART_Data[99];
int UARTcount = 0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
  /* CONFIGURE UART MODULE */

  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
  //IE2 |= UCA0TXIE; 							// enable TX ISR for enable/disable ADC10
  /* SET REF VOLTAGES AND CHANNELS */
  ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_3;    // TA1 trigger sample start
  ADC10CTL0 = ADC10SHT_2  + ADC10ON + ADC10IE;
  /* DELAY WHILE REF VOLTAGE SETTLES */
  __enable_interrupt();                     // Enable interrupts.
  TACCR0 = 30;                              // Delay to allow Ref to settle
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  TACTL = TASSEL_2 + MC_1;                  // TACLK = SMCLK, Up mode.
  LPM0;                                     // Wait for delay.
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();
//  /* END OF DELAY, DISABLE ISR USED IN DELAY */
  ADC10AE0 |= 0x08;                         // P1.3 ADC10 option select
  ADC10CTL0 |= ENC;                         // ADC10 Enable
//  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  TACCR0 = 10000;                          // PWM Period
  TACCTL1 = OUTMOD_3;                       // TACCR1 set/reset
  TACCR1 = 10000;                            // TACCR1 PWM Duty Cycle
  TACTL = TASSEL_2 + MC_1;

  UCA0TXBUF = 0x77;			// lucky number?

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
}

//  Echo back RXed character, confirm TX buffer is ready first
/* NOT NEEDED FOR SENDING ADC10 ONLY */

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	/* USED FOR DEBUGGIN! */

  //ADC_Data[DataCount] = UCA0RXBUF;
  //DataCount++;
  UART_Data[UARTcount] = UCA0RXBUF;
  UARTcount++;
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	//if(test == 0) { Channel1[DataCount]; test == 1; }
	//else { Channel2[DataCount]
	ADC10CTL0 &= ~ENC; // disable for UART TX
	unsigned char temp = 0;
	temp = ADC10MEM >> 2;
	if( DataCount < 99 ) {
	ADC_Data[DataCount] = temp;
	DataCount++;
	}
	//temp = ADC10MEM >> 2;
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	UCA0TXBUF = temp;                    // TX -> RXed character
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	ADC10CTL0 |= ENC;
}

/* ONLY FOR CONFIGURATION STEPS, WAITING FOR REF VOLTAGES */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  TACTL = 0;								// reset timer reg
  LPM0_EXIT;                                // Exit LPM0 on return
}

/*
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	ADC10CTL0 |= ENC; // enable the ADC again
}
*/

