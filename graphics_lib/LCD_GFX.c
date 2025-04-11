/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

 #include "LCD_GFX.h"
 #include "ST7735.h"
 
 /******************************************************************************
 * Local Functions
 ******************************************************************************/
 
 
 
 const uint16_t A[16] = {0x0180,
                         0x0240,
                         0x0420,
                         0x0810,
                         0x1008,
                         0x2004,
                         0x4002,
                         0xFFFF,
                         0x8001,
                         0x8001,   
                         0x8001,
                         0x8001,
                         0x8001,
                         0x8001,   
                         0x8001,
                         0x8001 };
 
 const uint16_t B[16] = {
     0xFFFC,
     0x8002,
     0x8001,
     0x8001,
     0x8002,
     0x8004,
     0x8008,
     0xFFF0,
     0x8008,
     0x8004,
     0x8002,
     0x8001,
     0x8001,
     0x8001,
     0x8002,
     0xFFFC
 };
 
 const uint16_t C[16] = {
     0x07F8,
     0x1806,
     0x2001,
     0x4000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x4000,
     0x2001,
     0x1806,
     0x07F8,
     0x0000,
     0x0000
 };
 
 const uint16_t D[16] = {
     0xFFF8,
     0x8006,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8001,
     0x8006,
     0xFFF8,
     0x0000
 };
 
 const uint16_t E[16] = {
     0xFFFF,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0xFFFC,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0xFFFF
 };
 
 const uint16_t F[16] = {
     0xFFFF,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0xFFFC,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x8000
 };
 
 const uint16_t G[16] = {
     0x07F8,
     0x1806,
     0x2001,
     0x4000,
     0x8000,
     0x8000,
     0x8000,
     0x8000,
     0x807F,
     0x8001,
     0x8001,
     0x4001,
     0x2001,
     0x1802,
     0x07FC,
     0x0000
 };
 
 const uint8_t FLAT[8] = {
     0x10, // ___X____
     0x10, // ___X____
     0x10, // ___X____
     0x1C, // ___XXX__
     0x12, // ___X__X_
     0x12, // ___X__X_
     0x1C, // ___XXX__
     0x00  // ________
 };
 
 const uint8_t SHARP[8] = {
     0x24, // __X__X__
     0x24, // __X__X__
     0x7E, // _XXXXXX_
     0x24, // __X__X__
     0x24, // __X__X__
     0x7E, // _XXXXXX_
     0x24, // __X__X__
     0x24  // __X__X__
 };
 
 /******************************************************************************
 * Global Functions
 ******************************************************************************/
 
 /**************************************************************************//**
 * @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 * @brief		Convert RGB888 value to RGB565 16-bit color data
 * @note
 *****************************************************************************/
 uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 {
     return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
 }
 
 /**************************************************************************//**
 * @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
 * @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
 * @note
 *****************************************************************************/
 void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
     LCD_setAddr(x,y,x,y);
     SPI_ControllerTx_16bit(color);
 }
 
 
 
 /******************************************************************************
 * LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
 * You are free to create and add any additional files, libraries, and/or
 *  helper function. All code must be authentically yours.
 ******************************************************************************/
 
 /**************************************************************************//**
 * @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
 * @brief		Draw a colored circle of set radius at coordinates
 * @note
 *****************************************************************************/
 void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
 {
     // Fill this out
     for (int x = x0 - radius; x <= x0 + radius; x++) {
         for (int y = y0 - radius; y <= y0 + radius; y++) {
             if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= (radius * radius)) {
                 LCD_drawPixel(x, y, color);
             }
         }
     }
 }
 
 
 /**************************************************************************//**
 * @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
 * @brief		Draw a colored block at coordinates
 * @note
 *****************************************************************************/
 //void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
 //{
 //	// Fill this out
 //    LCD_setAddr(x0, y0, x1, y1);
 //    for (int i = 0; i < (x1 - x0 + 1) * (y1 - y0 + 1); i++) { // iterate over total num of pixels, start from 0 and end at number - 1.
 //        SPI_ControllerTx_16bit(color);
 //    }
 //}
 
 /**************************************************************************//**
 * @fn			void LCD_setScreen(uint16_t color)
 * @brief		Draw the entire screen to a color
 * @note
 *****************************************************************************/
 void LCD_setScreen(uint16_t color) 
 {
     // Fill this out
     // Set the address window for the whole screen.
     LCD_setAddr(0, 0, LCD_WIDTH, LCD_HEIGHT);
     
     // Fill the entire screen (128 x 160 pixels)
     for (uint32_t i = 0; i < 128UL * 160UL; i++) {
         SPI_ControllerTx_16bit(color);
     }
 }
 
 /**************************************************************************//**
 * @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
 * @brief		Draw a string starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 
 
 void LCD_drawMeasure() {
     uint8_t i, j;
 
     // x = 80
     // y = 24
     // length = 80
     
     // x and y are the bottom left corner - []
     for (i = 0; i < 5; i ++) {
         LCD_setAddr(80 + i * 10, 24, 80 + i * 10, 104);
         // fill in data
         for (j = 0; j <= 80; j ++) {
             SPI_ControllerTx_16bit(BLACK);
         }
     }
     
 }
 
 
 void LCD_drawNote(char note, uint8_t sign) {
     
     
     // i maps A -> 1, B -> 2, ...
     uint8_t i = note - 0x40;
     
     // draw circle in the middle of the screen
     LCD_drawCircle(90 + i * 5, LCD_HEIGHT >> 1, 5, BLACK);
     
     // draw line up the side
     LCD_setAddr(90 + i * 5, (LCD_HEIGHT >> 1) + 5, 90 + i * 5 + 24, (LCD_HEIGHT >> 1) + 5);
     
     for (i = 0; i < 25; i ++) {
         SPI_ControllerTx_16bit(BLACK);
     }
     LCD_drawNoteChar(note);
     LCD_drawSign(note, sign);
 }
 
 void LCD_drawNoteChar(char note) {
     uint8_t i, j;
     uint16_t* character;
 
     switch (note) {
         case 'A': character = A; break;
         case 'B': character = B; break;
         case 'C': character = C; break;
         case 'D': character = D; break;
         case 'E': character = E; break;
         case 'F': character = F; break;
         case 'G': character = G; break;
         default: return;  // Unknown character
     }
     
 
     for (i = 0; i < 16; i++) {
         for (j = 0; j < 16; j++) {
             if ((character[i] & (1 << (15 - j))) != 0) {
                 LCD_setAddr(50 - (i << 1), 48 + (j << 1), 51 - (i << 1), 49 + (j << 1));
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
             }
         }
     }
 }
 
 void LCD_drawSign(char note, uint8_t sign) {
     uint8_t i, j;
     uint8_t* sign_array;
     
     uint8_t k = note - 0x40;
 
     switch (sign) {
         case 1: sign_array = SHARP; break;
         case 2: sign_array = FLAT; break;
         default: return;  // Unknown symbol
     }
 
     // Adjust base positions appropriately for your layout:
     uint8_t base_x = 50; // Left of your character
     uint8_t base_y = 30; // Vertical position aligned with character
 
     // DRAW SIGN ON CHAR
     for (i = 0; i < 8; i++) {
         for (j = 0; j < 8; j++) {
             if ((sign_array[i] & (1 << (7 - j))) != 0) {
                 // Notice how 'i' controls vertical (y), 'j' controls horizontal (x)
                 LCD_setAddr(base_x - (i << 1), base_y + (j << 1),
                             base_x - (i << 1) + 1, base_y + (j << 1) + 1);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
             }
         }
     }
     
     
     // DRAW SIGN ON NOTE
     base_x = 98 + k * 5; // Left of your character
     base_y = 45;
     
     for (i = 0; i < 8; i++) {
         for (j = 0; j < 8; j++) {
             if ((sign_array[i] & (1 << (7 - j))) != 0) {
                 // Notice how 'i' controls vertical (y), 'j' controls horizontal (x)
                 LCD_setAddr(base_x - (i << 1), base_y + (j << 1),
                             base_x - (i << 1) + 1, base_y + (j << 1) + 1);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
             }
         }
     }
 }
 
 void LCD_clearScreen() {
     uint16_t i;
     // clear measure
 //    for (i = 0; i < 5; i ++) {
 //        LCD_setAddr(80 + (i << 2) + i, 24, 80 + (i << 2) + i, 104);
 //        for (j = 0; j <= 80; j ++) {
 //            SPI_ControllerTx_16bit(WHITE);
 //        }
 //    }
     // clear note 
     LCD_setAddr(89, 47, 149, 69);
     
     for (i = 0; i < 1403; i ++) {
         SPI_ControllerTx_16bit(WHITE);
     }
     
     // clear character
     LCD_setAddr(20, 48, 51, 79);
     for (i = 0; i < 1024; i ++) {
         SPI_ControllerTx_16bit(WHITE);
     }
     // clear sign
     LCD_setAddr(36, 30, 51, 45);
     for (i = 0; i < 256; i ++) {
         SPI_ControllerTx_16bit(WHITE);
     }
     
     return;
 }
 
 