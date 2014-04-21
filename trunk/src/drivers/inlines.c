
/**
 * General purpose setup to get the board running.
 */
static inline void setup
  (void)
  __attribute__((always_inline));

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
 * Initial ADC configuration. 
 *
 * @param channels  The number of channels to sample.
 */
static inline void adc_setup 
  (const unsigned int channels)
  __attribute__((always_inline));

static inline void adc_setup 
  (const unsigned int channels)
{
    /* SET REF VOLTAGES AND CHANNELS */
  ADC10CTL1 = SHS_1 |                  /* use Timer_A0 */
              CONSEQ_3 |               /* repeat sequence of channels */
              INCH_7;                  /* sequence through all channels */
  ADC10CTL0 = SREF_0 |
              ADC10SHT_3 |             /* sample and hold for 64 clocks */
              ADC10ON |                /* ADC on */
              ADC10IE;                 /* interrupt active */

  /* use timer A1_0 for this to avoid an extra ISR */
  TA0CCR0   = 30;                       /* Delay to allow Ref to settle */
  TA0CTL    = TASSEL_2 | MC_1;          /* TACLK = SMCLK, Up mode. */
  TA0CCTL0 |= CCIE;                     /* Compare-mode interrupt. */
  __bis_SR_register(LPM0_bits | GIE);  /* low power with interrupts enabled */
  TA0CCTL0 &= ~CCIE;                    /* disable timer Interrupt */
  __disable_interrupt();


  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  BCSCTL3  = XCAP_3;                   /* oscillator capacitance == 12.5 pF */
  TA0CCTL1 = OUTMOD_3;                  /* When counter == TACCR1, set output. */
                                       /* When counter == TACCR0, clear output. */
  TA0CTL = TASSEL_1 |                   /* source from 32.768 kHz ACLK */
           MC_1;                        /* count up */ 
  update_rates(0, 0x8000 / channels);  /* 32768 / 32.768 kHz = 1.0 s */

  while (ADC10CTL1 & BUSY);
  ADC10DTC0  = 0;
  ADC10DTC1  = NUM_TOTAL_CHS; 
}

static inline void timer_setup
  (void)
{
  TA1CCR0   = 0x8000; 
  TA1CCTL1  = OUTMOD_1;
  TA1CTL    = TASSEL_1 | 
             MC_1;
  TA1CCTL0 |= CCIE;
}
