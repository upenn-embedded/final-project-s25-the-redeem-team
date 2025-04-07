#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h> 
#include <xc.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "transpose.h"
 
#define BUZZER PD6

// initializer for buzzer
void Initialize() {     
    // initialize BUZZER
    DDRD |= (1 << BUZZER); 
    PORTD |= (1 << BUZZER);
            
    // prescale (divide by 256)
    TCCR0B &= ~(1 << CS00);
    TCCR0B |= (1 << CS02);
    TCCR0B &= ~(1 << CS01); 

    // set timer 0 to PWM mode, phase correct mode 5
    // (so that it counts up to 0CR0A)
    TCCR0A |= (1<<WGM00);
    TCCR0A &= ~(1<<WGM01);
    TCCR0B |= (1<<WGM02);

    // toggle OC0A on compare match
    TCCR0A &= ~(1<<COM0A1);
    TCCR0A |= (1<<COM0A0);

    // TOP value --> MOVE THIS TO MAIN
    // (16MHz/(2*256 prescaler * 440 Hz)) = 71
    // (need to halve this value for mode 5)
    OCR0A = 35;

}

// call helpers from transpose
int main() {

}