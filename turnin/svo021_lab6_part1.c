/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section: 021
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * Demo link: https://youtu.be/Ib1z8TlhU_c 
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

enum rotate_states {rotate_init, rotate_zero, rotate_one, rotate_two} rotate_state;

rotate_tick() {
    switch(rotate_state) {
        case(rotate_init):
            rotate_state = rotate_zero;
            break;
        case(rotate_zero):
            rotate_state = rotate_one;
            break;
        case(rotate_one):
            rotate_state = rotate_two;
            break;
        case(rotate_two):
            rotate_state = rotate_zero;
            break;

        default:
            rotate_state = rotate_init;
    }

        switch(rotate_state) {
        case(rotate_init):
            PORTB = 0;
            break;
        case(rotate_zero):
            PORTB = 0x01;
            break;
        case(rotate_one):
            PORTB = 0x02;
            break;
        case(rotate_two):
            PORTB = 0x04;
            break;
        default:
            PORTB = 0;
            break;
    }

}

void main() {
    DDRB = 0xFF;
    PORTB = 0x00;
    TimerSet(1000);
    TimerOn();
    rotate_state = rotate_init;
    while(1) {
        while(!TimerFlag);
        rotate_tick();
        TimerFlag = 0;
    }
}
