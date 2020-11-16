/*	Author: Alyssa Zepeda
 *  Partner(s) Name: 
 *	Lab Section:024
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if(frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0X08;}
		else {TCCR3B |= 0X03;}

		if(frequency < 0.954) {OCR3A = 0xFFFF;}
		else if(frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short)(8000000 / (128 * frequency)) -1;}

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

//////////////////////////////////////////////////////////////////
enum states{start, init, inc, dec, wait, toggle} state;
double frequency[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

unsigned char freqPos = 0x00;
unsigned char pwr;
double currFreq;

void TickFct() {
	switch(state) {
		case start:
			state = init;
			break;
		case init:
			if((PINA & 0x07) == 0x01) {
				state = toggle;
			}
			else if((PINA & 0x07) == 0x02) {
				state = inc;
			}
			else if((PINA & 0x07) == 0x04) {
				state = dec;
			}
			else {
				state = init;
			}
			break;
		case inc:
			state = wait;
			break;
		case dec:
			state = wait;
			break;
		case toggle:
			state = wait;
			break;
		case wait:
			if((PINA & 0x07) == 0) {
				state = init;
			}
			else {
				state = wait;
			}
			break;
		default: break;
	}
	switch(state) {
		case start:
			pwr = 0;
			break;
		case init:
			break;
		case inc:
			if((freqPos + 1) < 0x07) {
				freqPos++;
			}
			else {
				freqPos = 0x07;
			}
			break;
		case dec:
			if((freqPos - 1) > 0) {
				freqPos--;
			}
			else {
				freqPos = 0;
			}
			break;
		case toggle:
			if(pwr == 0) {
				PWM_on();
				pwr = 1;
			}
			else if(pwr == 1) {
				PWM_off();
				pwr = 0;
			}
			break;
		case wait:
			currFreq = frequency[freqPos];
			set_PWM(currFreq);
			break;
		default: break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0X00; PORTA = 0XFF;
	DDRB = 0XFF; PORTB = 0X00;
	/* Insert your solution below */
	pwr = 0;
	state = start;
	while (1) {
		TickFct();
	}
    return 1;
}
