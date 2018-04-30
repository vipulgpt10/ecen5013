/**
 * @file i2c_wrapper.h
 * @brief An abtraction for i2c wrapper function
 *
 * This header file provides the prototypes of the function for
 * various i2c operations using TIVA I2C HAL Library library.
 *
 * @author Nagarjuna Reddy Kovuru
 * @date 26 April 2018
 *
 */
#ifndef __I2C_WRAPPER_H__
#define __I2C_WRAPPER_H__

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************

#define I2C_BUFFER_LENGTH           (32)


typedef enum
{
    I2C_READ_NULL_PTR = 0,
    SUCCESS
}i2cStatus_t;


//***********************************************************************************
// Global Variables
//***********************************************************************************

//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//**********
 * @brief i2c_read_byte()
 * This function reads single byte.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_byte(uint8_t address, uint8_t* data);

/******************************************************************//**********
 * @brief i2c_read_byte()
 * This function reads word.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_word(uint8_t address, uint16_t* data);

/******************************************************************//**********
 * @brief i2c_write_single()
 * This function writes single byte.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write_single(uint8_t address, uint8_t data);

/******************************************************************//**********
 * @brief i2c_write()
 * This function writes start byte in burst writing.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write(uint8_t address, uint8_t data);

/******************************************************************//**********
 * @brief i2c_write_temp()
 * This function writes last(finish) byte in burst writing.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write_temp(uint8_t address, uint8_t data);

/******************************************************************//**********
 * @brief i2c_init()
 * This function initializes I2C peripheral and GPIO pins(SCL&SDA)
 *****************************************************************************/
void i2c_init(void);

/******************************************************************//**********
 * @brief i2c_read_3bytes()
 * This function reads 3 bytes.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_3bytes(uint8_t address, uint32_t* data);

#endif /* __I2C_WRAPPER_H__ */
