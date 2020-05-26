/*
 * GccApplication1.c
 *
 * Created: 2019/10/13 22:14:42
 * Author : Penkich
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#include <stdlib.h>
//#include <math.h>

const int top0 = 0xf9;
const int top1 = 0x07c9;

float duty0 = 0.7;
float duty1 = 0.8;
float duty2 = 0.9;

void port_init(void)
{
	PORTB = 0xff;
	DDRB  = 0xff;
	PORTC = 0xff;
	DDRC  = 0xff;
	PORTD = 0xff;
	DDRD  = 0xff;
}

void timer0_init(void)
{
	TCCR0B = 0x00;
	TCNT0  = 0x01;
	OCR0A  = top0; // freq 1khz = f9
	OCR0B  = top0 * duty0; // pulse width control
	TCCR0A = 0b00110011;
	TCCR0B = 0b01001011;
}

void timer1_init(void)
{
	TCCR1B = 0x00;
	TCNT1H  = 0x00;
	TCNT1L  = 0x00;
	OCR1AH  = 0x00;
	OCR1AL  = 0x00; 
	OCR1B =  top1 * duty1;
	TCCR1A = 0b00110010;
	TCCR1B = 0b01011010;
	ICR1 = top1;
}

void timer2_init(void)
{
	TCCR2B = 0x00;
	TCNT2  = 0x01;
	OCR2A  = top0; // freq
	OCR2B  = top0 * duty2; // pulse width control
	TCCR2A = 0b00110011;
	TCCR2B = 0b01001100; // pre-scaler 1/64 diff from timer0
}



void init_devices(void)
{
	cli();
	port_init();
	timer0_init();
	timer1_init();
	timer2_init();
	
	MCUCR = 0x00;
	EICRA = 0x00;
	EIMSK = 0x00;
	
	TIMSK0 = 0x00;
	TIMSK1 = 0x00;
	TIMSK2 = 0x00;
	
	PCMSK0 = 0x00;
	PCMSK1 = 0x00;
	PCMSK2 = 0x00;
	
	PCICR = 0x00;
	PRR = 0x00;
	sei();
}

void main(void)
{
	init_devices();
	while(1)
        _delay_ms(100);	
	;
}

