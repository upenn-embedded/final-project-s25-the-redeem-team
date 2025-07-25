/* Modified: Adds PCINT on PC0 (RESET) and PC1 (MODE TOGGLE), real-time recording w/ playback delay */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "uart.h"

#define F_CPU 16000000UL
#define MAX_NOTE_COUNT 100
#define BUZZER PD5
#define PC_RESET PC0
#define PC_MODE PD1
#define LED PD1

const uint8_t LCD_RAW_NOTE = 1;

#include <util/delay.h>
#include <xc.h>

volatile short int SHIFT = 0;
volatile uint8_t listening_mode = 0;
volatile uint8_t RESET = 1;
volatile uint8_t prev_PC_state = 0xFF;
volatile uint8_t prev_PD_state = 0xFF;
volatile uint8_t playback_triggered = 0;
volatile uint8_t should_playback = 0;

volatile uint32_t tic = 0;
uint16_t note_start_times[128] = {0};

typedef struct {
    uint8_t note;
    uint16_t start_time;
    uint8_t duration;
} Note;

Note melody[MAX_NOTE_COUNT];
volatile int melody_idx = 0;

void stop_note() { OCR0A = 0; OCR0B = 0; }

void Initialize() {
    uart_init();
    lcd_init();
    
    LCD_setScreen(WHITE);

    // Initialize LED pin
    DDRD |= (1 << LED); 
    PORTD &= ~(1 << LED);   // Start with LED off
}

void InitializeTimer() {
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);
    OCR1A = 2499;
    TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    tic++;
    if (playback_triggered) {
        static uint16_t countdown = 100;
        if (--countdown == 0) {
            should_playback = 1;
            playback_triggered = 0;
            countdown = 100;
        }
    }
}

void InitializePWM() {
    DDRD |= (1 << BUZZER);
    PORTD &= ~(1 << BUZZER);
    TCCR0B &= ~(1 << CS00);
    TCCR0B |= (1 << CS02);
    TCCR0A |= (1 << WGM00);
    TCCR0B |= (1 << WGM02);
    TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
    
}

void InitializePinChangeInterrupts() {
    // PCINT for PC0 and PC1
    DDRC &= ~((1 << PC_RESET) | (1 << PC_MODE));
    PORTC |= (1 << PC_RESET) | (1 << PC_MODE);
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
    prev_PC_state = PINC;

    // PCINT for PD2 and PD3
    DDRD &= ~((1 << PD2) | (1 << PD3));
    PORTD |= (1 << PD2) | (1 << PD3);
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    prev_PD_state = PIND;
}

ISR(PCINT1_vect) {
    uint8_t curr_state = PINC;
    if ((prev_PC_state & (1 << PC_MODE)) && !(curr_state & (1 << PC_MODE))) {
        if (RESET) {
            RESET = 0;
            listening_mode = 1;
            tic = 0;
            PORTD |= (1 << LED);  // Turn on LED
            printf("Switched to RECORDING mode\n");
        } else {
            listening_mode = !listening_mode;
            if (!listening_mode) {
                PORTD &= ~(1 << LED);  // Turn off LED
                printf("Switched to PLAYBACK mode\n");
                playback_triggered = 1;
            } else {
                tic = 0;
                PORTD |= (1 << LED);  // Turn on LED
                printf("Switched to RECORDING mode\n");
            }
        }
    }
    if ((prev_PC_state & (1 << PC_RESET)) && !(curr_state & (1 << PC_RESET))) {
        RESET = 1;
        SHIFT = 0;
        stop_note();
        listening_mode = 0;
        melody_idx = 0;
        playback_triggered = 0;
        should_playback = 0;
        for (int i = 0; i < MAX_NOTE_COUNT; i++) melody[i] = (Note){0};
        for (int i = 0; i < 128; i++) note_start_times[i] = 0;
        tic = 0;
        PORTD &= ~(1 << LED);  // Turn off LED on reset
        printf("RESET activated\n");
    }
    prev_PC_state = curr_state;
}

ISR(PCINT2_vect) {
    uint8_t curr_state = PIND;
    if ((prev_PD_state & (1 << PD3)) && !(curr_state & (1 << PD3))) {
        SHIFT++;
//        printf("SHIFT incremented: %d\n", SHIFT);
    }
    if ((prev_PD_state & (1 << PD2)) && !(curr_state & (1 << PD2))) {
        SHIFT--;
//        printf("SHIFT decremented: %d\n", SHIFT);
    }
    prev_PD_state = curr_state;
}



uint16_t encode_note(int note_index) {
     // Define the 12 semitone notes in an octave
     const char note_chars[] = {
         'C', 'D', 'D', 'E', 'E', 'F', 'G', 'G', 'A', 'A', 'B', 'B' 
     };
 
     const uint8_t note_signs[] = {
         0, 2, 0, 2, 0, 0, 2, 0, 2, 0, 2, 0 
     };
 
     // Wrap note_index to within 0â11
     uint8_t i;
     if (LCD_RAW_NOTE) {
         i = note_index % 12;
     } else {
         i = (note_index + SHIFT) % 12;
     }
 
     if (i < 0) i += 12;
 
     char note_char = note_chars[i];
     uint8_t sign = note_signs[i];
 
     return ((uint16_t) note_char << 8) | sign;
 }

uint16_t freq_from_note(uint8_t note) {
    short int shift;
    if (listening_mode) {
        shift = 0;
    } else {
        shift = SHIFT;
    }
    switch (note + shift) {
        case 48: return 131; case 49: return 139; case 50: return 147; 
        case 51: return 156; case 52: return 165; case 53: return 175; 
        case 54: return 185; case 55: return 196; case 56: return 208; 
        case 57: return 220; case 58: return 233; case 59: return 247; 
        case 60: return 262; case 61: return 277; case 62: return 294;
        case 63: return 311; case 64: return 330; case 65: return 349;
        case 66: return 370; case 67: return 392; case 68: return 415;
        case 69: return 440; case 70: return 466; case 71: return 494;
        case 72: return 523; case 73: return 554; case 74: return 587;
        case 75: return 622; case 76: return 659; case 77: return 698;
        case 78: return 740; case 79: return 784; case 80: return 831;
        case 81: return 880; case 82: return 932; case 83: return 988;
        case 84: return 1047; case 85: return 1109; case 86: return 1175;
        case 87: return 1245; case 88: return 1319; case 89: return 1397; 
        case 90: return 1480; case 91: return 1568; case 92: return 1661; 
        case 93: return 1760; case 94: return 1865;  case 95: return 1976; 
        case 96: return 2093; default: return 0;
    }
}

void play_note(uint8_t note) {
    int freq = freq_from_note(note);
    int ocr_val = (62500) / (2 * freq);
    OCR0A = ocr_val;
    OCR0B = OCR0A / 2;
    _delay_ms(10);
}

void register_note(uint8_t note, uint16_t start_time, uint16_t duration) {
    if (melody_idx < MAX_NOTE_COUNT) {
        melody[melody_idx++] = (Note){note, start_time, duration};
    }
}

void play_melody(Note *melody) {
    tic = 0;
    // LCD graphics
    uint8_t note, sign;
    uint16_t encoded_note;
    for (int i = 0; melody[i].note != 0; i++) {
        while (tic < melody[i].start_time);
        uint16_t end_time = melody[i].start_time + melody[i].duration;
        encoded_note = encode_note(melody[i].note);
        note = encoded_note >> 8;
        sign = encoded_note & (0x0F);
        //        printf("Note: %c\n", note);
        //        printf("Sign: %d\n", sign);

        // draw note on LCD
        LCD_clearScreen();
        LCD_drawMeasure();
        LCD_drawNote(note, sign);
        if (LCD_RAW_NOTE == 0) {
            LCD_drawShift(SHIFT);
        }
//        printf("Note ON: %d\n", data1);
        while (tic < end_time) {
            play_note(melody[i].note);
            
        }
        stop_note();
    }
    for (int i = 0; i < MAX_NOTE_COUNT; i++) melody[i] = (Note){0};
    melody_idx = 0;
}

void process_uart() {
    uint8_t status = uart_receive(NULL);
    uint8_t data1 = uart_receive(NULL);
    uint8_t data2 = uart_receive(NULL);

    uint8_t command = status & 0xF0;
    
    // LCD graphics
    uint8_t note, sign;
    uint16_t encoded_note;

    static uint8_t current_note = 0;

    if (command == 0x90 && data2 > 0) {
        current_note = data1;
        note_start_times[current_note] = tic;
        if (listening_mode) register_note(current_note, tic, 0);
        play_note(current_note);
        
        encoded_note = encode_note(data1);
        note = encoded_note >> 8;
        sign = encoded_note & (0x0F);
//        printf("Note: %c\n", note);
//        printf("Sign: %d\n", sign);
 
        // draw note on LCD
        LCD_clearScreen();
        LCD_drawMeasure();
        LCD_drawNote(note, sign);
        if (LCD_RAW_NOTE == 0) {
            LCD_drawShift(SHIFT);
        }
//        printf("Note ON: %d\n", data1);
    } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
        if (note_start_times[data1] > 0) {
            uint16_t duration = tic - note_start_times[data1];
            for (int i = melody_idx - 1; i >= 0; i--) {
                if (melody[i].note == data1 && melody[i].duration == 0) {
                    melody[i].duration = duration;
                    break;
                }
            }
            note_start_times[data1] = 0;
        }
        if (current_note == data1) {
            stop_note();
        }
    }
}

int main() {
    cli();
    Initialize();
    InitializeTimer();
    InitializePWM();
    InitializePinChangeInterrupts();
    sei();

    stop_note();
    
    while (1) {
        if (RESET) {
            stop_note();
            continue;
        }
        if (should_playback) {
            printf("should playback\n");
            should_playback = 0;
            play_melody(melody);
        }

        if (uart_data_available()) {
            printf("uart data available\n");
            process_uart();
        }
    }
}
