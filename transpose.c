#include "transpose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


// Transpose a melody of MIDI notes from one key to another
void transpose_melody(int *melody, int length, int currentKey, int targetKey) {
    int shift = (targetKey - currentKey);
    for (int i = 0; i < length; i++) {
        melody[i] += shift;
    }
}

// Helper to print MIDI notes as note names (C4, D#4, etc.)
void print_melody(int *melody, int length) {
    const char *note_names[NOTE_COUNT] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    for (int i = 0; i < length; i++) {
        int note = melody[i];
        int pitch_class = note % 12;
        int octave = (note / 12) - 1; // MIDI octave system
        printf("%s%d ", note_names[pitch_class], octave);
    }
    printf("\n");
}





// Example usage
int main() {






    int melody[] = {60, 62, 64, 65, 67, 69, 71, 72}; // C4 to C5 (C major scale)
    int length = sizeof(melody) / sizeof(melody[0]);
    int currentKey = 60; // C4
    int targetKey = 62;  // D4

    printf("Original MIDI melody: ");
    print_melody(melody, length);

    transpose_melody(melody, length, currentKey % 12, targetKey % 12);

    printf("Transposed MIDI melody: ");
    print_melody(melody, length);

    return 0;
}

