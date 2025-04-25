#include "audio.h"

volatile Note melody[MAX_NOTE_COUNT];
volatile int melody_idx = 0;

/* Stops any currently playing note by setting timer compare registers to 0. */
void stop_note() { OCR0A = 0; OCR0B = 0; }

/* Calculates the frequency in Hz corresponding to a given MIDI note value,
 * adjusted by the shift_var if mode is 0. */
uint16_t freq_from_note(uint8_t note, volatile uint8_t mode, volatile short int shift_var) {
    short int shift = mode ? 0 : shift_var;
    switch (note + shift) {
        case 60: return 262; case 61: return 277; case 62: return 294;
        case 63: return 311; case 64: return 330; case 65: return 349;
        case 66: return 370; case 67: return 392; case 68: return 415;
        case 69: return 440; case 70: return 466; case 71: return 494;
        case 72: return 523; case 73: return 554; case 74: return 587;
        case 75: return 622; case 76: return 659; case 77: return 698;
        case 78: return 740; case 79: return 784; case 80: return 831;
        case 81: return 880; case 82: return 932; case 83: return 988;
        default: return 0;
    }
}

/* Plays a note by configuring Timer0 for the corresponding frequency. */
void play_note(uint8_t note, volatile uint8_t mode, volatile short int shift_var) {
    int freq = freq_from_note(note, mode, shift_var);
    int ocr_val = (62500) / (2 * freq);
    OCR0A = ocr_val;
    OCR0B = OCR0A / 2;
    _delay_ms(10);
}

/* Registers a note into the melody array with its start time and duration. */
void register_note(uint8_t note, uint16_t start_time, uint16_t duration) {
    if (melody_idx < MAX_NOTE_COUNT) {
        melody[melody_idx++] = (Note){note, start_time, duration};
    }
}

