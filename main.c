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
 
 
 
 int main(void)
 {
     Initialize();
     
     
         
     while (1) 
     {
         LCD_drawNote('A', 1);
         
         _delay_ms(1000);
         
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('B', 2);
         
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('C', 1);
         
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('D', 2);
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('E', 1);
         
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('F', 2);
         
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
         
         LCD_drawNote('G', 1);
         
         _delay_ms(1000);
         
         LCD_clearScreen();
         LCD_drawMeasure();
     }
 }
 