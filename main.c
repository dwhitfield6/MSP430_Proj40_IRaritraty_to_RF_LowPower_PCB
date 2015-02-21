#include <msp430.h>
#include "IR_read.h"

#define BatVoltage BIT0
#define Switch1 BIT1
#define LED0 BIT3
#define LED1 BIT2
#define IRrec BIT4
#define IRrec1 BIT5
#define TX BIT6

unsigned char differences =0;
int amount =10;

int ledcount= 1000;
unsigned char hold =0;
unsigned char holdArrayamount =0;

//unsigned char TXdata[12] = {0,2,2,2,2,2,0,0,1,0,0,0}; // channel E
unsigned char TXdata[12] = {2,2,2,2,2,2,0,0,1,0,0,0}; // channel D (A1 A0 x x x x x x)
unsigned int receive[100];
unsigned int HOLDreceive[100];
int i=0;
int timeout =0;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	  P1DIR |= (LED1 + LED0 + TX);                            // Set all pins but RXD to output
	  P1OUT &= ~(LED1 + LED0 + TX);
	  P1REN |= Switch1;
	  P1OUT |= Switch1;
	  P1IE |= IRrec; // P1.3 interrupt enabled
	  P1IES &= ~IRrec; // P1.3 interrupt enabled
	  P1IFG &= ~IRrec; // P1.3 IFG cleared
	  P1IE |= IRrec1; // P1.3 interrupt enabled
	  P1IES |= IRrec1; // P1.3 interrupt enabled
	  P1IFG &= ~IRrec1; // P1.3 IFG cleared

	  TA0CTL = TASSEL_2 + MC_2 + ID_3;           // SMCLK/8, upmode

	  _BIS_SR(LPM4_bits + GIE); // Enter LPM3 w/interrupt

	  //__enable_interrupt(); // enable all interrupts                                   // Enable CPU interrupts
	while(1)
	{

		_delay_cycles(100);


timeout++;
if(timeout == 1000)
{
	TA0CTL |= TACLR;
	hold = readSwitchP1(Switch1);

	if(hold == 1)
	{
		holdArrayamount = i;
		ArrayCopy(receive,HOLDreceive, 100);
		BlinkLED(LED1);
	}
	differences = ArrayCompare(receive, HOLDreceive, amount, holdArrayamount);
	_delay_cycles(10);
	resetBuffer(receive, 100);
	_delay_cycles(10);
	i=0;
	if(differences > 2)
	{
		P1OUT |= LED0;
		P1OUT &= ~LED1;
	}
	else
	{
		P1OUT |= LED1;
		P1OUT &= ~LED0;
   		send_rf(TX, TXdata);
   		send_rf(TX, TXdata);
   		send_rf(TX, TXdata);
   		send_rf(TX, TXdata);
	}
	ledcount =0;
}
if(timeout >1050)
{
timeout =1050;
}



ledcount++;
if(ledcount == 1000)
{
	P1OUT &= ~(LED0 + LED1);
	_BIS_SR(LPM4_bits + GIE); // Enter LPM3 w/interrupt
}
if(ledcount > 1050)
{
ledcount = 1050;
}


}
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	_BIC_SR(LPM4_EXIT); // wake up from low power mode
    receive[i] = TAR;
    if(i<99)
    {
    i++;
    }

  TA0CTL |= TACLR;
  P1IFG &= ~IRrec; // P1.3 IFG cleared
  P1IFG &= ~IRrec1; // P1.3 IFG cleared
  timeout=0;
}

