# Final project: Music to my Ears

### Melina Daniilidis, Garrett Kirsch, Nikolaos Rapanis

## Video Demonstration

## Photo album

+++ Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!).

## Software Requirements Specification Validation

| ID     | Description                                                                                                                                                                                                                                                                                                                                         |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| SRS-01 | The notes from the input recording shall be cleaned and parsed accurately based on their frequencies. For initial testing, we will assume that one note is placed at a time (no chords) and the melody is not more than seven notes. Once it can parse this simple input successfully, we will incrementally test chords and more complex melodies. |
| SRS-02 | The screen will show the number of times the shift button has been pressed, so the user has a visual indication of how many semitones they want to shift by.                                                                                                                                                                                        |
| SRS-03 | The transposition algorithm will correctly shift each note of the inputted melody by the user's desired amount of semitones.                                                                                                                                                                                                                        |
| SRS-04 | The screen should correctly display the names of the notes from the transposed melody in a readable format, so the user can try to play the transposed melody on the keyboard if they so choose.                                                                                                                                                    |
| SRS-05 | The speaker shall correctly output the transposition melody. We will test on the same types of inputs as the transposition algorithm and in similar order.                                                                                                                                                                                          |
| SRS-06 | The screen should display the transposed notes and the speaker will play the frequencies of the transposed notes simultaneously.                                                                                                                                                                                                                    |

## Hardware Requirements Specification Validation

| ID     | Description                                                                                                                  |
| ------ | ---------------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A mini electric piano will be used to provide inputs (i.e. music) that will then be processed by the atmega                  |
| HRS-02 | A separate speaker is used to sound the inputted keys of the piano, and the shifted notes.                                   |
| HRS-03 | There will be two buttons to choose the shift amount (either to a higher or lower note) by a semitone for each button press. |
| HRS-04 | A screen will be used to display the input key, output key, input octave, and output octave of the music.                    |

## Reflection






### 3. Results

*What were your results? Namely, what was the final solution/design to your problem?*

#### 3.1 Software Requirements Specification (SRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

Reflect on your project. Some questions to address:

* What did you learn from it?
* What went well?
* What accomplishments are you proud of?
* What did you learn/gain from this experience?
* Did you have to change your approach?
* What could have been done differently?
* Did you encounter obstacles that you didn’t anticipate?
* What could be a next step for this project?

## References

Fill in your references here as you work on your final project. Describe any libraries used here.
