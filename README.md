# Final project: Music to my Ears

### Melina Daniilidis, Garrett Kirsch, Nikolaos Rapanis

## Results

Our final device, Music to My Ears, successfully reads live MIDI input from an electric keyboard, allows the user to shift the melody up or down by a desired number of semitones using two buttons, and replays the transposed melody through a buzzer with accurate timing. The system also displays the inputted and shifted notes on a color LCD screen for easy visual feedback. It uses an ATmega328PB microcontroller to manage real-time MIDI UART communication, PWM audio output, and user input simultaneously. We implemented a full graphics library for musical notation display, wrote custom drivers for the buzzer and screen, and built a simple cardboard housing to organize the components neatly on the workbench. In the end, we delivered a working, responsive prototype that achieves the original project goal: enabling musicians to quickly and easily transpose melodies without needing music theory knowledge.

## Video Demonstration

+++ Video link

## Photo album

+++ Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!).

## Software Requirements Specification Validation

| ID     | Description                                                                                                                                                                                                                                                                                                                                         |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| SRS-01 | The notes from the input recording shall be cleaned and parsed accurately based on their frequencies. |
| SRS-02 | The screen will show the number of times the shift button has been pressed, so the user has a visual indication of how many semitones they want to shift by.                                                                                                                                                                                       |
| SRS-03 | The transposition algorithm will correctly shift each note of the inputted melody by the user's desired amount of semitones.                                                                                                                                   |
| SRS-04 | The screen should correctly display the names of the notes from the transposed melody in a readable format, so the user can try to play the transposed melody on the keyboard if they so choose.                                                                                                                                                    |
| SRS-05 | The speaker shall correctly output the transposition melody. We will test on the same types of inputs as the transposition algorithm and in similar order.                                                                                                                                                                                          |
| SRS-06 | The screen should display the transposed notes and the speaker will play the frequencies of the transposed notes simultaneously.                                                                       
**What worked well:**
We were able to read MIDI input reliably, store melodies, shift notes by a user-defined semitone amount, and display both the original and shifted notes on the LCD in real-time.

**What changed:**
Initially, we planned for selecting input and output keys (e.g., C major to A minor). Midway through development, we changed to simple up/down relative semitone shifting to avoid needing user key input and to simplify button controls. This also removed the need to validate user-selected keys against the melody.

**New Requirements Added:**
We added requirements for button-controlled shifting and displaying the shift count on the screen.

**Challenges:**
Handling interrupts while using UART-based MIDI input was challenging so we had to resort to using polling for the buttons, until we eventually managed to use interrupts without issues just before the final demonstration.

**Validation**
SRS-01: Validation from oscilloscope proof of midi parsing.
SRS-06: Validation from video of the screen displayng the transpoed notes.
+++

## Hardware Requirements Specification Validation

| ID     | Description                                                                                                                  |
| ------ | ---------------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A mini electric piano will be used to provide inputs (i.e. music) that will then be processed by the atmega                  |
| HRS-02 | There will be two buttons to choose the shift amount (either to a higher or lower note) by a semitone for each button press. |
| HRS-03 | A screen will be used to display the input key, output key, input octave, and output octave of the music.                    |

**What worked well:**
The MIDI communication circuit using UART was stable after bypassing the original optoisolator. Our LCD screen and buzzer drivers functioned reliably. Button presses for shifting semitones worked correctly with interrupts, avoiding UART conflicts.

**What changed:**
Initially, we expected the piano to have a built-in speaker, but it did not. We added a separate buzzer system instead. Additionally, we planned to have seperate speakers for listening and playback mode, however, we decided that this would be redundant.

**Challenges:**
The circuitry proposed by the official MIDI protocol documentation did not provide an output (potentially to a faulty opto-isolator). Due to the simple nature of our circuit, we decided not to use it at all with the potential risk of not getting all the signals (something that thankfully never happened).

**Validation**
HRS-01: Video showing MIDI input captured live, UART printout confirmation.
HRS-02: Buttons implemented and tested for shift amount changes
+++

## Reflection

What did we learn?
We built a complete embedded system that integrates UART-based MIDI communication, real-time audio output via PWM, and graphical output through an LCD, all controlled by user input. Also, we learned how to adapt a project based on our hardware limitations and integration challenges.

What went well?
Our LCD graphics library, UART MIDI parsing, and note-shifting logic all worked together smoothly. The display of notes and the buzzer's frequency output worked reliably once we had the right hardware in place. Our decision to simplify the transposition interface to semitone shifting also made the user experience much more intuitive.

What accomplishments are we proud of?
We are especially proud that our device can receive live MIDI input from a real electric piano, shift melodies in real-time, and provide visual and audio feedback using custom-written drivers without any delays. Despite setbacks with and design changes, we produced a functioning musical tool that achieves our original goal.

What did we gain from this experience?
Hands-on experience with the challenges of integrating multiple subsystems on a resource-constrained microcontroller, as well as the discipline required to adjust our design without compromising functionality. We also learned the value of thorough debugging, especially when dealing with asynchronous serial protocols and timing conflicts.

Did we have to change our approach?
Yes. Originally, we planned to allow the user to transpose a melody from one musical key to another (e.g., C major to G minor). However, this approach required additional user input and key signature validation, which added unnecessary complexity. We switched to a semitone-based shifting method using two buttons (up/down), which was simpler to implement, more user-friendly, and more robust for real-time performance.

What could have been done differently?
We could have built our own mock keyboard using switches and a resistive network with an ADC. This would have removed our dependency on external parts (the MIDI keyboard), which arrived later than expected and slowed early progress. Designing our own input hardware could have also given us more control over note timing and synchronization.

Did we encounter unexpected obstacles?
Yes — the biggest was our initial use of the opto-isolator for MIDI input, following the official MIDI circuit documentation. Despite proper setup, it failed to produce any readable output, likely due to a faulty opto-isolator. After debugging, we decided to remove the opto-isolator entirely and directly receive the MIDI signal. While this technically violated the MIDI electrical spec, it worked reliably for our simple circuit without any observed signal loss.

What could be a next step for this project?
The most natural next step would be supporting polyphonic input, i.e., handling chords. Right now, our system assumes one note at a time. Expanding it to handle simultaneous notes would require reworking our melody data structure and output timing, but it would make the tool far more useful to musicians working with real music.