
/**
 * General purpose setup to get the board running.
 */
static inline void setup
  (void)
{
  WDTCTL = WDTPW | WDTHOLD; /* stop watchdog */
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);               /* trap CPU if clock is not calibrated */
  } 
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}

/**
 * Set up the watchdog as an interval timer interrupt.
 */
static inline void timer_setup
  (void)
{
  WDTCTL  =  WDT_ADLY_16; /* interrupt every ~16 ms */
  IE1    &= ~NMIIE;
  IE1    |=  WDTIE;
}


/**
 * Initial ADC configuration. 
 *
 * @param channels  The number of channels to sample.
 * @param rate      The starting sample rate in Hz.
 */
 static inline void adc_setup 
  (uint8_t channels, uint8_t rate)
{
    /* SET REF VOLTAGES AND CHANNELS */
  ADC10CTL1 = SHS_1 |                  /* use Timer_A0 */
              CONSEQ_3 |               /* repeat sequence of channels */
              INCH_4;
              //((channels & 0xF) <<4 ); /* sequence through the requested */
                                       /* number of channels */

  ADC10CTL0 = ADC10SHT_2 |             /* sample and hold for 16 clocks */
              ADC10ON |                /* ADC on */
              ADC10IE;                 /* interrupt active */

  /* DELAY WHILE REF VOLTAGE SETTLES */
  TACCR0   = 30;                            // Delay to allow Ref to settle
  TACTL    = TASSEL_2 | MC_1;               // TACLK = SMCLK, Up mode.
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  __bis_SR_register(LPM0_bits | GIE);
  //LPM0;                                     // Wait for delay.
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();

  /* END OF DELAY, DISABLE ISR USED IN DELAY */
  ADC10AE0 = BIT4;//1 << channels;
  ADC10CTL0 |= ENC;                    /* enable conversion */
  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  
  TACCR0 = 0xf424;                          // PWM Period = 1 Hz
  TACCTL1 = OUTMOD_3;                       // TACCR1 set/reset
  TACCR1 = 0xf424;                          // TACCR1 PWM Duty Cycle
  TACTL = TASSEL_2 |                        // SMCLK
          MC_1 |                            // up mode
          ID_3;                             // divide by 8
}

