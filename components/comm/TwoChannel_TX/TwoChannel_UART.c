/*
 * This program is capable of utilizing the ADC10 and UART modules
 * ADC10: P1.4 and P1.3 are the input channels
 * 		  Samples a channel every 1ms
 * 		  immediately transmits 8 MSBs of ADC10 over UART
 *
 * UART: Set up to be transfer at 9600 baud
 * 		 No ISR enabled to drive UART. ONLY TX'd IN ADC10 ISR
 */

//******************************************************************************
#include <msp430g2553.h>

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
  //IE2 |= UCA0RXIE;   /* ENABLE ONLY IF RX ISR IS UNCOMMENTED BELOW */

  /* SET REF VOLTAGES AND CHANNELS */
  ADC10CTL1 = SHS_1 + CONSEQ_3 + INCH_4;    // TA1 trigger sample start, P1.4 top ADC10 Channel
  ADC10CTL0 = ADC10SHT_2  + ADC10ON + ADC10IE;	// 16x S&H time and ISR enabled
  //ADC10CTL0 |= MS; /* ENABLE THIS LINE TO DO CONSECUTIVE ADC Convertions w/o waiting on timer */


  /* DELAY WHILE REF VOLTAGE SETTLES */

  __enable_interrupt();                     // Enable interrupts. To be used w/timer.
  TACCR0 = 30;                              // Delay time to allow Ref to settle
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  TACTL = TASSEL_2 + MC_1;                  // TACLK = SMCLK, Up mode.
  LPM0;                                     // Enter low power while ref voltages settle
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();
//  /* END OF DELAY, DISABLE ISR USED IN DELAY */

  ADC10AE0 |= 0x18;                         // P1.4, P1.3 ADC10 In Channel select
  ADC10CTL0 |= ENC;                         // ADC10 Enable

//  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  TACCR0 = 10000;                          // PWM Period = 100 Hz
  TACCTL1 = OUTMOD_3;                       // TACCR1 set/reset
  TACCR1 = 10000;                            // TACCR1 PWM Duty Cycle (compare register)
  TACTL = TASSEL_2 + MC_1;					// Up Mode, SMCLK in input to Timer

  UCA0TXBUF = 0x77;							// Test UART before entering LMP0

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
  /* NOTE! LMP0 is needed so SMCLK is active for Timer.
   * CPU and MCLK are disabled while in LMP0
   */
}

//  Echo back RXed character, confirm TX buffer is ready first
/* NOT NEEDED FOR SENDING ADC10 ONLY */

//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
//{
//	/* USED FOR DEBUGGIN! */
//
//  //ADC_Data[DataCount] = UCA0RXBUF;
//  //DataCount++;
//  UART_Data[UARTcount] = UCA0RXBUF;
//  UARTcount++;
//  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
//  UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
//}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

	//ADC10CTL0 &= ~ENC; // disable for UART TX

	if( DataCount < 99 ) {
	ADC_Data[DataCount] = ADC10MEM >> 2;
	DataCount++;
	}
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	UCA0TXBUF = ADC10MEM >> 2;                    // TX -> RXed character
	//while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	//ADC10CTL0 |= ENC;
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

