#include <avr/interrupt.h>
#include <stdio.h>
#include <xc.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "uart.h"
#include "audio.h"

#define F_CPU 16000000UL
#define BUZZER PD5
#define PC_RESET PC0
#define PC_MODE PC1

volatile short int SHIFT = 0;
volatile uint8_t listening_mode = 0;
volatile uint8_t RESET = 1;
volatile uint8_t prev_PC_state = 0xFF;
volatile uint8_t prev_PD_state = 0xFF;
volatile uint8_t playback_triggered = 0;
volatile uint8_t should_playback = 0;

volatile uint32_t tic = 0;
uint16_t note_start_times[128] = {0};

// ----------------------------------------------------------
//  TIMER SETUP
// ----------------------------------------------------------

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

// ----------------------------------------------------------
//  PWM SETUP
// ----------------------------------------------------------

void InitializePWM() {
    DDRD |= (1 << BUZZER);
    PORTD &= ~(1 << BUZZER);
    TCCR0B &= ~(1 << CS00);
    TCCR0B |= (1 << CS02);
    TCCR0A |= (1 << WGM00);
    TCCR0B |= (1 << WGM02);
    TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
}

// ----------------------------------------------------------
//  BUTTON SETUP
// ----------------------------------------------------------

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
            printf("Switched to RECORDING mode\n");
        } else {
            listening_mode = !listening_mode;
            if (!listening_mode) {
                printf("Switched to PLAYBACK mode\n");
                playback_triggered = 1;
            } else {
                tic = 0;
                printf("Switched to RECORDING mode\n");
            }
        }
    }
    if ((prev_PC_state & (1 << PC_RESET)) && !(curr_state & (1 << PC_RESET))) {
        RESET = 1;
        listening_mode = 0;
        melody_idx = 0;
        playback_triggered = 0;
        should_playback = 0;
        for (int i = 0; i < MAX_NOTE_COUNT; i++) melody[i] = (Note){0};
        for (int i = 0; i < 128; i++) note_start_times[i] = 0;
        tic = 0;
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

// ----------------------------------------------------------
//  OUTPUT AND UART FUNCS
// ----------------------------------------------------------

void play_melody(Note *melody, volatile uint8_t mode) {
    tic = 0;
    for (int i = 0; melody[i].note != 0; i++) {
        while (tic < melody[i].start_time);
        uint16_t end_time = melody[i].start_time + melody[i].duration;
        while (tic < end_time) play_note(melody[i].note, mode, SHIFT);
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

    static uint8_t current_note = 0;

    if (command == 0x90 && data2 > 0) {
        current_note = data1;
        note_start_times[current_note] = tic;
        if (listening_mode) register_note(current_note, tic, 0);
        play_note(current_note, listening_mode, SHIFT);
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

// ----------------------------------------------------------
//  MAIN()
// ----------------------------------------------------------
int main() {
    cli();
    InitializeTimer();
    InitializePWM();
    uart_init();
    lcd_init();
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
            play_melody(melody, listening_mode);
        }

        if (uart_data_available()) {
            printf("uart data available\n");
            process_uart();
        }
    }
}
