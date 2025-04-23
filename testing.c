 #define F_CPU          16000000UL
 #define MAX_NOTE_COUNT 300 
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
 uint8_t SHIFT = 3;
 
 uint16_t freq_from_note(uint8_t note);
 
 typedef struct {
     uint8_t note; // from 60 to 83 (C4 to B5)
     uint16_t start_time;      // Time in tics (10ms units)
     uint8_t duration; // in ds (1/10 of a second)
 } Note; // 4 bytes
 
 Note melody[MAX_NOTE_COUNT]; // 300 * 4 = 1200 bytes
 int melody_idx = 0; // number of notes in the melody
 
 
 // start time array (separate from struct) to account for overlapping notes (mel)
 #define MAX_MIDI_NOTES 128
uint16_t note_start_times[MAX_MIDI_NOTES] = {0};  // 0 means not active

 
 volatile uint32_t tic = 0; // Timer counter for 10ms intervals
 uint16_t note_start_time = 0; // Time when the note started (in ms)
 
 void InitializeTimer() {
     // Configure Timer 1 in CTC mode, prescaler of 64 for 1ms intervals
     TCCR1B |= (1 << WGM12); // CTC mode
     TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
     OCR1A = 2499; // Compare match value for 10ms (16MHz / 64 / 100Hz)
     TIMSK1 |= (1 << OCIE1A)| (1 << OCIE1B); // CompareÂ MatchÂ B; // Enable interrupt on compare match
 }
 
 ISR(TIMER1_COMPA_vect) {
     tic++; // Increment every 10 ms
 }
 
 ISR(TIMER1_COMPB_vect) {
     //empty for now
 }
 
 // stores also the duration, is called when we release the note (mel)
 void register_note(uint8_t note, uint16_t start_time, uint16_t duration) {
//     printf("in reg note: note = %d, start_time = %d, duration = %d", note, start_time, duration);
   if (melody_idx < MAX_NOTE_COUNT) {
       melody[melody_idx].note = note;
       melody[melody_idx].start_time = start_time; // capture when it started
       melody[melody_idx].duration = duration; // duration will be set when note off is received
       melody_idx++;
   }
}
 
 void play_note(uint8_t note) {
     int freq = freq_from_note(note);
     printf("freq: %d\n", freq);
     int ocr_val = (62500) / (2 * freq);
     OCR0A = ocr_val;
     OCR0B = OCR0A / 2;
//     printf("OCR0A Value: %d\n", OCR0A);
     _delay_ms(10);
 }
 
 // helper func to stop note (mel)
 void stop_note() {
    OCR0A = 0;
    OCR0B = 0;
 }
 
 void print_melody(Note *melody, int length) {
    int i = 0;
    while (melody[i].note != 0) { // traverses only non-empty structs so it doesn't visit entire array (mel)
        printf("Note: %d\n", melody[i].note);
        printf("Start time: %d\n", melody[i].start_time);
        printf("Duration: %d\n\n", melody[i].duration);
        i++;
    }
}
 
 void print_note(uint8_t note, uint16_t timestamp, uint16_t duration) {
     printf("Note: %d\n", note);
     printf("Start time: %d\n", timestamp);
     printf("Duration: %d\n", duration);
 }
 
 void play_melody(Note *melody) {
    tic = 0; // reset tic for playback
    int i = 0;
    while (melody[i].note != 0) {
        // Wait until it's time to start this note
        while (tic < melody[i].start_time);  

        uint16_t note_end_time = melody[i].start_time + melody[i].duration;
        while (tic < note_end_time) {
            play_note(melody[i].note);
        }

        stop_note();
        i++;
    }
 }
 
 void clear_melody(Note* melody) {
    int i = 0;
    while (melody[i].note != 0) { // traverses only non-empty structs so it doesn't visit entire array (mel)
        melody[i].note = 0;
        melody[i].start_time = 0;
        melody[i].duration = 0;
        i++;
    }
    
    // reset melody_idx
    melody_idx = 0;
 }
 
 // initializer for buzzer
 
 void InitializePWM() {
     // initialize BUZZER
     DDRD |= (1 << BUZZER);
     PORTD &= ~(1 << BUZZER);
 
     // prescale (divide by 256)
     TCCR0B &= ~(1 << CS00);
     TCCR0B |= (1 << CS02);
     TCCR0B &= ~(1 << CS01);
 
     // set timer 0 to PWM mode, phase correct mode 5
     // (so that it counts up to 0CR0A)
     TCCR0A |= (1 << WGM00);
     TCCR0A &= ~(1 << WGM01);
     TCCR0B |= (1 << WGM02);
 
     //  TIMSK0 |= (1 << OCIE0A);
 
     // toggle OC0A on compare match
     TCCR0A |= (1 << COM0B1);
     TCCR0A |= (1 << COM0B0);
 
     uart_init();
     lcd_init();
//     LCD_setScreen(WHITE);
 }
 
 /* Takes in a MIDI note number and converts it to its frequency */
 uint16_t freq_from_note(uint8_t note) {
     uint8_t shift = (LCD_RAW_NOTE) ? 0 : SHIFT;
     switch (note + 12 + shift) {
         case 48: return 131; // C3
         case 49: return 139; // C#3
         case 50: return 147; // D3
         case 51: return 156; // D#3
         case 52: return 165; // E3
         case 53: return 175; // F3
         case 54: return 185; // F#3
         case 55: return 196; // G3
         case 56: return 208; // G#3
         case 57: return 220; // A3
         case 58: return 233; // A#3
         case 59: return 247; // B3
         case 60: return 262; // C4
         case 61: return 277; // C#4
         case 62: return 294; // D4
         case 63: return 311; // D#4
         case 64: return 330; // E4
         case 65: return 349; // F4
         case 66: return 370; // F#4
         case 67: return 392; // G4
         case 68: return 415; // G#4
         case 69: return 440; // A4
         case 70: return 466; // A#4
         case 71: return 494; // B4
         case 72: return 523; // C5
         case 73: return 554; // C#5
         case 74: return 587; // D5
         case 75: return 622; // D#5
         case 76: return 659; // E5
         case 77: return 698; // F5
         case 78: return 740; // F#5
         case 79: return 784; // G5
         case 80: return 831; // G#5
         case 81: return 880; // A5
         case 82: return 932; // A#5
         case 83: return 988; // B5
         case 84: return 1047; // C6
         case 85: return 1109; // C#6
         case 86: return 1175; // D6
         case 87: return 1245; // D#6
         case 88: return 1319; // E6
         case 89: return 1397; // F6
         case 90: return 1480; // F#6
         case 91: return 1568; // G6
         case 92: return 1661; // G#6
         case 93: return 1760; // A6
         case 94: return 1865; // A#6
         case 95: return 1976; // B6
         case 96: return 2093; // C7
         default: return 0; // Invalid note
     }
 }
 
 
 int main() {
     cli();
     InitializeTimer();
     InitializePWM();
     sei();
     uint8_t status, data1, data2;
     uint8_t current_note;
     
     while(1) {
        status = uart_receive(NULL);
        data1 = uart_receive(NULL);
        data2 = uart_receive(NULL);


        uint8_t command = status & 0xF0; // Mask channel


        if (command == 0x90 && data2 > 0) {

            // set the current note being played
            current_note = data1;
            
            // using C3 as a button to trigger playing the melody (for now)
            if (current_note == 48) {
                printf("recording ended. playing...\n");
//                print_melody(melody, MAX_NOTE_COUNT);
                play_melody(melody);
                clear_melody(melody);
            } 
            else if (current_note == 50) {
                printf("recording started\n");
                tic = 0;
            } 
            else {
                // Record start time for this specific note
                note_start_times[current_note] = tic;  

                // play audio
                play_note(current_note);  
            }
        } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
            if (current_note == data1) {
                stop_note();
            }
//            printf("start time: %d\n", note_start_times[data1]);
            if (note_start_times[data1] > 0 && (data1 != 48 || data1 != 50)) {
                uint16_t duration = tic - note_start_times[data1];
                register_note(data1, note_start_times[data1], duration);
//                print_melody(melody, MAX_NOTE_COUNT);

                note_start_times[data1] = 0;  // Mark note as no longer active
            }
         
        }
     }
 }