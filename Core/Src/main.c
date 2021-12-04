/**
  ******************************************************************************
  * @file    main.c
  * @author  Marco, Roldan L.
  * @version v1.0
  * @date    August 29, 2021
  * @brief   SSD1306 OLED Driver test code
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


#include "stm32f10x.h"
#include "ssd1306_oled.h"
#include <stdio.h>


int main()
{
  I2C_Init_t ssd1306_i2c_conf;

  i2c_structInit(&ssd1306_i2c_conf);
  i2c_init(I2C1, &ssd1306_i2c_conf);
	ssd1306_init();

  ssd1306_drawBitmap(Launchpad_Logo);

	while(1)
	{
      ssd1306_drawBitmap(Launchpad_Logo);
	}
}