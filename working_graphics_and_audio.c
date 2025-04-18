 /*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 


 #define F_CPU          16000000UL
 #define MAX_NOTE_COUNT 300
 #define BUZZER         PD5
#define SHIFT_DOWN PD2
#define SHIFT_UP PD3


 #include <avr/io.h>
 #include "ST7735.h"
 #include "LCD_GFX.h"
 #include <stdio.h>
 
 
 #include <avr/interrupt.h>
 #include "uart.h"
 
 #include <util/delay.h> 
 #include <xc.h>
 #include "math.h"


const uint8_t LCD_RAW_NOTE = 0;
uint8_t SHIFT = 3;

uint16_t freq_from_note(uint8_t note);

 typedef struct {
     uint8_t note; // from 60 to 83 (C4 to B5)
     uint16_t start_time; // in ds (1/10 of a second)
     uint8_t duration; // in ds (1/10 of a second)
 } Note; // 4 bytes
 
Note melody[MAX_NOTE_COUNT]; // 300 * 4 = 1200 bytes
int melody_idx = 0; // number of notes in the melody

uint16_t encode_note(int note_index) {
     // Define the 12 semitone notes in an octave
     const char note_chars[] = {
         'C',  // 0
         'D',  // 1 (D♭)
         'D',  // 2
         'E',  // 3 (E♭)
         'E',  // 4
         'F',  // 5
         'G',  // 6 (G♭)
         'G',  // 7
         'A',  // 8 (A♭)
         'A',  // 9
         'B',  // 10 (B♭)
         'B'   // 11
     };
 
     const uint8_t note_signs[] = {
         0,  // C
         2,  // D♭
         0,  // D
         2,  // E♭
         0,  // E
         0,  // F
         2,  // G♭
         0,  // G
         2,  // A♭
         0,  // A
         2,  // B♭
         0   // B
     };
 
     // Wrap note_index to within 0–11
     uint8_t i;
     if (LCD_RAW_NOTE) {
        i = note_index % 12;
     } else {
        i = (note_index + SHIFT) % 12;
     }
     
     if (i < 0) i += 12;
 
     char note_char = note_chars[i];
     uint8_t sign = note_signs[i];
 
     return ((uint16_t)note_char << 8) | sign;
 }


void register_note(uint8_t note, uint16_t timestamp) {
    if (melody_idx < MAX_NOTE_COUNT) {
        melody[melody_idx].note = note;
        melody[melody_idx].start_time = timestamp;
        melody[melody_idx].duration = 0; // duration will be set when note off is received
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
//     cli();
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
     lcd_init();
     LCD_setScreen(WHITE);
//     sei();
 }
 
 /* Takes in a MIDI note number and converts it to its frequency */
 uint16_t freq_from_note(uint8_t note) {
     uint8_t shift = (LCD_RAW_NOTE) ? 0 : SHIFT;
      switch (note + 12 + shift) {
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
          case 84:
              return 1047; // C6
          default:
              return 0;
      }
  }
 
 /** 
  * @brief Takes in the transposed melody (in MIDI note # form) and finds the OCR0AB
           values for each note 
 * @param clk_freq clock frequency in MHz
 */

 // call helpers from transpose
 int main() {
    InitializePWM();
    uint8_t status, data1, data2, note, sign;
    uint8_t current_note;
    uint16_t encoded_note;

     while(1) {
        status = uart_receive(NULL);
        data1 = uart_receive(NULL);
        data2 = uart_receive(NULL);
       
        
        uint8_t command = status & 0xF0; // Mask channel
        

        if (command == 0x90 && data2 > 0) {
            
            
            register_note(data1,0);
            
            // set the current note being played
            current_note = data1;
            
            // play audio
            play_note(data1);
            
            // decode note into char and sign
            encoded_note = encode_note(data1);
            note = encoded_note >> 8;
            sign = encoded_note & (0x0F);
            printf("Note: %c\n", note);
            printf("Sign: %d\n", sign);
            
            // draw note on LCD
            LCD_clearScreen();
            LCD_drawMeasure();
            LCD_drawNote(note, sign);
           printf("Note ON: %d\n", data1);
//            
        } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
            printf("Note OFF: %d\n", data1);
            if (current_note == data1) {
                OCR0A = 0;
                OCR0B = 0;
            }
            
        } else {
            printf("Unhandled MIDI message: %02X %02X %02X\n", status, data1, data2);
        }
 
          
         // TODO: what about rhythm?
     }
 }