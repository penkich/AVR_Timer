/*
 * adc_test1.c
 *
 * Created: 2019/10/15 7:47:49
 * Author : penkich
 * ATMEGA328で、パルス幅可変、周期1msのpwmを３つ作る。
 *
 */ 

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

// Voltage Reference Selections
#define ADC_REFS_AREFF  0x00                        // 00
#define ADC_REFS_AVCC   (_BV(REFS0))                // 01
#define ADC_REFS_IN     (_BV(REFS1) | _BV(REFS0))   // 11

// Input Channel Selections
#define ADC_MUX_ADC0    0x00                    // 0000
#define ADC_MUX_ADC1    (_BV(MUX0))             // 0001
#define ADC_MUX_ADC2    (_BV(MUX1))             // 0010
#define ADC_MUX_ADC3    (_BV(MUX1) | _BV(MUX0)) // 0011
#define ADC_MUX_ADC4    (_BV(MUX2))             // 0100
#define ADC_MUX_ADC5    (_BV(MUX2) | _BV(MUX0)) // 0101
#define ADC_MUX_VBG     (_BV(MUX3) | _BV(MUX2) | _BV(MUX1))             // 1110
#define ADC_MUX_GND     (_BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0)) // 1111

// ADC Prescaler Selections(Division Factor)
#define ADC_ADPS_2      (_BV(ADPS0))                            // 001
#define ADC_ADPS_4      (_BV(ADPS1))                            // 010
#define ADC_ADPS_8      (_BV(ADPS1) | _BV(ADPS0))               // 011
#define ADC_ADPS_16     (_BV(ADPS2))                            // 100
#define ADC_ADPS_32     (_BV(ADPS2) | _BV(ADPS0))               // 101
#define ADC_ADPS_64     (_BV(ADPS2) | _BV(ADPS1))               // 110
#define ADC_ADPS_128    (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS1))  // 111

#endif  // ADC_H_INCLUDED

#ifndef F_CPU
#define F_CPU 16000000UL    // 動作周波数に16MHzを指定
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

const int top0 = 0xf9;
const int top1 = 0x07c9;

void port_init(void)
{
	PORTB = 0xff;
	DDRB  = 0xff;
	PORTC = 0x00;
	DDRC  = 0x00;
	PORTD = 0xff;
	DDRD  = 0xff;
}

void timer0_init(void)
{
	TCCR0B = 0x00;
	TCNT0  = 0x01;
	OCR0A  = top0; // freq 1khz = f9
//	OCR0B  = top0 * duty; // pulse width control
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
//	OCR1B =  top1 * duty;
	TCCR1A = 0b00110010;
	TCCR1B = 0b01011010;
	ICR1 = top1;
}

void timer2_init(void)
{
	TCCR2B = 0x00;
	TCNT2  = 0x01;
	OCR2A  = top0; // freq
//	OCR2B  = top0 * duty; // pulse width control
	TCCR2A = 0b00110011;
	TCCR2B = 0b01001100; // pre-scaler 1/64 diff from timer0
}

void init_devices(void)
{
	cli();
	port_init();
	
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

int main(void)
{
    init_devices();

	timer0_init();
	timer1_init();
	timer2_init();
	
	while(1)
	{
	    ADMUX =  ADC_REFS_AVCC | ADC_MUX_ADC0;
	    ADCSRA = _BV(ADEN) | ADC_ADPS_128;
	    DIDR0 |= _BV(ADC0D);

		// ADCスタート
		ADCSRA |= _BV(ADIF);
		ADCSRA |= _BV(ADSC);
		
		// ADC完了待ち
		loop_until_bit_is_set(ADCSRA, ADIF);
		
		// ADC結果取得
		float result0 = ADC;

	    ADMUX =  ADC_REFS_AVCC | ADC_MUX_ADC1;
	    ADCSRA = _BV(ADEN) | ADC_ADPS_128;
	    DIDR0 |= _BV(ADC1D);

		// ADCスタート
		ADCSRA |= _BV(ADIF);
		ADCSRA |= _BV(ADSC);
		
		// ADC完了待ち
		loop_until_bit_is_set(ADCSRA, ADIF);
		
		// ADC結果取得
		float result1 = ADC;

    	ADMUX =  ADC_REFS_AVCC | ADC_MUX_ADC2;
	    ADCSRA = _BV(ADEN) | ADC_ADPS_128;
	    DIDR0 |= _BV(ADC2D);

		// ADCスタート
		ADCSRA |= _BV(ADIF);
		ADCSRA |= _BV(ADSC);
		
		// ADC完了待ち
		loop_until_bit_is_set(ADCSRA, ADIF);
		
		// ADC結果取得
		float result2 = ADC;
        OCR0B =  top0 * result0 / 1024.0;
        OCR1B =  top1 * result1 / 1024.0;
        OCR2B =  top0 * result2 / 1024.0;

		_delay_ms(10);	
	}
}

