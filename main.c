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
 
  #include <util/delay.h> 
 
 // define note struct
 //typedef struct {
 //    char note;
 //    uint8_t sign;
 //} Note;
 
 void Initialize()
 {
     lcd_init();
     
     LCD_setScreen(WHITE);
     
     // draw measure
     LCD_drawMeasure(80, 24, 80);
     
     //LCD_drawSharp(10, 10, 20);
 }
 
 
 //void drawNote(char note, uint8_t sign) {
 //    // i maps A -> 1, B -> 2, ...
 //    uint8_t i = note - 0x40;
 //    
 //    // draw circle in the middle of the screen
 //    LCD_drawCircle(95 + i * 5, LCD_HEIGHT >> 1, 5, BLACK);
 //    
 //    
 //}
 
 int main(void)
 {
     Initialize();
     
     
         
     while (1) 
     {
         LCD_drawNote('A', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('B', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('C', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('D', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('E', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('F', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
         
         LCD_drawNote('G', 0);
         
         
         _delay_ms(1000);
         
         LCD_setScreen(WHITE);
         LCD_drawMeasure(80, 24, 80);
     }
 }
 