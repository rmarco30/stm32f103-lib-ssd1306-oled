/**
  ******************************************************************************
  * @file    i2c.h
  * @author  Marco, Roldan L.
  * @version v1.0
  * @date    August 27, 2021
  * @brief   I2C peripheral driver
  *   
  *          The I2C APIs present here follows the transfer sequence for reading 
  *          and writing described in the reference manual (RM0008).
  * 
  *          Device used: Bluepill (STM32F103C8)
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

#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

/* Own address used when in SLAVE mode */
#define USE_I2C_REMAP               0




typedef struct
{
    uint16_t I2C_MODE;
    uint32_t I2C_CLOCK_SPEED;
    uint16_t I2C_FASTMODE;
    uint16_t I2C_DUTY_CYCLE;
    uint16_t I2C_CLOCK_STRETCHING;
    uint16_t I2C_ADDRESSING_MODE;
    uint16_t I2C_DUAL_ADDRESSING_MODE;
    uint16_t I2C_OWN_ADDRESS1;
    uint16_t I2C_OWN_ADDRESS2;
    uint16_t I2C_GENERAL_CALL;
    uint16_t I2C_PEC;
    uint16_t I2C_ARP;
    uint16_t I2C_DMA_TRANSFER;

} I2C_Init_t;


/* I2C_MODE */
#define I2C_MODE_I2C                          (uint16_t)0x0000;
#define I2C_MODE_SMBUS_DEVICE                 (uint16_t)0x0002;
#define I2C_MODE_SMBUS_HOST                   (uint16_t)0x000A;

/* I2C_FASTMODE */
#define I2C_FASTMODE_ENABLE                   (uint16_t)0x8000;
#define I2C_FASTMODE_DISABLE                  (uint16_t)0x0000;

/* I2C_DUTY_CYCLE */
#define I2C_DUTY_2                            (uint16_t)0x0000;
#define I2C_DUTY_16_9                         (uint16_t)0x4000;

/* I2C_CLOCK_STRETCHING */
#define I2C_CLK_STRETCH_ENABLE                (uint16_t)0x0000;
#define I2C_CLK_STRETCH_DISABLE               (uint16_t)0x0080;

/* I2C_ADDRESSING_MODE */
#define I2C_ADDR_MODE_7BIT                    (uint16_t)0x0000;
#define I2C_ADDR_MODE_10BIT                   (uint16_t)0x8000;

/* I2C_DUAL_ADDRESSING_MODE */
#define I2C_DUAL_ADDR_MODE_ENABLE             (uint16_t)0x0001;
#define I2C_DUAL_ADDR_MODE_DISABLE            (uint16_t)0x0000;

/* I2C_GENERAL_CALL */
#define I2C_GENERAL_CALL_ENABLE               (uint16_t)0x0040;
#define I2C_GENERAL_CALL_DISABLE              (uint16_t)0x0000;

/* I2C_PEC */
#define I2C_PEC_ENABLE                        (uint16_t)0x0020;
#define I2C_PEC_DISABLE                       (uint16_t)0x0000;

/* I2C_ARP */
#define I2C_ARP_ENABLE                        (uint16_t)0x0010;
#define I2C_ARP_DISABLE                       (uint16_t)0x0000;

/* I2C_DMA_TRANSFER */
#define I2C_DMA_ENABLE                        (uint16_t)0x0800;
#define I2C_DMA_DISABLE                       (uint16_t)0x0000;


typedef enum
{
    SLAVE = 0,
    MASTER = !SLAVE
} i2cMode_t;






void i2c_structInit(I2C_Init_t* i2c_conf);


/**
 * @brief    Initializes I2Cx and its GPIO
 * @param    I2Cx: either I2C1 or I2C2
 * @param    i2c_clk_speed: I2Cx clock speed in hertz, should not exceed (400 Khz)
 * @retval   none
 */
void i2c_init(I2C_TypeDef* I2Cx, I2C_Init_t* i2c_conf);



/**
 * @brief    Issue a start condition. When this function is
 *           called without calling stop first then this will
 *           be treated as a restart condition.
 * @param    none
 * @retval   none
 */
void i2c_start(I2C_TypeDef* I2Cx);



/**
 * @brief    Issue a stop condition to release the line
 * @param    none
 * @retval   none
 */
void i2c_stop(I2C_TypeDef* I2Cx);



/**
 * @brief    This function is called after issuing a start condition,
 *           this initiates the communication to slave device.
 * @param    slave_addr_rw: pre-shifted slave address and pre-appended RnW bit
 * @retval   none
 */
void i2c_request(I2C_TypeDef* I2Cx, uint8_t slave_addr_rw);



/**
 * @brief    Transmit a byte of data
 * @param    data: 1 byte data to be transmitted
 * @retval   none
 */
void i2c_write(I2C_TypeDef* I2Cx, uint8_t data);



/**
 * @brief    Transmit a N byte of data
 * @param    mode: MASTER or SLAVE transmitter
 * @param    data_bytes: number of bytes to transmit
 * @param    data_buffer: pointer to array where data are stored
 * @retval   none
 */
void i2c_write_burst(I2C_TypeDef* I2Cx, i2cMode_t mode, uint8_t data_bytes, uint8_t *data_buffer);



/**
 * @brief    Receives a byte of data
 *           Note: Stop condition is not required to call explicitly
 *           after each call to this function. This receiving sequence
 *           handles it already.
 * @param    none
 * @retval   1 byte of data from slave
 */
uint8_t i2c_read(I2C_TypeDef* I2Cx);



/**
 * @brief    Receives N bytes of data (N must be >= 2)
 *           Note: Stop condition is not required to call explicitly
 *           after each call to this function. This receiving sequence
 *           handles it already.
 * @param    mode: MASTER or SLAVE receiver
 * @param    data_bytes: number of bytes to receive. When in SLAVE mode
 *                       this parameter is ignored.
 * @param    data_buffer: pointer to array where data will be stored
 * @retval   none
 */
void i2c_read_burst(I2C_TypeDef* I2Cx, i2cMode_t mode, uint8_t data_bytes, uint8_t *data_buffer);


#endif