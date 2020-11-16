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
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

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
enum states{start, init, on, check, off, wait} state;
//mary had a little lamb
double melody[13] = {329.63, 293.66, 261.63, 293.66, 329.63, 329.63, 329.63, 329.63, 293.66, 293.66, 329.63, 293.66, 261.63};
unsigned short timeHeld[13] = {50, 25, 25, 25, 25, 25, 50, 25, 25, 25, 25, 25, 50};
unsigned short timeBet[13] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}

unsigned char i; 
unsigned char j; 
unsigned char cnt;

void TickFct() {
	switch(state) {
		case start:
			state = init;
			break;
		case init:
			if((PINA & 0x01) == 0x01) {
				state = on;
			}
			else {
				state = init;
			}
			break;
		case on:
			if(j <= timeHeld[i]) {
				state = on;
			}
			else if(j > timeHeld[i]) {
				cnt++;
				if(cnt > 12) {
					state = off;
				}
				else {
					state = check;
					j = 0;
				}
			}
			break;
		case check:
			if(j <= timeBetween[cnt]) {
				state = check;
			}
			else if(j > timeBetween[cnt]){
				state = on;
				i++;
				j = 0;
			}
			break;
		case off:
			state = wait;
			break;
		case wait:
			if((PINA & 0x01) == 0x01) {
				state = wait;
			}
			else {
				state = init;
			}
			break;
		default: break;
	}
	switch(state) {
		case start:
			pwr = 0;
			break;
		case init:
			i = 0;
			j = 0;
			cnt = 0;
			break;
		case on:
			set_PWM(melody[i]);
			j++;
			break;
		case check:
			set_PWM(0);
			j++;
			break;
		case off:
			set_PWM(0);
			break;
		case wait:
			break;
		
		default: break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0X00; PORTA = 0XFF;
	DDRB = 0XFF; PORTB = 0X00;
	/* Insert your solution below */
	
	state = start;
	TimerSet(12);
	TimerOn();
	PWM_on();
	while (1) {
		TickFct();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
