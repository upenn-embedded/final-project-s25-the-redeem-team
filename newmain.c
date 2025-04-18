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
#include "math.h"


typedef struct {
   uint8_t note; // from 60 to 83 (C4 to B5)
   uint16_t start_time; // in ds (1/10 of a second)
   uint8_t duration; // in ds (1/10 of a second)
} Note; // 4 bytes

Note melody[MAX_NOTE_COUNT]; // 300 * 4 = 1200 bytes
int melody_idx = 0; // number of notes in the melody

volatile uint32_t tic = 0; // Timer counter for 10ms intervals

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
   TCCR0A |= (1<<WGM00);
   TCCR0A &= ~(1<<WGM01);
   TCCR0B |= (1<<WGM02);
   
   TIMSK0 |= (1 << OCIE0A);

   // toggle OC0A on compare match
   TCCR0A |= (1<<COM0B1);
   TCCR0A |= (1<<COM0B0);

   uart_init();
}

void InitializeTimer() {
   cli();

   // Configure Timer 1 in CTC mode, prescaler of 256 for 1ms intervals
   TCCR1B |= (1 << WGM12);      // CTC mode
   TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 256
   OCR1A = 2499;                  // Compare match value for 10 ms
   TIMSK1 |= (1 << OCIE1A);     // Enable interrupt on compare match

   sei();                       // Enable global interrupts
}

ISR(TIMER1_COMPA_vect) {
   timer_counter++; // Increment every 1 ms
}

/* Takes in a MIDI note number and converts it to its frequency */
uint16_t freq_from_note(uint8_t note) {
   switch (note) {
       case 60:
           return 262; // C4
       case 61:
           return 277; // C#4
       case 62:
           return 294; // D4
       case 63:
           return 311; // D#4
       case 64:
           return 330; // E4
       case 65:
           return 349; // F4
       case 66:
           return 370; // F#4
       case 67:
           return 392; // G4
       case 68:
           return 415; // G#4
       case 69:
           return 440; // A4
       case 70:
           return 466; // A#4
       case 71:
           return 494; // B4
       case 72:
           return 523; // C5
       case 73:
           return 554; // C#5
       case 74:
           return 587; // D5
       case 75:
           return 622; // D#5
       case 76:
           return 659; // E5
       case 77:
           return 698; // F5
       case 78:
           return 740; // F#5
       case 79:
           return 784; // G5
       case 80:
           return 831; // G#5
       case 81:
           return 880; // A5
       case 82:
           return 932; // A#5
       case 83:
           return 988; // B5
       default:
           return 0;
   }
}


bool register_note(uint8_t note, uint16_t timestamp, uint8_t on_off) {
    if (on_off) {
        // Register note on
        if (melody_length < MAX_NOTE_COUNT) {
            melody[melody_length].note = note;
            melody[melody_length].start_time = timestamp;
            melody[melody_length].duration = 0; // duration will be set when note off is received
            melody_length++;
            return true;
        } else {
            return false; // Melody array is full
        }
    } else {
        // Register note off
        for (int i = melody_length - 1; i >= 0; i++) {
            if (melody[i].note == note && melody[i].duration == 0) {
                melody[i].duration = timestamp - melody[i].start_time;
                return true;
            }
        }
    }
    return false; // Note not found or already off
}

void play_note(uint8_t note) {
   int freq = freq_from_note(note);
   int ocr_val = (62500) / (2*freq);
   OCR0A = ocr_val;
   OCR0B = OCR0A / 2;
   printf("OCR0A Value: %d\n", OCR0A);
   _delay_ms(10);
}
    
void print_melody(Note *melody, int length) {
   for (int i = 0; i < length; i++) {
       printf("Note: %d\n", melody[i].note);
       printf("Start time: %d\n", melody[i].note);
       printf("Duration: %d\n\n", melody[i].duration);
   }
}

int main() {
   InitializePWM();
   InitializeTimer();

   uint8_t status, note, velocity;
   uint8_t curr_note;

   while(1) {
       status = uart_receive(NULL);
       note = uart_receive(NULL);
       velocity = uart_receive(NULL);

       uint8_t command = status & 0xF0; // Mask channel

       register_note(note, tic, command); // Register note on

       if (command == 0x90 && velocity > 0) {
           printf("Note ON: %d\n", note);

           // Record the current time as the start time of the note
           play_note(note);
           curr_note = note;
       } else if ((command == 0x80) || (command == 0x90 && velocity == 0)) {
           printf("Note OFF: %d\n", note);
           if (curr_note == note) {
               // Calculate the duration the note was pressed
               print_melody(melody, MAX_NOTE_COUNT); // check if duration was logged
               
               // stop playing note
               OCR0A = 0;
               OCR0B = 0;
           }
       } else {
           printf("Unhandled MIDI message: %02X %02X %02X\n", status, note, velocity);
       }
    }
}