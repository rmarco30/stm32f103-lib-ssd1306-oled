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

#include "stm32f10x.h"
#include "i2c.h"



/* I2C enums */
typedef enum
{
    NACK = 0,
    ACK = !NACK
} i2cAckBit_t;


/* Static function prototype */
static void i2c_ack_bit(I2C_TypeDef* I2Cx, i2cAckBit_t ack_nack);



/**
 * @brief    Initializes a I2C_Init_t type structure to default values
 *           Note: If the structure was declared with local scope it is not
 *           guaranteed that uninitialized members will set to zero. It is
 *           mandatory to initialize all members manually or call this
 *           function to ensure all structure members will be initialized to
 *           bare minimum configuration.
 *           Structure values after calling this function.
 *           I2C_MODE = I2C_MODE_I2C
 *           I2C_CLOCK_SPEED = 400000 (KHz)
 *           I2C_FASTMODE = I2C_FASTMODE_ENABLE
 *           I2C_OWN_ADDRESS1 = 0xC8
 *           I2C_OWN_ADDRESS2 = 0xD8
 *           The rest will be on their reset values in the datasheet.
 * @param    i2c_conf: pointer to I2C_Init_t type structure.
 * @retval   none
 */
void i2c_structInit(I2C_Init_t* i2c_conf)
{
    i2c_conf->I2C_MODE = I2C_MODE_I2C;
    i2c_conf->I2C_CLOCK_SPEED = 400000UL;
    i2c_conf->I2C_FASTMODE = I2C_FASTMODE_ENABLE;
    i2c_conf->I2C_DUTY_CYCLE = I2C_DUTY_2;
    i2c_conf->I2C_CLOCK_STRETCHING = I2C_CLK_STRETCH_ENABLE;
    i2c_conf->I2C_ADDRESSING_MODE = I2C_ADDR_MODE_7BIT;
    i2c_conf->I2C_DUAL_ADDRESSING_MODE = I2C_DUAL_ADDR_MODE_DISABLE;
    i2c_conf->I2C_OWN_ADDRESS1 = 0x7C;
    i2c_conf->I2C_OWN_ADDRESS2 = 0x7A;
    i2c_conf->I2C_GENERAL_CALL = I2C_GENERAL_CALL_DISABLE;
    i2c_conf->I2C_PEC = I2C_PEC_DISABLE;
    i2c_conf->I2C_ARP = I2C_ARP_DISABLE;
    i2c_conf->I2C_DMA_TRANSFER = I2C_DMA_DISABLE;
}



/**
 * @brief    Initializes I2Cx and its GPIO
 * @param    none
 * @retval   none
 */
void i2c_init(I2C_TypeDef* I2Cx, I2C_Init_t* i2c_conf)
{
    /* Small delay to ensures stable VDD */
    for(uint16_t i = 0; i < 1000; i++);

    uint32_t i2c_base = (uint32_t)I2Cx;

    RCC->APB2ENR |= ( RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN );

    switch(i2c_base)
    {
        case I2C1_BASE:

            RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

            #if (!USE_I2C_REMAP)
            
            GPIOB->CRL |= (GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
            GPIOB->CRL |= (GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
            GPIOB->BSRR |= (GPIO_BSRR_BS6 | GPIO_BSRR_BS7);

            #else

            GPIOB->CRH |= (GPIO_CRH_CNF8 | GPIO_CRH_MODE8);
            GPIOB->CRH |= (GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
            GPIOB->BSRR |= (GPIO_BSRR_BS8 | GPIO_BSRR_BS9);

            #endif

            break;

        case I2C2_BASE:

            RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

            GPIOB->CRH |= (GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
            GPIOB->CRH |= (GPIO_CRH_CNF11 | GPIO_CRH_MODE11);
            GPIOB->BSRR |= (GPIO_BSRR_BS10 | GPIO_BSRR_BS11);

            break;

        default:
            /* If you reached here you your I2Cx address is wrong */
            break;
    }


    /* Perform a I2C peripheral reset */
    I2Cx->CR1 |= I2C_CR1_SWRST;
    for(uint8_t i = 0; i < 100; i++);
    I2Cx->CR1 &= ~( I2C_CR1_SWRST );

    /* Set CR1 values */
    I2Cx->CR1 |= ( i2c_conf->I2C_MODE |
                   i2c_conf->I2C_ARP |
                   i2c_conf->I2C_PEC |
                   i2c_conf->I2C_GENERAL_CALL |
                   i2c_conf->I2C_CLOCK_STRETCHING );
    

    /* Set this mcu's slave address 1 and 2 if dual addressing mode
       is enabled */
    I2Cx->OAR1 |= (1 << 14U) | ( i2c_conf->I2C_OWN_ADDRESS1 << 1U ) ;

    if( i2c_conf->I2C_DUAL_ADDRESSING_MODE == 0x0001)
    {
        I2Cx->OAR2 |= i2c_conf->I2C_DUAL_ADDRESSING_MODE;
        I2Cx->OAR2 |= i2c_conf->I2C_OWN_ADDRESS2 << 1;
    }

    /* Configure FREQ bits based on APB1 peripheral clock speed*/
    uint32_t apb1_prescaler = (RCC->CFGR & 0x700) >> 8;

    switch(apb1_prescaler)
    {
        case 4:
            I2Cx->CR2 |= ( SystemCoreClock / 2000000 );
            break;
        case 5:
            I2Cx->CR2 |= ( SystemCoreClock / 4000000 );
            break;
        case 6:
            I2Cx->CR2 |= ( SystemCoreClock / 8000000 );
            break;
        case 7:
            I2Cx->CR2 |= ( SystemCoreClock / 16000000 );
        default:
            I2Cx->CR2 |= SystemCoreClock;
            break;
    }

    /* Configure CCR register */
    I2Cx->CCR |= ( i2c_conf->I2C_FASTMODE | i2c_conf->I2C_DUTY_CYCLE );

    /* Configure I2C SCL to prefered speed */
    uint16_t tmp = ( SystemCoreClock / (4 * i2c_conf->I2C_CLOCK_SPEED) );

    if( tmp < 0x04 )
    {
        if( ((I2Cx->CCR & I2C_CCR_FS) != 0) && ((I2Cx->CCR & I2C_CCR_DUTY) != 0) )
        {
            /* Minimum allowed value for FAST DUTY mode */
            tmp = 0x01;
        }
        else
        {
            /* Minimum allowed value for any other configuration */
            tmp = 0x04;
        }
    }
    I2Cx->CCR |= tmp;


    /* Configure SCL rise time */
    uint32_t rise_time = 0;

    if( i2c_conf->I2C_CLOCK_SPEED == 400000 && ((I2Cx->CCR & I2C_CCR_FS) != 0) )
    {
        /* If Fast mode */
        /* Value acquired from I2C specifications */
        rise_time = 300;    /* 300 ns */
    }
    else
    {
        rise_time = 1000;   /* 1000 ns */
    }

    I2Cx->TRISE = ( ((rise_time * I2Cx->CR2) / 1000) + 1 );

    /* Enable I2Cx */
    I2Cx->CR1 |= I2C_CR1_PE;
}



/**
 * @brief    Issue a start condition. When this function is
 *           called without calling stop first then this will
 *           be treated as a restart condition.
 * @param    none
 * @retval   none
 */
void i2c_start(I2C_TypeDef* I2Cx)
{
    I2Cx->CR1 |= I2C_CR1_START;
}



/**
 * @brief    Issue a stop condition to release the line
 * @param    none
 * @retval   none
 */
void i2c_stop(I2C_TypeDef* I2Cx)
{
    I2Cx->CR1 |= I2C_CR1_STOP;
}



/**
 * @brief    Issue a ACK or NACK. This function is not usually
 *           called explicitly, most of the time this is
 *           auto-generated by the hardware.
 * @param    ack_bit: ACK or NACK
 * @retval   none
 */
static void i2c_ack_bit(I2C_TypeDef* I2Cx, i2cAckBit_t ack_bit)
{
    if(ack_bit)
    {
        I2Cx->CR1 |= I2C_CR1_ACK;
    }
    else
    {
        I2Cx->CR1 &= ~(I2C_CR1_ACK);
    }
}



/**
 * @brief    This function is called after issuing a start condition,
 *           this initiates the communication to slave device.
 * @param    slave_addr_rw: pre-shifted slave address and pre-appended RnW bit
 * @retval   none
 */
void i2c_request(I2C_TypeDef* I2Cx, uint8_t slave_addr_rw)
{
    /* EV5 - SB = 1 */
    while( !(I2Cx->SR1 & I2C_SR1_SB) );         
    I2Cx->DR = slave_addr_rw;

    /* EV6 - ADDR = 1 */
    while( !((I2Cx->SR1 & I2C_SR1_ADDR)) );     
}



/**
 * @brief    Transmit a byte of data
 * @param    data: 1 byte data to be transmitted
 * @retval   none
 */
void i2c_write(I2C_TypeDef* I2Cx, uint8_t data)
{
    /* EV6 - address matched, ADDR = 1. Clear ADDR bit */
    I2Cx->SR2 = I2Cx->SR2;
    /* EV8_1 - Write data to DR */
    while ( !(I2Cx->SR1 & I2C_SR1_TXE ));
    I2Cx->DR = data;
    /* EV8_2 - data byte transmitted */
    while( (!(I2Cx->SR1 & I2C_SR1_BTF)) && (!(I2Cx->SR1 & I2C_SR1_TXE)) );
    /* Issue a stop condition after exiting this function */
}



/**
 * @brief    Transmit a N byte of data
 * @param    mode: MASTER or SLAVE transmitter
 * @param    data_bytes: number of bytes to transmit
 * @param    data_buffer: pointer to array where data are stored
 * @retval   none
 */
void i2c_write_burst(I2C_TypeDef* I2Cx, i2cMode_t mode, uint8_t data_bytes, uint8_t *data_buffer)
{
    if( mode )
    {
        /* EV6 - address matched, ADDR = 1. Clear ADDR bit */
        I2Cx->SR2 = I2Cx->SR2;
        /* EV8_1 - Loop through the buffer to transmit data */
        for(uint8_t i = 0; i != data_bytes; i++)
        {
            while ( !(I2Cx->SR1 & I2C_SR1_TXE) );   
            I2Cx->DR = *(data_buffer + i);
        }
        /* EV8_2 - All data bytes transmitted */
        while( (!(I2Cx->SR1 & I2C_SR1_BTF)) || (!(I2Cx->SR1 & I2C_SR1_TXE)) );
        /* Issue a stop condition after exiting this function */
    }
    else
    {
        /* Set ACK bit before transmission starts */
        i2c_ack_bit(I2Cx, ACK);
        /* EV1 - Address matched, clear ADDR bit */
        while( !((I2Cx->SR1 & I2C_SR1_ADDR)) );
        I2Cx->SR2 = I2Cx->SR2;

        uint8_t j = 0;
        /* EV3-1 - Loop through the buffer to transmit
           data until NACK is received */
        while( !(I2Cx->SR1 & I2C_SR1_AF) )
        {
            if(data_bytes > 1)
            {
                I2Cx->DR = *(data_buffer + j);
                j++;
            }
            else
            {
                I2Cx->DR = *(data_buffer);
            }
            /* Wait for ACK from master after each byte */
            while ( !(I2Cx->SR1 & I2C_SR1_TXE) );
        }
        /* EV3-2 - NACK received, AF = 1, clear AF bit */
        I2Cx->SR1 &= ~( I2C_SR1_AF );
    }
}



/**
 * @brief    Receives a byte of data
 *           Note: Stop condition is not required to call explicitly
 *           after each call to this function. This receiving sequence
 *           handles it already.
 * @param    none
 * @retval   1 byte of data from slave
 */
uint8_t i2c_read(I2C_TypeDef* I2Cx)
{
    /* This procedure is only applicable for 1 byte reception */

    /* Clear ACK bit before reception starts */
    i2c_ack_bit(I2Cx, NACK);
    /* EV6_3 - Clear ADDR bit, issue a stop condition */
    I2Cx->SR2 = I2Cx->SR2;
    i2c_stop(I2Cx);

    /* EV7 - Data byte received, read DR */
    while( !(I2Cx->SR1 & I2C_SR1_RXNE) );
    return I2Cx->DR;
}



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
void i2c_read_burst(I2C_TypeDef* I2Cx, i2cMode_t mode, uint8_t data_bytes, uint8_t *data_buffer)
{
    if( mode )
    {
        if(data_bytes == 2)
        {
            /* This procedure is only applicable for 2 bytes reception */

            /* Set POS and ACK bit before reception starts */
            I2Cx->CR1 |= I2C_CR1_POS;
            i2c_ack_bit(I2Cx, ACK);

            /* EV6 - Clear ADDR1 then clear ACK bit */
            I2Cx->SR2 = I2Cx->SR2;
            i2c_ack_bit(I2Cx, NACK);
            
            /* EV7_3 - Data1 in DR, Data2 in shift register, BTF is set */
            while( !(I2Cx->SR1 & I2C_SR1_BTF) );
            i2c_stop(I2Cx);

            /* Read Data1 */
            *(data_buffer + 0) = I2Cx->DR;
            /* Read Data2 */
            *(data_buffer + 1) = I2Cx->DR;
        }

        else if(data_bytes > 2)
        {
            /* This procedure is only applicable for reception of N > 2 bytes */

            /* Set ACK bit to automatically send ack after each byte */
            i2c_ack_bit(I2Cx, ACK);

            /* EV6 - Clear ADDR1 */
            I2Cx->SR2 = I2Cx->SR2;

            uint8_t j = 0;
            /* EV7 - Receive each byte until only 3 remains */
            for(uint8_t i = data_bytes; i != 3; i--)
            {
                while( !(I2Cx->SR1 & I2C_SR1_RXNE) );
                *(data_buffer + j) = I2Cx->DR;
                j++;
            }

            /* When 3 bytes remain to read */

            /* EV7_2 - DataN-2 in DR, DataN-1 in shift register,
            BTF is set, clear the ACK bit to NACK the last byte (DataN),
            issue a stop after reading DataN-2 */
            while( !(I2Cx->SR1 & I2C_SR1_BTF) );
            i2c_ack_bit(I2Cx, NACK);
            
            /* Read DataN-2, this will move DataN-1 to DR, and receive
            DataN to shift register */
            *(data_buffer + j) = I2Cx->DR;              
            j++;
            i2c_stop(I2Cx);

            /* Read DataN-1, DataN will move to DR*/
            while( !(I2Cx->SR1 & I2C_SR1_BTF) );
            *(data_buffer + j) = I2Cx->DR;              
            j++;

            /* Read DataN, all bytes received, NACK will be automatically
            generated */
            *(data_buffer + j) = I2Cx->DR;              
            j++;        
        }

        else
        {
            /* data_bytes must be >= 2 */
            while(1);
        }
    }

    else
    {
        /* Set ACK bit before reception starts */
        i2c_ack_bit(I2Cx, ACK);
        /* EV1 - Address matched, clear ADDR bit */
        while( !((I2Cx->SR1 & I2C_SR1_ADDR)) );
        I2Cx->SR2 = I2Cx->SR2;

        uint8_t j = 0;
        while( !(I2Cx->SR1 & I2C_SR1_STOPF) )
        {
            /* EV2 - Receive each byte */
            while( (I2Cx->SR1 & I2C_SR1_RXNE) )
            {
                *(data_buffer + j) = I2Cx->DR;
                j++;
            }
        }
        /* EV4 - Stop bit detected */
        I2Cx->CR1 = I2Cx->CR1;
    }
}


#if 0

/**
 * @brief    Initialize the I2Cx with minimal configuration
 * @param    none
 * @retval   none
 */     
static void i2c_config(void)
{
    /* Perform a I2C peripheral reset */
    I2Cx->CR1 |= I2C_CR1_SWRST;
    for(uint8_t i = 0; i < 100; i++);
    I2Cx->CR1 &= ~( I2C_CR1_SWRST );

    /* Set this mcu's slave address */
    I2Cx->OAR1 |= ( STM32F1_SLV_ADDR << 1 );

    /* Peripheral clock frequency 36 MHz */
    I2Cx->CR2 = 0x24;

    /* Configure I2C SCL to 100 KHz */
    I2Cx->CCR = 0xB4;

    /* Configure SCL rise time */
    I2Cx->TRISE = 0x25;

    /* Enable I2Cx */
    I2Cx->CR1 |= I2C_CR1_PE;
}



/**
 * @brief    Configure I2Cx associated pins (SDA1/SCL1)
 * @param    none
 * @retval   none
 */
static void i2c_gpio(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= ( RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN );

    /* PB6 - SCL, PB7 - SDA  */
    /* Alternate function output Open-drain, 50 MHz */

    GPIOB->CRL &= ~( GPIO_CRL_CNF6_1 | GPIO_CRL_CNF6_0 |
                     GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0 );

    GPIOB->CRL &= ~( GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0 |
                     GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 );

    GPIOB->CRL |= ( GPIO_CRL_CNF6_1 | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0 );
    GPIOB->CRL |= ( GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 );

    GPIOB->BSRR |= ( GPIO_BSRR_BS6 | GPIO_BSRR_BS7 );
}
#endif