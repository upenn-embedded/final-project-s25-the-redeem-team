#ifndef AUDIO_H
#define AUDIO_H

#include <avr/io.h>
#include <util/delay.h>

#define MAX_NOTE_COUNT 300

typedef struct {
    uint8_t note;
    uint16_t start_time;
    uint8_t duration;
} Note;

extern volatile Note melody[MAX_NOTE_COUNT];
extern volatile int melody_idx;

/**
 * Stops any currently playing note by setting timer compare registers to 0.
 */
void stop_note(void);

/**
 * Calculates the frequency in Hz corresponding to a given MIDI note value,
 * adjusted by the shift_var if mode is 0.
 *
 * @param note MIDI note number (e.g., 60 for middle C)
 * @param mode Determines whether shifting is applied
 * @param shift_var Value to shift the note by if mode is 0
 * @return Frequency in Hz
 */
uint16_t freq_from_note(uint8_t note, volatile uint8_t mode, volatile short int shift_var);

/**
 * Plays a note by configuring Timer0 for the corresponding frequency.
 *
 * @param note MIDI note number to play
 * @param mode Determines whether shifting is applied
 * @param shift_var Value to shift the note by if mode is 0
 */
void play_note(uint8_t note, volatile uint8_t mode, volatile short int shift_var);

/**
 * Registers a note into the melody array with its start time and duration.
 *
 * @param note MIDI note number
 * @param start_time Time at which the note should start
 * @param duration Duration for which the note should play
 */
void register_note(uint8_t note, uint16_t start_time, uint16_t duration);

#endif // AUDIO_H
