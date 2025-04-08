#include <stdio.h>
#define NOTE_COUNT 12

// transpose entire melody from one key to another
void transpose_melody(int *melody, int length, int currentKey, int targetKey);

// helper func to print the melody
void print_melody(int *melody, int length);