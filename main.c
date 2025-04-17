/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 

 #define F_CPU 16000000UL

 #include <avr/io.h>
 #include "ST7735.h"
 #include "LCD_GFX.h"
 #include <stdio.h>
 
 
 #include <avr/interrupt.h>
 #include "uart.h"
 
 #include <util/delay.h> 
 #include <xc.h>
 
 
 
 // define note struct
 //typedef struct {
 //    char note;
 //    uint8_t sign;
 //} Note;
 
 //#define CONTROLLER
 //#define PERIPHERAL
 #define MIDI
 
 void Initialize()
 {
     lcd_init();
     //DDRD |= (1 << PD1);  // Set PD1 as output
     uart_init();
     LCD_setScreen(WHITE);
     
     // draw measure
     LCD_drawMeasure();
 }
 
 #ifdef CONTROLLER
 
 volatile uint8_t note = 'A';
 volatile uint8_t sign = 1;
 
 void USART0_Init(uint16_t ubrr) {
     UBRR0H = (ubrr >> 8);
     UBRR0L = ubrr;
     UCSR0B = (1 << TXEN0);                     // Enable transmitter
     UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // 8 data bits, 1 stop bit
 }
 
 void USART0_Transmit(uint8_t data) {
     while (!(UCSR0A & (1 << UDRE0)));  // Wait until buffer is empty
     UDR0 = data;
 }
 
 int main(void) {
     USART0_Init(103);  // 9600 baud at 16MHz
     Initialize();
 
     while (1) {
 //        USART0_Transmit('A');
 //        USART0_Transmit(1);
 //        _delay_ms(1000);
 
         // transmit note and sign
         USART0_Transmit(note + 1);
         USART0_Transmit(sign + 1);
         
         // draw note
         LCD_clearScreen();
         LCD_drawMeasure();
         LCD_drawNote(note, sign);
         
         // update/reset note
         note = note + 1;
         if (note == 'H') {
             note = 'A';
         }
         
         _delay_ms(1000);  // Send every 1 second
     }
 }
 #endif
 
 #ifdef PERIPHERAL
 
 #include <avr/interrupt.h>
 volatile uint8_t note = 0;
 volatile uint8_t sign = 0;
 volatile uint8_t byte_count = 0;
 volatile uint8_t note_change = 0;
 
 void USART0_Init(uint16_t ubrr) {
     UBRR0H = (ubrr >> 8);
     UBRR0L = ubrr;
     UCSR0B = (1 << RXEN0) | (1 << RXCIE0);     // Enable receiver + RX complete interrupt
     UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // 8-bit data
     sei();  // Enable global interrupts
 }
 
 // USART Receive Complete ISR
 ISR(USART_RX_vect) {
     uint8_t data = UDR0;
 
     if (byte_count == 0) {
         note = data;
         byte_count = 1;
     } else {
         sign = data;
         byte_count = 0;
         note_change = 1;
         // Optional: Signal that two bytes are ready (e.g., toggle LED or set a flag)
     }
 }
 
 
 int main(void) {
     USART0_Init(103);  // 9600 baud at 16MHz
     Initialize();
 
     while (1) {
 
         if (note_change) {
             LCD_clearScreen();
             LCD_drawMeasure();
             LCD_drawNote(note, sign);
             note_change = 0;
             _delay_ms(50);
         }
 
     }
 }
 #endif
 
 #ifdef MIDI
 //
//  volatile uint8_t received_byte = 0;
//  volatile uint8_t data_ready = 0;
 
 
//  ISR(USART0_RX_vect) {
//      received_byte = UDR0;     // Read the received byte
//      data_ready = 1;           // Flag to signal main loop
//  }
 
//  int main(void) {
//      Initialize();
//      sei();
 
//      char buffer[100];
//      uint8_t i = 0;
//      char c;
 
//      while (1) {
//          c = uart_receive(NULL);  // Blocks until byte received
 
//          if (c == '\n' || c == '\r') {
//              buffer[i] = '\0';  // Null-terminate string
//              printf("Received line: %s\n", buffer);
 
//              // Reset buffer index for next message
//              i = 0;
//          } else if (i < 100 - 1) {
//              buffer[i++] = c;  // Store byte
//          } else {
//              // Buffer overflow protection
//              i = 0;
//              printf("Input too long, resetting buffer.\n");
//          }
//      }
//  }
 

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

int main() {
    uart_init();

    uint8_t status, data1, data2;

    while (1) {
        // blocking read of MIDI bytes
        status = uart_receive(NULL);
        data1 = uart_receive(NULL);
        data2 = uart_receive(NULL);

        process_midi_message(status, data1, data2);
    }

    return 0;
}
 #endif