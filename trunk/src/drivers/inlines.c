
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

  /* DELAY WHILE REF VOLTAGE SETTLES */
  TACCR0   = 30;                       /* Delay to allow Ref to settle */
  TACTL    = TASSEL_2 | MC_1;          /* TACLK = SMCLK, Up mode. */
  TACCTL0 |= CCIE;                     /* Compare-mode interrupt. */
  __bis_SR_register(LPM0_bits | GIE);  /* low power with interrupts enabled */
  TACCTL0 &= ~CCIE;                    /* disable timer Interrupt */
  __disable_interrupt();

  /* SET TIMER PWM FOR ADC10 TRIGGER! */
  TACCTL1 = OUTMOD_3;                  /* When counter == TACCR1, set output. */
                                       /* When counter == TACCR0, clear output. */
  TACCR0 = 0xf424 / channels;          /* 62500 * 64 / (16 MHz / 2 / 2) = 1.0 s */
  TACCR1 = 0xf424 / channels;          /* 50% duty cycle */
  TACTL = TASSEL_2 |                   /* Source from 16 MHz SMCLK */
          MC_1 |                       /* Count up */
          ID_3;                        /* divide clock by 8 */

  while (ADC10CTL1 & BUSY);
  ADC10DTC0  = 0;
  ADC10DTC1  = NUM_TOTAL_CHS; 
}

