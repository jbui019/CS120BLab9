/*	Author: jbui019
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){
		if(!frequency){
			TCCR3B &= 0x08;
		}
		else{
			TCCR3B |= 0x03;
		}
		if(frequency < 0.954){
			OCR3A = 0xFFFF;
		}
		else if(frequency > 31250){
			OCR3A = 0x0000;
		}
		else{
			OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		}

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
enum States{start, init, up, down, wait, toggle}state;
double array[9] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0};
double temp = 0;
unsigned char position = 0x00;
unsigned char clicker = 0x00;
unsigned char temp2 = 0x00;
unsigned char buttons;
unsigned char button1;
unsigned char button2;
unsigned char button3;
void tick(){
	switch(state){//transitions
		case start:
			state = init;
			break;
		case init:
			if(buttons == 0x01){
				state = up;
			}
			else if(buttons == 0x02){
				state = down;
			}
			else if(buttons == 0x04){
				state = toggle;
			}
			else{
				state = init;
			}
			break;

		case up:
			temp = array[position];
			state = wait;
			break;

		case down:
			temp = array[position];
			state = wait;
			break;
		case wait:
			if(buttons == 0x00){
				state = init;
			}
			else{
				state = wait;
			}
			break;

		case toggle:
			state = wait;
			break;

		default:
			break;
	}

	switch(state){//actions
		case start:
			break;
		case init:
			break;

		case up:
			if((position + 1) > 0x07){
				position = 0x07;
			}
			else{
				position++;
			}
			break;
		case down:
			if((position - 1) < 0x00){
				position = 0x00;
			}
			else{
				position--;
			}
			break;
		case wait:
			temp = array[position];
			set_PWM(temp);
			break;

		case toggle:
			if(clicker){
				PWM_off();
				temp2 = position;
				position = 8;
				clicker = 0;
			}
			else{
				PWM_on();
				position = temp2;
				clicker = 1;
			}
			break;

		default:
			break;

	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = start;
	PWM_on();
	clicker = 1;
    /* Insert your solution below */
    while (1) {
	buttons = (~PINA & 0x07);
	button1 = (~PINA & 0x01);
	button2 = ((~PINA & 0x02) >> 1);
	button3 = ((~PINA & 0x04) >> 2);
	tick();
    }
    return 1;
}
