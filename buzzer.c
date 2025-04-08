#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h> 
#include <xc.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "math.h"

#include "transpose.h"
 
#define BUZZER PD6

// initializer for buzzer
void InitializePWM() {     
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
    // OCR0A = 35;

}

/* Takes in a MIDI note number and converts it to its frequency */
int midi_to_freq(int note_num) {
    int freq = 440 * pow(2, (note_num - 69) / 12)
    return freq;
}

/** 
 * @brief Takes in the transposed melody (in MIDI note # form) and finds the OCR0A 
          values for each note 
* @param clk_freq clock frequency in MHz
*/
int* find_ocr_vals(int* transp_melody, int clk_freq, int prescaler) {
    int transp_len = sizeof(transp_melody);
    int* ocr_arr[transp_len];
    for (int i = 0; i < transp_len; i++) {
        int freq = midi_to_freq(transp_melody[i]);
        int ocr_val = (clk_freq * pow(10, 6)) / (2*prescaler*freq);
        ocr_arr[i] = ocr_val / 2; // need to halve it because we're in mode 5
    }
    return ocr_arr;
}

// call helpers from transpose
int main() {
    InitializePWM();
    while(1) {
        int melody[] = {60, 62, 64, 65, 67, 69, 71, 72}; // C4 to C5 (C major scale)
        int length = sizeof(melody) / sizeof(melody[0]);
        int currentKey = 60; // C4
        int targetKey = 62;  // D4

        printf("Original MIDI melody: ");
        print_melody(melody, length);

        transpose_melody(melody, length, currentKey % 12, targetKey % 12);

        printf("Transposed MIDI melody: ");
        print_melody(melody, length);

        // frequency stuff:
        int* ocr_vals = find_ocr_vals(melody, 16, 256);
        for (int i = 0; i < sizeof(ocr_vals); i++) {
            OCR0A = ocr_vals[i];
            _delay_ms(500);
        }

        // TODO: what about rhythm?
    }
}