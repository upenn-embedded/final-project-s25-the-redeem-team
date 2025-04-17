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
 
 
 
 // define note struct
 //typedef struct {
 //    char note;
 //    uint8_t sign;
 //} Note;
 
 //#define CONTROLLER
 //#define PERIPHERAL
 #define MIDI
 
 //void variable_delay_ms(uint16_t ms) {
 //     while (ms--) {
 //         _delay_ms(1);
 //     }
 // }
 //
 //uint16_t freq_from_note(uint8_t note) {
 //     switch (note) {
 //         case 60:
 //             return 262; // C4
 //         case 61:
 //             return 277; // C#4
 //         case 62:
 //             return 294; // D4
 //         case 63:
 //             return 311; // D#4
 //         case 64:
 //             return 330; // E4
 //         case 65:
 //             return 349; // F4
 //         case 66:
 //             return 370; // F#4
 //         case 67:
 //             return 392; // G4
 //         case 68:
 //             return 415; // G#4
 //         case 69:
 //             return 440; // A4
 //         case 70:
 //             return 466; // A#4
 //         case 71:
 //             return 494; // B4
 //         case 72:
 //             return 523; // C5
 //         case 73:
 //             return 554; // C#5
 //         case 74:
 //             return 587; // D5
 //         case 75:
 //             return 622; // D#5
 //         case 76:
 //             return 659; // E5
 //         case 77:
 //             return 698; // F5
 //         case 78:
 //             return 740; // F#5
 //         case 79:
 //             return 784; // G5
 //         case 80:
 //             return 831; // G#5
 //         case 81:
 //             return 880; // A5
 //         case 82:
 //             return 932; // A#5
 //         case 83:
 //             return 988; // B5
 //         default:
 //             return 0;
 //     }
 // }
 //
 //void Initialize()
 //{
 ////	lcd_init();
 //    //DDRD |= (1 << PD1);  // Set PD1 as output
 //    //uart_init();
 ////    LCD_setScreen(WHITE);
 //    
 //    // draw measure
 ////    LCD_drawMeasure();
 //    
 //    // Timer0 setup
 //    // initialize BUZZER
 //    DDRD |= (1 << BUZZER); 
 //    PORTD |= (1 << BUZZER);
 //            
 //    // prescale (divide by 256)
 //    TCCR0B &= ~(1 << CS00);
 //    TCCR0B |= (1 << CS02);
 //    TCCR0B &= ~(1 << CS01); 
 //
 //    // set timer 0 to PWM mode, phase correct mode 5
 //    // (so that it counts up to 0CR0A)
 //    TCCR0A |= (1<<WGM00);
 //    TCCR0A &= ~(1<<WGM01);
 //    TCCR0B |= (1<<WGM02);
 //
 //    // toggle OC0A on compare match
 //    TCCR0A &= ~(1<<COM0B1);
 //    TCCR0A |= (1<<COM0B0);
 //}
 // void process_midi_message(uint8_t status, uint8_t data1, uint8_t data2) {
 //    uint8_t command = status & 0xF0; // Mask channel
 //
 //    if (command == 0x90 && data2 > 0) {
 //        printf("Note ON: %d\n", data1);
 //    } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
 //        printf("Note OFF: %d\n", data1);
 //    } else {
 //        printf("Unhandled MIDI message: %02X %02X %02X\n", status, data1, data2);
 //    }
 //}
 // 
 //uint8_t OCR0B_from_freq(uint16_t freq) {
 //    if (freq == 440) {
 //        return 35;
 //    }
 //    return (uint8_t) ((round(16000000 / (2 * 256 * freq))) / 2); // need to halve it because we're in mode 5
 // }
 // 
 // void handle_speaker(uint16_t frequency, uint8_t duration) {
 //    printf("inputted freq: %d\n", frequency);
 //     OCR0B = OCR0B_from_freq(frequency);
 //    printf("calculated ocr value: %d\n", OCR0B);
 //     variable_delay_ms(duration * 100);
 //    //  PORTD &= ~(1 << BUZZER);
 //     return;
 // }
 //
 //#include <stdint.h>
 //
 //uint16_t encode_note(int note_index) {
 //    // Define the 12 semitone notes in an octave
 //    const char note_chars[] = {
 //        'C',  // 0
 //        'D',  // 1 (D♭)
 //        'D',  // 2
 //        'E',  // 3 (E♭)
 //        'E',  // 4
 //        'F',  // 5
 //        'G',  // 6 (G♭)
 //        'G',  // 7
 //        'A',  // 8 (A♭)
 //        'A',  // 9
 //        'B',  // 10 (B♭)
 //        'B'   // 11
 //    };
 //
 //    const uint8_t note_signs[] = {
 //        0,  // C
 //        2,  // D♭
 //        0,  // D
 //        2,  // E♭
 //        0,  // E
 //        0,  // F
 //        2,  // G♭
 //        0,  // G
 //        2,  // A♭
 //        0,  // A
 //        2,  // B♭
 //        0   // B
 //    };
 //
 //    // Wrap note_index to within 0–11
 //    int i = note_index % 12;
 //    if (i < 0) i += 12;
 //
 //    char note_char = note_chars[i];
 //    uint8_t sign = note_signs[i];
 //
 //    return ((uint16_t)note_char << 8) | sign;
 //}
 //
 //
 //int main() {
 //    Initialize();
 //    uart_init();
 //
 //    uint8_t status, data1, data2, note, sign;
 //    uint16_t encoded_note;
 //
 //    while (1) {
 //        // blocking read of MIDI bytes
 //        status = uart_receive(NULL);
 //        data1 = uart_receive(NULL);
 //        data2 = uart_receive(NULL);
 //        
 //        encoded_note = encode_note(data1);
 //        note = encoded_note << 8;
 //        
 ////        if (data1 == 60) {
 ////            LCD_clearScreen();
 ////            LCD_drawMeasure();
 ////            LCD_drawNote('C', 0);
 ////        }
 //        
 //        process_midi_message(status, data1, data2);
 //
 //        handle_speaker(freq_from_note(data1), 5);
 //        
 //        
 //
 //  
 //    }
 //
 //    return 0;
 //}
 
 
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
 void find_ocr_vals(int* transp_melody, int* ocr_arr, uint8_t transp_len) {
     for (int i = 0; i < transp_len; i++) {
         int freq = freq_from_note(transp_melody[i]);
         int ocr_val = (62500) / (2*freq);
         printf("ocr val before dividing: %d\n", ocr_val);
         ocr_arr[i] = ocr_val; // need to halve it because we're in mode 5
         printf("Freq: %d\n", freq);
     }
     
 }
 void print_melody(int *melody, int length) {
     const char *note_names[12] = {
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
 // call helpers from transpose
 int main() {
     InitializePWM();
     while(1) {
         int melody[] = {60, 61, 62, 63, 64, 65, 66, 67}; // C4 to C5 (C major scale)
         int length = 8;
 //        int currentKey = 60; // C4
 //        int targetKey = 62;  // D4
 
         printf("Original MIDI melody: ");
         print_melody(melody, length);
 
         // transpose_melody(melody, length, currentKey % 12, targetKey % 12);
 
         // printf("Transposed MIDI melody: ");
         // print_melody(melody, length);
 
         // frequency stuff:
         uint8_t transp_len = length;
         int ocr_arr[transp_len];
         
         find_ocr_vals(melody, ocr_arr, transp_len);
         for (int i = 0; i < length; i++) {
             OCR0A = ocr_arr[i];
             OCR0B = OCR0A / 2;
             printf("OCR0A Value: %d\n", OCR0A);
             _delay_ms(500);
         }
 
         // TODO: what about rhythm?
     }
 }