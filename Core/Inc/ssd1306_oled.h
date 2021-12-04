/**
  ******************************************************************************
  * @file    ssd1306_oled.h
  * @author  Marco, Roldan L.
  * @version v1.0
  * @date    August 29, 2021
  * @brief   
  ******************************************************************************
  *
  * Copyright (C) 2021  Marco, Roldan L.
  * 
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.en.html.
  * 
  * 
  * https://github.com/rmarco30
  * 
  ******************************************************************************
**/

#ifndef __SSD1306_OLED_H
#define __SSD1306_OLED_H

#include "stm32f10x.h"
#include "ssd1306_image.h"
#include "ssd1306_font.h"

#include "i2c.h"

/**
 * 
 * I2C Data bus format
 * 
 * [S] [SSD1306_SLAVE_ADDR_W] [ACK] [CTRL_BYTE] [ACK] [DATA] [Sr / P]
 * 
 * CTRL_BYTE decides wether the following DATA will be a command or data which will be stored in GDDRAM
 * 
**/



/* I2C peripheral used, either I2C1 or I2C2 */
#define SSD1306_I2Cx                ( I2C1 )

/* SSD1306 Display Width and Height */
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64

/* Change the slave address according on your device */
#define SSD1306_SLAVE_ADDR          0x3C
#define SSD1306_SLAVE_ADDR_W        ( SSD1306_SLAVE_ADDR << 1 )
#define SSD1306_SLAVE_ADDR_R        ( (SSD1306_SLAVE_ADDR << 1) | 0x01 )


/* SSD1306 Typedefs */

typedef enum
{
    CMD_CTRL_BYTE = 0x00,
    DATA_CTRL_BYTE = 0x40
} SSD1306_CtrlByte_t;

typedef enum
{
    FALSE = 0,
    TRUE = !FALSE
} SSD1306_FunctionalState_t;

typedef enum
{
    RIGHT = 0,
    LEFT = !RIGHT,

    VRIGHT = 0x01,
    VLEFT = 0x02,

    DOWN = 0,
    UP = !DOWN
} SSD1306_ScrollDir_t;

typedef enum
{
    FRAME_5 = 0,
    FRAME_64,
    FRAME_128,
    FRAME_256,
    FRAME_3,
    FRAME_4,
    FRAME_25,
    FRAME_2
} SSD1306_FrameFreq_t;


typedef enum
{
    PAGE0 = 0,
    PAGE1 = 1,
    PAGE2 = 2,
    PAGE3 = 3,
    PAGE4 = 4,
    PAGE5 = 5,
    PAGE6 = 6,
    PAGE7 = 7
} SSD1306_PageNum_t;


typedef enum
{
    VERTICAL = 0,
    HORIZONTAL = !VERTICAL
} SSD1306_Orientation_t;


typedef enum
{
    HORIZONTAL_MODE = 0x00,
    VERTICAL_MODE,
    PAGE_MODE
} SSD1306_AddrMode_t;




/**
 * @brief    Executes display's initialization sequence
 * @param    none
 * @retval   none
 */
void ssd1306_init(void);


/**
 * @brief    Print character(s) to the current cursor position on display
 * @param    ch: pointer to array of character(s)
 * @retval   none
 */
void ssd1306_drawChar(const char *ch);


/**
 * @brief    Prints a bitmap to the entire display
 * @param    bitmap: pointer to bitmap
 * @retval   none
 */
void ssd1306_drawBitmap(const uint8_t *bitmap);


/**
 * @brief    Draw a pixel anywhere on the display
 * @param    x_pos: x-coordinate of pixel
 * @param    y_pos: y-coordinate of pixel
 * @retval   none
 */
void ssd1306_drawPixel(uint8_t x_pos, uint8_t y_pos);


/**
 * @brief    Clears a pixel anywhere on the display
 * @param    x_pos: x-coordinate of pixel
 * @param    y_pos: y-coordinate of pixel
 * @retval   none
 */
void ssd1306_clearPixel(uint8_t x_pos, uint8_t y_pos);


/**
 * @brief    Draw a line anywhere on the display
 * @param    x_pos1: point 1 x-coordinate
 * @param    y_pos1: point 1 y-coordinate
 * @param    x_pos2: point 2 x-coordinate
 * @param    y_pos2: point 2 y-coordinate
 * @retval   none
 */
void ssd1306_drawLine(uint8_t x_pos1, uint8_t y_pos1, uint8_t x_pos2, uint8_t y_pos2);


/**
 * @brief    Draw a vertical line anywhere on the display
 * @param    x_pos: x-coordinate of the line, this covers x_pos1 and x_pos2
 * @param    y_pos1: point 1 y-coordinate
 * @param    y_pos2: point 2 y-coordinate
 * @retval   
 */
void ssd1306_drawVerticalLine(uint8_t x_pos, uint8_t y_pos1, uint8_t y_pos2);


/**
 * @brief    Draw a horizontal line anywhere on the display
 * @param    y_pos: y-coordinate of the line, this covers y_pos1 and y_pos2
 * @param    x_pos1: point 1 x-coordinate
 * @param    x_pos2: point 2 x-coordinate
 * @retval   
 */
void ssd1306_drawHorizontalLine(uint8_t y_pos, uint8_t x_pos1, uint8_t x_pos2);


/**
 * @brief    Draw a circle anywhere on the display
 * @param    x_cen: x-coordinate of circle's center, range 0..display width
 * @param    y_cen: y-coordinate of circle's center, range 0..display heigth
 * @param    radius: circle's radius, for largest circle radius = display height / 2
 * @retval   none
 */
void ssd1306_drawCircle(uint8_t x_cen, uint8_t y_cen, uint8_t radius);


/**
 * @brief    Move the cursor to desired area on the display
 * @param    col: column range from 0..127
 * @param    row: row range from PAGE0..PAGE7
 * @retval   none
 */
void ssd1306_displayMoveCursor(uint8_t col, SSD1306_PageNum_t row);


/**
 * @brief    Clears the entire display
 * @param    none
 * @retval   none
 */
void ssd1306_displayClear(void);


/**
 * @brief    Adjust the contrast of the display
 * @param    val: contrast level from 0-255, reset value is 128
 * @retval   none
 */
void ssd1306_displayContrast(uint8_t val);


/**
 * @brief    Inverts the display
 *           If TRUE, applying '1' in any bit position turns off the pixel.
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayInvert(SSD1306_FunctionalState_t state);


/**
 * @brief    Turns ON or OFF the display
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayOn(SSD1306_FunctionalState_t state);


/**
 * @brief    Scrolls the display horizontally
 * @param    dir: direction of the scroll. LEFT or RIGHT
 * @param    freq: scroll speed in frames. Choose values from SSD1306_FrameFreq_t
 *                 the lower the frame the faster the scrolling speed.
 * @param    page_start: start page where the scroll will start
 * @param    page_end: end page where the scroll will end
 *           page_start and page_end covers the area where horizontal scrolling will be
 *           applied.
 * @retval   none
 */
void ssd1306_displayScrollHorizontal(SSD1306_ScrollDir_t dir, SSD1306_FrameFreq_t freq, 
                               SSD1306_PageNum_t page_start, SSD1306_PageNum_t page_end);



/**
 * @brief    Experimental function that scrolls the display vertically
 *           Note: for this function to work as intended, avoid placing any combination of pixels on PAGE7
 * @param    dir: scroll direction. UP or DOWN
 * @param    freq: scroll speed in frames. Choose values from SSD1306_FrameFreq_t
 *                 the lower the frame the faster the scrolling speed.
 * @param    freeze: page to freeze, this page will not scroll vertically, only works when dir is UP
 * @retval   none
 */
void ssd1306_displayScrollVertical(SSD1306_ScrollDir_t dir, SSD1306_FrameFreq_t freq, SSD1306_PageNum_t freeze);



/**
 * @brief    Scrolls the display diagonally
 * @param    dir: scroll direction. VRIGHT or VLEFT
 * @param    freq: scroll speed in frames. Choose values from SSD1306_FrameFreq_t
 *                 the lower the frame the faster the scrolling speed.
 * @param    page_start: start page where the scroll will start.
 * @param    page_end: end page where the scroll will end.
 *           page_start and page_end covers the area where diagonal scrolling will be
 *           applied. The page that is not included will scroll vertically. To fix it in place
 *           ssd1306_displaySetVerticalScrollArea() must be configured.
 * @param    offset: vertical scrolling offset. setting this to 0 removes the vertical
 *           movement of the display, while setting this to 0x01 scrolls the display to
 *           dir + from up to bottom, and 0x3f scrolls the display from dir + bottom to up
 *           example: dir = VRIGHT, offset = 0x01, the display will move diagonally from bottom left
 *           to upper right direction.
 * @retval   none
 */
void ssd1306_displayScrollDiagonal(SSD1306_ScrollDir_t dir, SSD1306_FrameFreq_t freq,
                                   SSD1306_PageNum_t page_start, SSD1306_PageNum_t page_end, uint8_t offset);



/**
 * @brief    Sets the scroll area for diagonal scrolling 
 * @param    fixed: value from 0..63. This will freeze the rows that is excluded
 *           from page_start + page_end parameter of ssd1306_displayScrollDiagonal()
 *           function. example: if page_start = PAGE1, page_end = PAGE7,
 *           fixed must be equal to 7 (size of one PAGE. value starting from 0)
 * @retval   none
 */
void ssd1306_displaySetVerticalScrollArea(uint8_t fixed);


/**
 * @brief    Enables or disables the last scroll command issued
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayScrollState(SSD1306_FunctionalState_t state);


/**
 * @brief    Set the addressing mode used by the display
 *           see the display datasheet for more information
 * @param    mode: Any one of the item under SSD1306_AddrMode_t typedef
 *                 HOR_ADDR_MODE (default)
 * @retval   none
 */
void ssd1306_displayAddrMode(SSD1306_AddrMode_t mode);


/**
 * @brief    Flips the display horizontally
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayFlip(SSD1306_Orientation_t orientation, FunctionalState state);


/**
 * @brief    Update the entire GDDRAM
 * @param    none
 * @retval   none
 */
void ssd1306_ramUpdateFull(void);


/**
 * @brief    Update a byte of the GDDRAM
 * @param    byte_pos: address of the byte to update. value range 0..1023
 * @param    byte_val: value to be put in the byte
 *           Note: The current value stored in the GDDRAM before a call to this
 *                 function will be retain.
 * @retval   none
 */
void ssd1306_ramClear(void);


/**
 * @brief    Write a byte to the GDDRAM
 * @param    byte_pos: address of the byte to update. value range 0..1023
 * @param    byte_val: value to be put in the byte
 *           Note: * The current value stored in the GDDRAM before a call to this
 *                   function will be retain.
 *                 * This will only write a value the GDDRAM, to display the
 *                   result ssd1306_ramUpdateFull() function must be called.
 * @retval   none
 */
void ssd1306_ramWrite(uint16_t byte_pos, uint8_t byte_val);


/**
 * @brief    Clears the entire GDDRAM
 * @param    none
 *           Note: This will only clear the GDDRAM, to display the result 
 *                 ssd1306_ramUpdateFull() function must be called.
 * @retval   none
 */
void ssd1306_ramUpdateByte(uint16_t byte_pos, uint8_t byte_val);



#endif /* __SSD1306_OLED_H */