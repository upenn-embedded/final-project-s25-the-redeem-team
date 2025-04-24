/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */

 #define F_CPU          16000000UL
 #define MAX_NOTE_COUNT 5 // lowered it for debugging purposes (mel)
 #define BUZZER         PD5
 
 #include <avr/io.h>
 #include "ST7735.h"
 #include "LCD_GFX.h"
 #include <stdio.h>
 
 #include <avr/interrupt.h>
 #include "uart.h"
 
 #include <util/delay.h> 
 #include <xc.h>
 
 const uint8_t LCD_RAW_NOTE = 0;
 volatile int SHIFT = -1;
 
 int listening_mode = 1;
 int first_time_entering_playback_loop = 1;
 
 // for playback
 int curr_note_num = 0;
 uint16_t curr_note_end_time;
 
 uint16_t freq_from_note(uint8_t note);
 
 typedef struct {
     uint8_t note; // from 60 to 83 (C4 to B5)
     uint16_t start_time; // in 10 ms (1/100 of a second)
     uint8_t duration; // in ds (1/10 of a second)
 } Note; // 4 bytes
 
 Note curr_note;
 Note melody[MAX_NOTE_COUNT]; // 300 * 4 = 1200 bytes
 int melody_idx = 0; // number of notes in the melody
 
 volatile uint32_t tic = 0; // Timer counter for 10ms intervals
 uint16_t note_start_time = 0; // Time when the note started (in ms)
 
 volatile uint8_t prev_PD_state = 0xFF; // For pin change edge detection
 
 void InitializeTimer() {
     TCCR1B |= (1 << WGM12);
     TCCR1B |= (1 << CS11) | (1 << CS10);
     OCR1A = 2499;
     TIMSK1 |= (1 << OCIE1A)| (1 << OCIE1B);
 }
 
 ISR(TIMER1_COMPA_vect) {
     tic++;
 }
 
 ISR(TIMER1_COMPB_vect) {
     // empty for now
 }
 
 void InitializePinChangeInterrupts() {
     DDRD &= ~((1 << PD2) | (1 << PD3));
     PORTD |= (1 << PD2) | (1 << PD3);
     PCICR |= (1 << PCIE2);
     PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
     prev_PD_state = PIND;
 }
 
 ISR(PCINT2_vect) {
     uint8_t curr_state = PIND;
 
     if ((prev_PD_state & (1 << PD3)) && !(curr_state & (1 << PD3))) {
         SHIFT++;
         printf("SHIFT incremented: %d\n", SHIFT);
     }
 
     if ((prev_PD_state & (1 << PD2)) && !(curr_state & (1 << PD2))) {
         SHIFT--;
         printf("SHIFT decremented: %d\n", SHIFT);
     }
 
     prev_PD_state = curr_state;
 }
 
 uint16_t encode_note(int note_index) {
     const char note_chars[] = {'C','D','D','E','E','F','G','G','A','A','B','B'};
     const uint8_t note_signs[] = {0,2,0,2,0,0,2,0,2,0,2,0};
     uint8_t i = (LCD_RAW_NOTE) ? note_index % 12 : (note_index + SHIFT) % 12;
     if (i < 0) i += 12;
     return ((uint16_t) note_chars[i] << 8) | note_signs[i];
 }
 
 void register_note(uint8_t note, uint16_t timestamp, uint16_t duration) {
     if (melody_idx < MAX_NOTE_COUNT) {
         melody[melody_idx].note = note;
         melody[melody_idx].start_time = timestamp;
         melody[melody_idx].duration = duration;
         melody_idx++;
     }
 }
 
 void play_note(uint8_t note) {
     int freq = freq_from_note(note);
     int ocr_val = (62500) / (2 * freq);
     OCR0A = ocr_val;
     OCR0B = OCR0A / 2;
     printf("OCR0A Value: %d\n", OCR0A);
     _delay_ms(10);
 }
 
 void print_melody(Note *melody, int length) {
     for (int i = 0; i < length; i++) {
         printf("Note: %d\n", melody[i].note);
         printf("Start time: %d\n", melody[i].start_time);
         printf("Duration: %d\n\n", melody[i].duration);
     }
 }
 
 void InitializePWM() {
     DDRD |= (1 << BUZZER);
     PORTD &= ~(1 << BUZZER);
 
     TCCR0B &= ~(1 << CS00);
     TCCR0B |= (1 << CS02);
     TCCR0B &= ~(1 << CS01);
 
     TCCR0A |= (1 << WGM00);
     TCCR0A &= ~(1 << WGM01);
     TCCR0B |= (1 << WGM02);
 
     TCCR0A |= (1 << COM0B1);
     TCCR0A |= (1 << COM0B0);
 
     uart_init();
     lcd_init();
 }
 
 uint16_t freq_from_note(uint8_t note) {
     int shift = (LCD_RAW_NOTE) ? 0 : SHIFT;
     switch (note + 12 + shift) {
         case 48: return 131; case 49: return 139; case 50: return 147; case 51: return 156;
         case 52: return 165; case 53: return 175; case 54: return 185; case 55: return 196;
         case 56: return 208; case 57: return 220; case 58: return 233; case 59: return 247;
         case 60: return 262; case 61: return 277; case 62: return 294; case 63: return 311;
         case 64: return 330; case 65: return 349; case 66: return 370; case 67: return 392;
         case 68: return 415; case 69: return 440; case 70: return 466; case 71: return 494;
         case 72: return 523; case 73: return 554; case 74: return 587; case 75: return 622;
         case 76: return 659; case 77: return 698; case 78: return 740; case 79: return 784;
         case 80: return 831; case 81: return 880; case 82: return 932; case 83: return 988;
         case 84: return 1047; case 85: return 1109; case 86: return 1175; case 87: return 1245;
         case 88: return 1319; case 89: return 1397; case 90: return 1480; case 91: return 1568;
         case 92: return 1661; case 93: return 1760; case 94: return 1865; case 95: return 1976;
         case 96: return 2093; default: return 0;
     }
 }
 
 int main() {
     cli();
     InitializeTimer();
     InitializePWM();
     InitializePinChangeInterrupts();
     sei();
 
     uint8_t status, data1, data2, note, sign;
     uint8_t current_note;
     uint16_t encoded_note;
 
     while (1) {
         if (listening_mode) {
             status = uart_receive(NULL);
             data1 = uart_receive(NULL);
             data2 = uart_receive(NULL);
 
             uint8_t command = status & 0xF0;
 
             if (command == 0x90 && data2 > 0) {
                 current_note = data1;
                 note_start_time = tic;
                 play_note(data1);
             } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
                 if (current_note == data1) {
                     uint16_t duration = tic - note_start_time;
                     register_note(data1,note_start_time,duration);
                     print_melody(melody, MAX_NOTE_COUNT);
                     OCR0A = 0;
                     OCR0B = 0;
                 }
             }
         } else {
             if (first_time_entering_playback_loop) {
                 tic = 0;
                 first_time_entering_playback_loop = 0;
                 curr_note_num = 0;
                 curr_note = melody[0];
             }
 
             if (curr_note_num >= melody_idx) continue;
 
             if (curr_note_end_time < tic) {
                 curr_note_num++;
                 curr_note = melody[curr_note_num];
                 curr_note_end_time = curr_note.start_time + curr_note.duration;
                 continue;
             } else if (curr_note.start_time > tic) {
                 continue;
             } else {
                 play_note(curr_note.note);
             }
         }
     }
 }
 