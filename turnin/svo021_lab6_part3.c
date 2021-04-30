/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section: 021
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo link: https://youtu.be/-m6iHqFt0t8
 */
#include <avr/io.h>
#include<avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum counter_states {counter_init, counter_add, counter_inc, counter_sub, counter_dec, counter_reset} counter_state;
unsigned char tmpA = 0;
unsigned char hold_timer = 0;

void counter_tick() {
    switch(counter_state) {
        case counter_init:
        // checks with path to take
            if ((tmpA & 0x03) == 0x00) counter_state = counter_init;
            else if ((tmpA & 0x03) == 0x01) counter_state = counter_add;
            else if ((tmpA & 0x03) == 0x02) counter_state = counter_sub;
            else counter_state = counter_reset;
            break;
        case counter_add:
        // this state is just for the increment only checking for PORTB current value
            counter_state = counter_inc;
            break;
        case counter_inc:
        // if we are still holding, then we increment the tick counter, which is then used to determine whether we increment PORTB or not
            if ((tmpA & 0x03) == 0x00) counter_state = counter_init;
            else if ((tmpA & 0x03) == 0x01) {
                counter_state = counter_inc;
                hold_timer++;
            }
            else if ((tmpA & 0x03) == 0x02) counter_state = counter_init;
            else counter_state = counter_reset;
            break;
        case counter_sub:
        // same as counter_add
            counter_state = counter_dec;
            break;
        case counter_dec:
        // same as counter_inc
            if ((tmpA & 0x03) == 0x00) counter_state = counter_init;
            else if ((tmpA & 0x03) == 0x01) counter_state = counter_init;
            else if ((tmpA & 0x03) == 0x02) {
                counter_state = counter_dec;
                hold_timer++;
            }
            else counter_state = counter_reset;
            break;
        case counter_reset:
        // waits for both buttons to be let go to account for human inconsistency
            if ((tmpA & 0x03) == 0x00) counter_state = counter_init;
            else counter_state = counter_reset;
            break;
        default: 
            counter_state = counter_init;
    }

    switch(counter_state) {
        case counter_init:
            break;
        case counter_add:
            if (PORTB < 9) {
                PORTB = PORTB + 1;
            }
            break;
        case counter_inc:
        // checks for 9 because we use 1 tick to get here
            if (hold_timer >= 9) {
                hold_timer = 0;
                if (PORTB < 9) {
                    PORTB = PORTB + 1;
                }
            }
            break;
        case counter_sub:
            if (PORTB > 0) {
                PORTB = PORTB - 1;
            }
            break;
        case counter_dec:
        // checks for 9 because we use 1 tick to get here
            if (hold_timer >= 9) {
                hold_timer = 0;
                if(PORTB > 0) {
                    PORTB = PORTB - 1;
                }
            }
            break;

        case counter_reset:
            PORTB = 0x00;
            hold_timer = 0;
            break;
        default: 
            PORTB = 0;
            hold_timer = 0;
    }

}

void main() {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(100);
    TimerOn();
    counter_state = counter_init;
    PORTB = 0x07;
    while(1) {
        tmpA = ~PINA;
        while(!TimerFlag);
        counter_tick();
        TimerFlag = 0;
    }
}
