/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section: 021
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo link: https://youtu.be/-Z7QrS0koyA
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

enum rotate_states {rotate_init, rotate_zero, rotate_one_front, rotate_one_back, rotate_two, rotate_freeze, rotate_freeze_wait, rotate_reset} rotate_state;
unsigned char tmpA = 0;
unsigned char press_flag = 0;

void rotate_tick() {
    switch(rotate_state) {
        case(rotate_init):
            rotate_state = rotate_zero;
            break;
        case(rotate_zero):
            if((tmpA & 0x01) == 0x01) {
                rotate_state = rotate_freeze;
            }
            else rotate_state = rotate_one_front;
            break;
        case(rotate_one_front):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_freeze;
            else rotate_state = rotate_two;
            break;
        case(rotate_one_back):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_freeze;
            else rotate_state = rotate_zero;
            break;
        case(rotate_two):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_freeze;
            else rotate_state = rotate_one_back;
            break;
        case(rotate_freeze):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_freeze;
            else rotate_state = rotate_freeze_wait;
            break;
        case(rotate_freeze_wait):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_reset;
            else rotate_state = rotate_freeze_wait;
            break;
        case(rotate_reset):
            if((tmpA & 0x01) == 0x01) rotate_state = rotate_reset;
            else rotate_state = rotate_zero;
            break;
        default:
            rotate_state = rotate_init;
    }

        switch(rotate_state) {
        case(rotate_init):
            PORTB = 0x01;
            break;
        case(rotate_zero):
            PORTB = 0x01;
            break;
        case(rotate_one_front):
        case(rotate_one_back):
            PORTB = 0x02;
            break;
        case(rotate_two):
            PORTB = 0x04;
            break;
        case(rotate_freeze):
        case(rotate_freeze_wait):
        case(rotate_reset):
            break;
        default:
            PORTB = 0x01;
            break;
    }

}

void main() {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(300);
    TimerOn();
    rotate_state = rotate_init;
    while(1) {
        tmpA = ~PINA;
        while(!TimerFlag);
        rotate_tick();
        TimerFlag = 0;
    }
}
