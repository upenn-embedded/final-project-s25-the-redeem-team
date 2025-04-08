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
 
 /**************************************************************************//**
 * @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
 * @brief		Draw a character starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
     uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
     int i, j;
     if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
         for(i=0;i<5;i++){
             uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
             for(j=0;j<8;j++){
                 if ((pixels>>j)&1==1){
                     LCD_drawPixel(x+i,y+j,fColor);
                 }
                 else {
                     LCD_drawPixel(x+i,y+j,bColor);
                 }
             }
         }
     }
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
 * @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
 * @brief		Draw a line from and to a point with a color
 * @note
 *****************************************************************************/
 void LCD_drawLine(short x0,short y0,short x1,short y1, uint16_t c)
 {
     // Fill this out
     float slope, i, j, dx, dy;
     i = x0;
     j = y0;
 
     if (x1 == x0) {
         dx = 0;
         
         dy = (y1 > y0) ? 1 : -1;
     } else if (y1 == y0) {
         dx = (x1 > x0) ? 1 : -1;
         dy = 0;
     } else {
         slope = (float) (y1 - y0)/(x1 - x0);
     
         // check if slope if < 1
         if (-1 < slope < 1) {
             dx = 1;
             dy = slope;
         } else {
             dx = 1 / slope;
             dy = 1;
         }
     }
 
     
     while (!((int) i == x1 && (int) j == y1)) {
         LCD_drawPixel((int) i, (int) j, c);
         i += dx;
         j += dy;
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
 //void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
 //{
 //	// Fill this out
 //    uint8_t xCurr = x;
 //    while (*str != '\0') {
 //        LCD_drawChar(xCurr, y, *str, fg, bg);
 //        if (*str != ' ') { // don't need to give as much space to spaces.
 //            xCurr += 6;
 //        } else {
 //            xCurr += 4;
 //        }
 //        str++;
 //        
 //    }
 //}
 
 void LCD_drawMeasure(uint8_t x, uint8_t y, uint8_t length) {
     uint8_t i, j;
     
     // x and y are the bottom left corner - []
     for (i = 0; i < 5; i ++) {
         LCD_setAddr(x + i * 10, y, x + i * 10, y - length);
         // fill in data
         for (j = 0; j < length; j ++) {
             SPI_ControllerTx_16bit(BLACK);
         }
     }
     
 }
 
 void LCD_drawSharp(uint8_t x, uint8_t y, uint8_t length) {
     uint8_t i, j, quarter, three_quarters;
     
     quarter = length / 3;
     three_quarters = quarter << 1;
     
     
     // x and y are the bottom left corner
     
     // left vertical line at y + length/4
     
     LCD_setAddr(x, y + quarter, x + length, y + quarter);
     
     for (i = 0; i < length; i ++) {
         SPI_ControllerTx_16bit(BLACK);
     }
     
     // right vertical line at y + 3 * length / 4
     
     LCD_setAddr(x, y + three_quarters, x + length, y + three_quarters);
     
     for (i = 0; i < length; i ++) {
         SPI_ControllerTx_16bit(BLACK);
     }
     
     // top horizontal line at x + 3 * length / 4
     
     LCD_setAddr(x + three_quarters, y, x + three_quarters, y + length);
     
     for (i = 0; i < length; i ++) {
         SPI_ControllerTx_16bit(BLACK);
     }
     
     // bottom horizontal line at x + length / 4
     
     LCD_setAddr(x + quarter, y, x + quarter, y + length);
     
     for (i = 0; i < length; i ++) {
         SPI_ControllerTx_16bit(BLACK);
     }
     
 //    // top slanted horizontal line from (x + length / 2, y) to (x + 3 * length / 4, y + length)
 //    LCD_drawLine(x + (length >> 1), y, x + 3* (length >> 2), y + length, BLACK); 
 //    
 //    
 //    // bottom slanted horizontal line from (x + length / 4, y) to (x +  length / 2, y + length)
 //    LCD_drawLine(x + (length >> 2), y, x + (length >> 1), y + length, BLACK); 
     
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
 }
 void LCD_drawNoteChar(char note) {
 //    if (note == 'A') {
 //        LCD_drawLine(40, (LCD_HEIGHT >> 1), 20, (LCD_HEIGHT >> 1) + 10, BLACK);
 //        LCD_drawLine(20, (LCD_HEIGHT >> 1) - 10, 40, (LCD_HEIGHT >> 1), BLACK);
 //        LCD_drawLine(30, (LCD_HEIGHT >> 1) - 4, 30, (LCD_HEIGHT >> 1) + 4, BLACK);
 //    }
     int i, j;
     uint16_t* character;
     switch (note) {
         case 'A': character = A;
     }
     
     for (i = 0; i < 16; i ++) {
         for (j = 0; j < 16; j ++) {
             if (A[i] & (1 << j) != 0) {
                 //  fill in 2x2 pixel block
                 LCD_setAddr(50 - 2*i, 48 + (j << 1), 51 - 2*i, 49 + (j << 1));
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 SPI_ControllerTx_16bit(BLACK);
                 
             }
         }
     }
             
     
     
     return;
 }