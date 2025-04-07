#include "transpose.h"

// notes are represented as integers from 0 to 11 (C=0, C#=1, ..., B=11)
// melody is an array of integers in that format

// transpose one note
int transpose_note(int note, int shift) {
    return (note + shift + NOTE_COUNT) % NOTE_COUNT;
}

// transpose entire melody from one key to another
void transpose_melody(int *melody, int length, int curr_key, int target_key) {
    int shift = (target_key - curr_key + NOTE_COUNT) % NOTE_COUNT;
    for (int i = 0; i < length; i++) {
        melody[i] = transpose_note(melody[i], shift);
    }
}

// helper func to print the melody
void print_melody(int *melody, int length) {
    const char *note_names[NOTE_COUNT] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    for (int i = 0; i < length; i++) {
        printf("%s ", note_names[melody[i]]);
    }
    printf("\n");
}

int main() {
    int melody[] = {0, 2, 4, 5, 7, 9, 11}; // C major scale
    int length = sizeof(melody) / sizeof(melody[0]);
    int curr_key = 0; // C
    int target_key = 2;  // D

    printf("Original melody: ");
    print_melody(melody, length);

    transpose_melody(melody, length, curr_key, target_key);

    printf("Transposed melody: ");
    print_melody(melody, length);

    return 0;
}
