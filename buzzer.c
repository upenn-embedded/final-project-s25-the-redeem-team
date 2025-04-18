/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 


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

// timer variables
volatile uint32_t timer_counter = 0; // Elapsed time in ms
uint32_t note_start_time = 0; // Time when the note started (in ms)



void register_note(uint8_t note, uint16_t timestamp, uint8_t duration) {
    if (melody_length < MAX_NOTE_COUNT) {
        melody[melody_idx].note = note;
        melody[melody_idx].start_time = timestamp;
        melody[melody_idx].duration = duration; // duration will be set when note off is received
        melody_idx++;
    }
}

void play_note(uint8_t note) {
    int freq = freq_from_note(note);
    int ocr_val = (62500) / (2*freq);
    OCR0A = ocr_val;
    OCR0B = OCR0A / 2;
    printf("OCR0A Value: %d\n", OCR0A);
    _delay_ms(10);
}



 // initializer for buzzer
 void InitializePWM() {     
     // initialize BUZZER
     //cli();
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
 
     // TOP value --> MOVE THIS TO MAIN
     // (16MHz/(2*256 prescaler * 440 Hz)) = 71
     // (need to halve this value for mode 5)
     // OCR0A = 35;
     uart_init();
     //sei();
 }

 void InitializeTimer() {
    // Configure Timer 1 in CTC mode, prescaler of 256 for 1ms intervals
    TCCR1B |= (1 << WGM12);      // CTC mode
    TCCR1B |= (1 << CS12);       // Prescaler 256
    OCR1A = 62;                  // Compare match value for 1 ms (16MHz / 256 / 1000)
    TIMSK1 |= (1 << OCIE1A);     // Enable interrupt on compare match
}

ISR(TIMER1_COMPA_vect) {
    timer_counter++; // Increment every 1 ms
}

void play_note_with_duration(uint8_t note, uint16_t duration) {
    // Get the frequency for the note
    int freq = freq_from_note(note);
    int ocr_val = (62500) / (2 * freq);
    
    // Set the PWM to play the note
    OCR0A = ocr_val;
    OCR0B = ocr_val / 2;
    
    // printf("Note ON: %d\n", note);
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
 
 /** 
  * @brief Takes in the transposed melody (in MIDI note # form) and finds the OCR0AB
           values for each note 
 * @param clk_freq clock frequency in MHz
 */
//  void find_ocr_vals(int* transp_melody, int* ocr_arr, uint8_t transp_len) {
//      for (int i = 0; i < transp_len; i++) {
//          int freq = freq_from_note(transp_melody[i]);
//          int ocr_val = (62500) / (2*freq);
//          printf("ocr val before dividing: %d\n", ocr_val);
//          ocr_arr[i] = ocr_val; // need to halve it because we're in mode 5
//          printf("Freq: %d\n", freq);
//      }
     
//  }
//  void print_melody(int *melody, int length) {
//      const char *note_names[12] = {
//          "C", "C#", "D", "D#", "E", "F",
//          "F#", "G", "G#", "A", "A#", "B"
//      };
 
//      for (int i = 0; i < length; i++) {
//          int note = melody[i];
//          int pitch_class = note % 12;
//          int octave = (note / 12) - 1; // MIDI octave system
//          printf("%s%d ", note_names[pitch_class], octave);
//      }
//      printf("\n");
//  }

//  void process_midi_message(uint8_t status, uint8_t data1, uint8_t data2) {
//     uint8_t command = status & 0xF0; // Mask channel

//     if (command == 0x90 && data2 > 0) {
//         printf("Note ON: %d\n", data1);
//         register_note(data1,0,0);
//     } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
//         printf("Note OFF: %d\n", data1);
//     } else {
//         printf("Unhandled MIDI message: %02X %02X %02X\n", status, data1, data2);
//     }
// }
 // call helpers from transpose
 int main() {
     InitializePWM();
     InitializeTimer();
    uint8_t status, data1, data2;

    uint8_t curr_note;

     while(1) {
        status = uart_receive(NULL);
        data1 = uart_receive(NULL);
        data2 = uart_receive(NULL);
        // int melody[] = {data1}; // C4 to C5 (C major scale)
        // int length = 8;
 //        int currentKey = 60; // C4
 //        int targetKey = 62;  // D4
 
        //  printf("Original MIDI melody: ");
        //  print_melody(melody, length);
 
         // transpose_melody(melody, length, currentKey % 12, targetKey % 12);
 
         // printf("Transposed MIDI melody: ");
         // print_melody(melody, length);

        
        // process_midi_message(status, data1, data2);

        uint8_t command = status & 0xF0; // Mask channel

        if (command == 0x90 && data2 > 0) {
            printf("Note ON: %d\n", data1);

            // Record the current time as the start time of the note
            note_start_time = timer_counter;
            play_note(data1);
            curr_note = data1;
        } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
            printf("Note OFF: %d\n", data1);
            if (curr_note == data1) {
                // Calculate the duration the note was pressed
                uint32_t duration = timer_counter - note_start_time;
                register_note(data1,note_start_time,duration);
                // print_melody(melody, MAX_NOTE_COUNT); // for debugging
                
                // stop playing note
                OCR0A = 0;
                OCR0B = 0;

                // replay note for the marked duration
                play_note_with_duration(curr_note, duration);
            }
        } else {
            printf("Unhandled MIDI message: %02X %02X %02X\n", status, data1, data2);
        }
 
         // frequency stuff:
        //  uint8_t transp_len = length;
        //  int ocr_arr[transp_len];
         
        //  find_ocr_vals(melody, ocr_arr, transp_len);
        //  for (int i = 0; i < length; i++) {
        //      OCR0A = ocr_arr[i];
        //      OCR0B = OCR0A / 2;
        //      printf("OCR0A Value: %d\n", OCR0A);
        //      _delay_ms(10);
        //  }
 
         // TODO: what about rhythm?
     }
 }