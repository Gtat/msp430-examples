
//******************************************************************************


#include <msp430g2553.h>

unsigned short DataCount = 0;
unsigned short ADC_Data[99];

int test = 0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_1;    // TA1 trigger sample start
  ADC10CTL0 = ADC10SHT_2  + ADC10ON + ADC10IE; // samples 16 times.
  __enable_interrupt();                     // Enable interrupts (for timer).
  TACCR0 = 30;                              // Delay to allow Ref to settle
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  TACTL = TASSEL_2 + MC_1;                  // TACLK = SMCLK, Up mode.
  LPM0;                                     // Wait for delay.
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();					// clock interrupt disabled
  ADC10AE0 |= 0x02;                         // P1.1, P1.0 ADC10 option select
  ADC10CTL0 |= ENC;                         // ADC10 Enable

  TACCR0 = 501-1;                          // PWM Period
  TACCTL1 = OUTMOD_3;                       // TACCR1 set/reset
  TACCR1 = 500;                            // TACCR1 PWM Duty Cycle
  TACTL = TASSEL_1 + MC_1;                  // ACLK, up mode

  //while(1) {}
  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupts
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	//if(test == 0) { Channel1[DataCount]; test == 1; }
	//else { Channel2[DataCount]
	ADC_Data[DataCount] = ADC10MEM;
	DataCount++;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  TACTL = 0;								// reset timer reg
  LPM0_EXIT;                                // Exit LPM0 on return
}
