#define F_CPU 16000000UL

 #include <avr/io.h>
 #include <stdlib.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 
 #include <math.h>
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #define F_CPU          16000000UL
 #define MAX_NOTE_COUNT 300
 #define BUZZER         PD5
 
 typedef struct {
     uint8_t note; // from 60 to 83 (C4 to B5)
     uint16_t start_time; // in ds (1/10 of a second)
     uint8_t duration; // in ds (1/10 of a second)
 } Note; // 4 bytes
 
 Note melody[MAX_NOTE_COUNT]; // 300 * 4 = 1200 bytes
 
 int melody_length = 0; // number of notes in the melody
 
 bool listening_mode = 0; // 1 = listening to melody, 0 = playing melody
 
 
 void variable_delay_ms(uint16_t ms) {
     while (ms--) {
         _delay_ms(1);
     }
 }
 
 bool register_note(uint8_t note, uint16_t timestamp, uint8_t on_off) {
     if (on_off) {
         // Register note on
         if (melody_length < MAX_NOTE_COUNT) {
             melody[melody_length].note = note;
             melody[melody_length].start_time = timestamp;
             melody[melody_length].duration = 0; // duration will be set when note off is received
             melody_length++;
             return true;
         } else {
             return false; // Melody array is full
         }
     } else {
         // Register note off
         for (int i = melody_length - 1; i >= 0; i++) {
             if (melody[i].note == note && melody[i].duration == 0) {
                 melody[i].duration = timestamp - melody[i].start_time;
                 return true;
             }
         }
     }
     return false; // Note not found or already off
 }
 
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
 
 bool Initialize() {
    //  melody[0] = (Note){64, 0, 5}; // mi (E4)
    //  melody[1] = (Note){64, 5, 5}; // mi (E4)
    //  melody[2] = (Note){65, 10, 5}; // fa (F4)
    //  melody[3] = (Note){67, 15, 5}; // sol (G4)
    //  melody[4] = (Note){67, 20, 5}; // sol (G4)
    //  melody[5] = (Note){65, 25, 5}; // fa (F4)
    //  melody[6] = (Note){64, 30, 5}; // mi (E4)
    //  melody[7] = (Note){62, 35, 5}; // re (D4)
    //  melody[8] = (Note){60, 40, 5}; // do (C4)
    //  melody[9] = (Note){60, 45, 5}; // do (C4)
    //  melody[10] = (Note){62, 50, 5}; // re (D4)
    //  melody[11] = (Note){64, 55, 5}; // mi (E4)
    //  melody[12] = (Note){64, 60, 5}; // mi (E4)
    //  melody[13] = (Note){62, 65, 5}; // re (D4)
    //  melody[14] = (Note){62, 70, 5}; // re (D4)
 
    //  melody_length = 15; // Set the length of the melody

    uart_init();
 
     //-------- Timer0 setup - CTC ------------- // 
    //  TCCR0A = (1 << WGM01); // Set to CTC mode
    //  TCCR0B = (1 << CS02); // Set prescaler to 256
    //  TCNT0 = 0; // Initialize counter
    //  TIMSK0 = (1 << OCIE0A); // Enable timer interrupt
    //  DDRD |= (1 << BUZZER); // Set buzzer pin as output
    //  sei();

    // --------- Timer0 setup - PWM ---------- //
    
    // initialize BUZZER
    DDRD |= (1 << BUZZER); 
    PORTD |= (1 << BUZZER);
            
    // prescale (divide by 256)
    TCCR0B &= ~(1 << CS00);
    TCCR0B |= (1 << CS02);
    TCCR0B &= ~(1 << CS01); 

    // set timer 0 to PWM mode, phase correct mode 5
    // (so that it counts up to 0CR0A)
    TCCR0A |= (1<<WGM00);
    TCCR0A &= ~(1<<WGM01);
    TCCR0B |= (1<<WGM02);

    // toggle OC0A on compare match
    TCCR0A &= ~(1<<COM0B1);
    TCCR0A |= (1<<COM0B0);
 
    return true;
 }
 
//  ISR(TIMER0_COMPA_vect) {
//      PORTD ^= (1 << BUZZER); // Toggle buzzer pin
//  }
 
 uint8_t OCR0B_from_freq(uint16_t freq) {
    if (freq == 440) {
        return 35;
    }
    return (uint8_t) ((round(16000000 / (2 * 256 * freq))) / 2); // need to halve it because we're in mode 5
 }

 
 bool handle_speaker(uint16_t frequency, uint8_t duration) {
    printf("inputted freq: %d\n", freq);
     OCR0B = OCR0B_from_freq(frequency);
    printf("calculated ocr value: %d\n", OCR0B);
     variable_delay_ms(duration * 100);
    //  PORTD &= ~(1 << BUZZER);
     return true;
 }

 void process_midi_message(uint8_t status, uint8_t data1, uint8_t data2) {
    uint8_t command = status & 0xF0; // Mask channel

    if (command == 0x90 && data2 > 0) {
        printf("Note ON: %d\n", data1);
    } else if ((command == 0x80) || (command == 0x90 && data2 == 0)) {
        printf("Note OFF: %d\n", data1);
    } else {
        printf("Unhandled MIDI message: %02X %02X %02X\n", status, data1, data2);
    }
}
 
 int main(void) {
     initialise();
     uint8_t status, data1, data2;
 
     while (1) {
        uint8_t status, data1, data2;

        status = uart_receive(NULL);
        data1 = uart_receive(NULL);
        data2 = uart_receive(NULL);

        process_midi_message(status, data1, data2);

        handle_speaker(freq_from_note(data1), 5);




        //  if (listening_mode) {
        //      // TODO: WHEN WE HAVE THE PARTS
        //  } else {
        //      // Playing mode: play melody
        //      variable_delay_ms(melody[0].start_time * 100); // Wait for the first note to start
 
        //      for (int i = 0; i < melody_length; i++) {
        //          handle_speaker(freq_from_note(melody[i].note), melody[i].duration);
        //      }
 
        //      _delay_ms(50);
        //  }
         
         
     }
 }