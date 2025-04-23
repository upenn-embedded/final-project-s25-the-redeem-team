#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t int0_waiting_rising = 1;
volatile uint8_t int1_waiting_rising = 1;
volatile uint8_t last_pd4_state = 0;

void init_buttons(void) {
    // Set PD2, PD3, PD4 as input
    DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4));
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4);  // Enable pull-ups

    // --- INT0: PD2 rising edge
    EICRA |= (1 << ISC01) | (1 << ISC00);  // Waiting for rising edge
    // --- INT1: PD3 rising edge
    EICRA |= (1 << ISC11) | (1 << ISC10);  // Waiting for rising edge
    EIMSK |= (1 << INT0) | (1 << INT1);    // Enable INT0 and INT1

    // --- PCINT2 (PCINT20 = PD4)
    PCICR |= (1 << PCIE2);       // Enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT20);    // Enable PCINT20
    last_pd4_state = PIND & (1 << PD4);  // Initialize state

    sei(); // Enable global interrupts
}

// === INT0 (PD2)
ISR(INT0_vect) {
    if (int0_waiting_rising) {
        // TODO: Button 2 released
    }
    // Toggle edge detection
    if (int0_waiting_rising) {
        EICRA &= ~((1 << ISC01) | (1 << ISC00)); // Falling
        EICRA |= (1 << ISC01);
    } else {
        EICRA &= ~((1 << ISC01) | (1 << ISC00)); // Rising
        EICRA |= (1 << ISC01) | (1 << ISC00);
    }
    int0_waiting_rising ^= 1;
}

// === INT1 (PD3)
ISR(INT1_vect) {
    if (int1_waiting_rising) {
        // TODO: Button 2 released
    }

    // Toggle edge detection
    if (int1_waiting_rising) {
        EICRA &= ~((1 << ISC11) | (1 << ISC10)); // Falling
        EICRA |= (1 << ISC11);
    } else {
        EICRA &= ~((1 << ISC11) | (1 << ISC10)); // Rising
        EICRA |= (1 << ISC11) | (1 << ISC10);
    }
    int1_waiting_rising ^= 1;
}

// === PCINT2_vect for PD4 (PCINT20)
ISR(PCINT2_vect) {
    uint8_t current_state = PIND & (1 << PD4);

    if ((last_pd4_state == 0) && (current_state != 0)) {
        // Rising edge on PD4
        // TODO: Button 3 released
    }

    last_pd4_state = current_state;
}