/**
  ******************************************************************************
  * @file    ssd1306_oled.c
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

#include "ssd1306_oled.h"


/* Ram space that simulate the display's GDDRAM */
static uint8_t ssd1306_ram[1024] __attribute__((aligned(4)));
static uint8_t *p_ram = ssd1306_ram;

static void ssd1306_cmd_single(uint8_t cmd);
static void ssd1306_cmd_double(uint8_t cmd, uint8_t val);



/**
 * @brief    Print character(s) to the current cursor position on display
 * @param    ch: pointer to array of character(s)
 * @retval   none
 */
void ssd1306_drawChar(const char *ch)
{
    uint32_t len;
    for(len = 0; ch[len] != '\0'; len++);

    for(uint32_t bitpos = 0; bitpos < len; bitpos++)
    {
        for(uint32_t bitmap = 0; bitmap < 5; bitmap++)
        {
            i2c_start(SSD1306_I2Cx);
            i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
            i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
            i2c_write(SSD1306_I2Cx, font[ch[ bitpos ] - 0x20] [bitmap] );
        }
    }
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Prints a bitmap to the entire display
 * @param    bitmap: pointer to bitmap
 * @retval   none
 */
void ssd1306_drawBitmap(const uint8_t *bitmap)
{
    ssd1306_displayMoveCursor(0, 0);

    for(uint16_t i = 0; i < 1024; i++)
    {
        i2c_start(SSD1306_I2Cx);
        i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
        i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
        i2c_write(SSD1306_I2Cx, bitmap[i]);
    }
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Draw a pixel anywhere on the display
 * @param    x_pos: x-coordinate of pixel
 * @param    y_pos: y-coordinate of pixel
 * @retval   none
 */
void ssd1306_drawPixel(uint8_t x_pos, uint8_t y_pos)
{
    ssd1306_displayMoveCursor(x_pos, y_pos / 8);
    uint16_t byte_pos = x_pos + (128 * ( y_pos / 8) );

    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx, *(p_ram + byte_pos) |= (0x01 << (y_pos % 8) )  );
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Clears a pixel anywhere on the display
 * @param    x_pos: x-coordinate of pixel
 * @param    y_pos: y-coordinate of pixel
 * @retval   none
 */
void ssd1306_displayClearPixel(uint8_t x_pos, uint8_t y_pos)
{
    ssd1306_displayMoveCursor(x_pos, y_pos / 8);
    uint16_t byte_pos = x_pos + (128 * ( y_pos / 8) );

    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx, *(p_ram + byte_pos) = (0x00 << (y_pos % 8) )  );
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Draw a line anywhere on the display
 * @param    x_pos1: point 1 x-coordinate
 * @param    y_pos1: point 1 y-coordinate
 * @param    x_pos2: point 2 x-coordinate
 * @param    y_pos2: point 2 y-coordinate
 * @retval   none
 */
void ssd1306_drawLine(uint8_t x_pos1, uint8_t y_pos1, uint8_t x_pos2, uint8_t y_pos2)
{
    /**
     * Bresenham's Line Algorithm
     * 
     * pk = dy2 - dX
     * 
     * if pk < 0
     * pk = pk + dy2
     * 
     * if pk >= 0
     * pk = pk + dy2 - dx2
     * 
     */

    int8_t dx = x_pos2 - x_pos1;
    int8_t dy = y_pos2 - y_pos1;

    int8_t dx_sym = (dx > 0) ? 1 : -1;
    int8_t dy_sym = (dy > 0) ? 1 : -1;

    dx = dx * dx_sym;
    dy = dy * dy_sym;

    int16_t dx2 = 2 * dx;
    int16_t dy2 = 2 * dy;
    int16_t pk;

    if( dx >= dy)
    {
        pk = dy2 - dx;
        while( x_pos1 != x_pos2)
        {
            ssd1306_drawPixel(x_pos1, y_pos1);
            x_pos1 = x_pos1 + dx_sym;

            if(pk < 0)
            {
                pk = pk + dy2;
            }
            else
            {
                pk = pk + dy2 - dx2;
                y_pos1 = y_pos1 + dy_sym;
            }
        }
    }

    else
    {
        pk = dx2 - dy;
        while( y_pos1 != y_pos2)
        {
            ssd1306_drawPixel(x_pos1, y_pos1);
            y_pos1 = y_pos1 + dy_sym;

            if(pk < 0)
            {
                pk = pk + dx2;
            }

            else
            {
                pk = pk + dx2 - dy2;
                x_pos1 = x_pos1 + dx_sym;
            }
        }
    }
    ssd1306_drawPixel(x_pos1, y_pos1);
}


/**
 * @brief    Draw a vertical line anywhere on the display
 * @param    x_pos: x-coordinate of the line, this covers x_pos1 and x_pos2
 * @param    y_pos1: point 1 y-coordinate
 * @param    y_pos2: point 2 y-coordinate
 * @retval   
 */
void ssd1306_drawVerticalLine(uint8_t x_pos, uint8_t y_pos1, uint8_t y_pos2)
{
    int8_t y_dist;
    
    if(y_pos2 > y_pos1)
    {
        y_dist = y_pos2 - y_pos1;

        for(uint8_t i = 0; i <= (uint8_t)y_dist; i++)
        {
            ssd1306_drawPixel(x_pos, y_pos1 + i);
        }
    }

    else
    {
        y_dist = y_pos1 - y_pos2;
        for(uint8_t i = 0; i <= (uint8_t)y_dist; i++)
        {
            ssd1306_drawPixel(x_pos, y_pos1 - i);
        }
    }
}


/**
 * @brief    Draw a horizontal line anywhere on the display
 * @param    y_pos: y-coordinate of the line, this covers y_pos1 and y_pos2
 * @param    x_pos1: point 1 x-coordinate
 * @param    x_pos2: point 2 x-coordinate
 * @retval   
 */
void ssd1306_drawHorizontalLine(uint8_t y_pos, uint8_t x_pos1, uint8_t x_pos2)
{
    int8_t x_dist;

    if(x_pos2 > x_pos1)
    {
        x_dist = x_pos2 - x_pos1;

        for(uint8_t i = 0; i <= (uint8_t)x_dist; i++)
        {
            ssd1306_drawPixel(x_pos1 + i, y_pos);
        }
    }
    else
    {
        x_dist = x_pos1 - x_pos2;

        for(uint8_t i = 0; i <= (uint8_t)x_dist; i++)
        {
            ssd1306_drawPixel(x_pos1 - i, y_pos);
        }
    }
}


/**
 * @brief    Draw a circle anywhere on the display
 * @param    x_cen: x-coordinate of circle's center, range 0..display width
 * @param    y_cen: y-coordinate of circle's center, range 0..display heigth
 * @param    radius: circle's radius, for largest circle radius = display height / 2
 * @retval   none
 */
void ssd1306_drawCircle(uint8_t x_cen, uint8_t y_cen, uint8_t radius)
{
    int8_t x0 = 0;
    int8_t y0 = radius;
    int8_t d0 = 1 - radius;
    uint8_t display_width = SSD1306_WIDTH - 1;
    uint8_t display_height = SSD1306_HEIGHT - 1;

    while(x0 < y0)
    {
        x0 = x0 + 1;

        if( d0 < 0 )
        {
            d0 = d0 + ( 2 * x0 ) + 1;
        }
        else
        {
            y0 = y0 - 1;
            d0 = d0 + ( 2 * (x0-y0) ) + 1;    
        }

        /* Draw a pixel in each quadrant,
           if out of bounds do not draw the pixel */

        /* 1st octant */
        if( ( (x_cen + y0) <= 127 ) && ( ( y_cen - x0 ) >= 0  )  )
        {
            ssd1306_drawPixel(x_cen + y0, y_cen - x0);
        }

        /* 2nd octant */
        if( ( (x_cen + x0) <= 127 ) && ( ( y_cen - y0 ) >= 0  )  )
        {
            ssd1306_drawPixel(x_cen + x0, y_cen - y0);
        }
        
        /* 3rd octant */
        if( ( (x_cen - x0) >= 0 ) && ( ( y_cen - y0 ) >= 0  )  )
        {
            ssd1306_drawPixel(x_cen - x0, y_cen - y0);
        }

        /* 4th octant */
        if( ( (x_cen - y0) >= 0 ) && ( ( y_cen - x0 ) >= 0  )  )
        {
            ssd1306_drawPixel(x_cen - y0, y_cen - x0);
        }

        /* 5th octant */
        if( ( (x_cen - y0) >= 0 ) && ( ( y_cen + x0 ) <= 63  )  )
        {
            ssd1306_drawPixel(x_cen - y0, y_cen + x0);
        }

        /* 6th octant */
        if( ( (x_cen - x0) >= 0 ) && ( ( y_cen + y0 ) <= 63  )  )
        {
            ssd1306_drawPixel(x_cen - x0, y_cen + y0);
        }

        /* 7th octant */
        if( ( (x_cen + x0) <= 127 ) && ( ( y_cen + y0 ) <= 63  )  )
        {
            ssd1306_drawPixel(x_cen + x0, y_cen + y0);
        }
        
        /* 8th octant */
        if( ( (x_cen + y0) <= 127 ) && ( ( y_cen + x0 ) <= 63  )  )
        {
            ssd1306_drawPixel(x_cen + y0, y_cen + x0);
        }
    }

    /* Draw a pixel in 0, 90, 180, 270 degrees,
       if out of bounds do not draw the pixel */

    if( (x_cen + radius) <= display_width )
    {
        ssd1306_drawPixel(x_cen + radius, y_cen);
    }
    if( (x_cen - radius) >= 0 )
    {
        ssd1306_drawPixel(x_cen - radius, y_cen);
    }
    if( (y_cen + radius) <= display_height )
    {
        ssd1306_drawPixel(x_cen, y_cen + radius);
    }
    if( (y_cen - radius) >= 0 )
    {
        ssd1306_drawPixel(x_cen, y_cen - radius);
    }
}


/**
 * @brief    Move the cursor to desired area on the display
 * @param    col: column range from 0..127
 * @param    row: row range from PAGE0..PAGE7
 * @retval   none
 */
void ssd1306_displayMoveCursor(uint8_t col, SSD1306_PageNum_t row)
{
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, 0x21);
    i2c_write(SSD1306_I2Cx, col);
    i2c_write(SSD1306_I2Cx, 0x7F);
    i2c_write(SSD1306_I2Cx, 0x22);
    i2c_write(SSD1306_I2Cx, row);
    i2c_write(SSD1306_I2Cx, 0x07);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Clears the entire display
 * @param    none
 * @retval   none
 */
void ssd1306_displayClear(void)
{
    ssd1306_displayMoveCursor(0, 0);

    for(uint16_t i = 0; i < 1024; i++)
    {
        i2c_start(SSD1306_I2Cx);
        i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
        i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
        i2c_write(SSD1306_I2Cx, 0x00);
    }
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Adjust the contrast of the display
 * @param    val: contrast level from 0-255, reset value is 128
 * @retval   none
 */
void ssd1306_displayContrast(uint8_t val)
{
    ssd1306_cmd_double(0x81, val);
}


/**
 * @brief    Inverts the display
 *           If TRUE, applying '1' in any bit position turns off the pixel.
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayInvert(SSD1306_FunctionalState_t state)
{
    if(state)
    {
        ssd1306_cmd_single(0xA7);
    }
    else
    {
        ssd1306_cmd_single(0xA6);
    }
}


/**
 * @brief    Turns ON or OFF the display
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayOn(SSD1306_FunctionalState_t state)
{
    if(state)
    {
        ssd1306_cmd_single(0xAF);
    }
    else
    {
        ssd1306_cmd_single(0xAE);
    }
}


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
                           SSD1306_PageNum_t page_start, SSD1306_PageNum_t page_end)
{
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, CMD_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx, 0x26 | dir);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, page_start);
    i2c_write(SSD1306_I2Cx, freq);
    i2c_write(SSD1306_I2Cx, page_end);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, 0xFF);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Experimental function that scrolls the display vertically
 *           Note: for this function to work as intended, avoid placing any combination of pixels on PAGE7
 * @param    dir: scroll direction. UP or DOWN
 * @param    freq: scroll speed in frames. Choose values from SSD1306_FrameFreq_t
 *                 the lower the frame the faster the scrolling speed.
 * @param    freeze: page to freeze, this page will not scroll vertically, only works when dir is UP
 * @retval   none
 */
void ssd1306_displayScrollVertical(SSD1306_ScrollDir_t dir, SSD1306_FrameFreq_t freq, SSD1306_PageNum_t freeze)
{
    uint8_t offset = (dir) ? 0x01 : 0x3f;
    uint8_t fixed = 8 * (freeze + 1);
    ssd1306_displayScrollDiagonal(0, freq, PAGE7, PAGE7, offset);
    ssd1306_displaySetVerticalScrollArea(fixed);
}


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
                                   SSD1306_PageNum_t page_start, SSD1306_PageNum_t page_end, uint8_t offset)
{
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, CMD_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx, 0x28 | dir);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, page_start);
    i2c_write(SSD1306_I2Cx, freq);
    i2c_write(SSD1306_I2Cx, page_end);
    i2c_write(SSD1306_I2Cx, offset);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Sets the scroll area for diagonal scrolling 
 * @param    fixed: value from 0..63. This will freeze the rows that is excluded
 *           from page_start + page_end parameter of ssd1306_displayScrollDiagonal()
 *           function. example: if page_start = PAGE1, page_end = PAGE7,
 *           fixed must be equal to 7 (size of one PAGE. value starting from 0)
 * @retval   none
 */
void ssd1306_displaySetVerticalScrollArea(uint8_t fixed)
{
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, CMD_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx, 0xA3);
    i2c_write(SSD1306_I2Cx, fixed);
    i2c_write(SSD1306_I2Cx, 64 - fixed);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Enables or disables the last scroll command issued
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayScrollState(SSD1306_FunctionalState_t state)
{
    if(state)
    {
        ssd1306_cmd_single(0x2F);
    }
    else
    {
        ssd1306_cmd_single(0x2E);
    }
}


/**
 * @brief    Set the addressing mode used by the display
 *           see the display datasheet for more information
 * @param    mode: Any one of the item under SSD1306_AddrMode_t typedef
 *                 HOR_ADDR_MODE (default)
 * @retval   none
 */
void ssd1306_displayAddrMode(SSD1306_AddrMode_t mode)
{
    ssd1306_cmd_double(0x20, mode);
}


/**
 * @brief    Flips the display horizontally
 * @param    state: TRUE or FALSE
 * @retval   none
 */
void ssd1306_displayFlip(SSD1306_Orientation_t orientation, FunctionalState state)
{
    if(orientation)
    {
        if(state)
        {
            ssd1306_cmd_single(0xA0);
        }
        else
        {
            ssd1306_cmd_single(0xA1);
        }
    }
    else
    {
        if(state)
        {
            ssd1306_cmd_single(0XC0);
        }
        else
        {
            ssd1306_cmd_single(0xC8);
        }
    }
}


/**
 * @brief    Update the entire GDDRAM
 * @param    none
 * @retval   none
 */
void ssd1306_ramUpdateFull(void)
{
    for(uint16_t i = 0; i < 1024; i++)
    {
        i2c_start(SSD1306_I2Cx);
        i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
        i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
        i2c_write(SSD1306_I2Cx,  *p_ram++ );
    }
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Update a byte of the GDDRAM
 * @param    byte_pos: address of the byte to update. value range 0..1023
 * @param    byte_val: value to be put in the byte
 *           Note: The current value stored in the GDDRAM before a call to this
 *                 function will be retain.
 * @retval   none
 */
void ssd1306_ramUpdateByte(uint16_t byte_pos, uint8_t byte_val)
{
    uint8_t y_pos = byte_pos / 128;
    uint8_t x_pos = byte_pos - (128 * y_pos);
    ssd1306_displayMoveCursor(x_pos, y_pos);
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, DATA_CTRL_BYTE);
    i2c_write(SSD1306_I2Cx,  *(p_ram + byte_pos) |= byte_val );
    i2c_stop(SSD1306_I2Cx);
}


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
void ssd1306_ramWrite(uint16_t byte_pos, uint8_t byte_val)
{
    *(p_ram + byte_pos) |= byte_val;
}


/**
 * @brief    Clears the entire GDDRAM
 * @param    none
 *           Note: This will only clear the GDDRAM, to display the result 
 *                 ssd1306_ramUpdateFull() function must be called.
 * @retval   none
 */
void ssd1306_ramClear(void)
{
    ssd1306_displayMoveCursor(0,0);
    for(uint16_t i = 0; i < 1024; i++)
    {
        *p_ram++ = 0x00;
    }
}


/**
 * @brief    Issue a double byte command to the display
 *           see the display datasheet for more information
 * @param    cmd: command byte
 * @retval   none
 */
static void ssd1306_cmd_single(uint8_t cmd)
{
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, cmd);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Issue a double byte command to the display
 *           see the display datasheet for more information
 * @param    cmd: command byte
 * @param    val: command value 
 * @retval   none
 */
static void ssd1306_cmd_double(uint8_t cmd, uint8_t val)
{    
    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, 0x00);
    i2c_write(SSD1306_I2Cx, cmd);
    i2c_write(SSD1306_I2Cx, val);
    i2c_stop(SSD1306_I2Cx);
}


/**
 * @brief    Executes display's initialization sequence
 * @param    none
 * @retval   none
 */
void ssd1306_init(void)
{
    for(uint8_t i = 0; i < 254; i++);

    i2c_start(SSD1306_I2Cx);
    i2c_request(SSD1306_I2Cx, SSD1306_SLAVE_ADDR_W);
    i2c_write(SSD1306_I2Cx, CMD_CTRL_BYTE);

    /* Entire Display OFF */
    i2c_write(SSD1306_I2Cx, 0xAE);


    /* Timing & Driving Scheme Setting */

    /* Set Display Clock Divide Ratio and Oscillator Frequency */
    i2c_write(SSD1306_I2Cx, 0xD5);
    /* Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended */
    i2c_write(SSD1306_I2Cx, 0xF0);


    /* Set pre-charge period */
    i2c_write(SSD1306_I2Cx, 0xD9);
    /* Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK */
    i2c_write(SSD1306_I2Cx, 0xF1);


    /* Set Vcomh deselect level */
    i2c_write(SSD1306_I2Cx, 0xDB);
    /* Vcomh deselect level ~ 0.77 Vcc */
    i2c_write(SSD1306_I2Cx, 0x20);


    /* Charge Pump Setting*/
    i2c_write(SSD1306_I2Cx, 0x8D);
    /* Enable charge dump during display on */
    i2c_write(SSD1306_I2Cx, 0x14);


    /* Addressing Setting */

    /* Set memory addressing mode */
    i2c_write(SSD1306_I2Cx, 0x20);
    i2c_write(SSD1306_I2Cx, 0x00);


    /* Hardware Configuration (Panel resolution & layout related) */

    /* Set Display Start Line */
    i2c_write(SSD1306_I2Cx, 0x40);
    // i2c_write(0x7F);

    /* x axis */
    /* Set Segment Re-map */
    // i2c_write(0xA0);
    i2c_write(SSD1306_I2Cx, 0xA1);

    /* Set Multiplex Ratio */
    i2c_write(SSD1306_I2Cx, 0xA8);
    i2c_write(SSD1306_I2Cx, SSD1306_HEIGHT - 1); // 64 COM lines


    /* y axis */
    /* Set COM Output scan direction */
    // i2c_write(0xC0);    // COM0 - COM63
    i2c_write(SSD1306_I2Cx, 0xC8); // COM63 - COM0

    /* Set display offset */
    i2c_write(SSD1306_I2Cx, 0xD3);
    /* 0 offset */
    i2c_write(SSD1306_I2Cx, 0x00);

    /* Set com pins hardware configuration */
    i2c_write(SSD1306_I2Cx, 0xDA);
    /* Alternative com pin configuration, disable com left/right remap */
    i2c_write(SSD1306_I2Cx, 0x12);

    /* Set contrast control */
    i2c_write(SSD1306_I2Cx, 0x81);
    /* Set Contrast to 128 */
    i2c_write(SSD1306_I2Cx, 0x80);

    /* Entire display ON, resume to RAM content display */
    i2c_write(SSD1306_I2Cx, 0xA4);

    /* Set Display in Normal Mode, 1 = ON, 0 = OFF */
    i2c_write(SSD1306_I2Cx, 0xA6);

    /* Deactivate scroll */
    i2c_write(SSD1306_I2Cx, 0x2E);

    /* Display on in normal mode */
    i2c_write(SSD1306_I2Cx, 0xAF);

    i2c_stop(SSD1306_I2Cx);


    ssd1306_displayClear();
}