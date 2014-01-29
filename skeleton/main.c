#include <msp430.h>
#include <stdio.h>

#include "ringq.h"
#include "usci.h"

void setup
  (void)
{
  WDTCTL = WDTPW + WDTHOLD; /* stop watchdog */
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);
  } 
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}


RING_QUEUE_DECLARE(char, 16) stringq;

void push_test
  (char * buf)
{
  char * p;

  p = buf;
  while (*p)
  {
    RING_QUEUE_PUSH(stringq, *p++); 
  }
}

int putchar
  (int c)
{
  if (c)
  {
    while(UCA0STAT & UCBUSY);
    UCA0TXBUF = c;
    return 1;
  } 
  else
  {
    return 0;
  }
}

int main
  (void)
{
  setup();
  usci_setup(USCI_CHANNEL_A0, USCI_MODE_RS232, BAUDRATE(1000000, 9600), 0);

  RING_QUEUE_INIT(stringq);
  push_test("Hello, queue!");
  while (putchar(RING_QUEUE_POP(stringq)));
  
  return 0;
}


