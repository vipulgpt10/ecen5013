/**
 * @file i2c_wrapper.h
 * @brief An abtraction for i2c wrapper function
 *
 * This header file provides the prototypes of the function for
 * various i2c operations using mraa/i2c library.
 *
 * @author Vipul Gupta
 * @date 09 March 2018
 *
 */

#ifndef __I2C_WRAPPER_H__
#define __I2C_WRAPPER_H__

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

/* mraa header */
#include <mraa/i2c.h>


#define I2C_BUS	(2)

/**
 * @brief Writes a byte of data using i2c
 *
 * Given the device address, register address and the data,
 * it will write the byte of data to the register specified.
 *
 * @param address Device slave address
 * @param cmd Register address
 * @param data Data byte to be written
 *
 * @return Error code (0 Success)
 */
int8_t i2c_write_byte(uint8_t address, const uint8_t cmd, 
								const uint8_t data);
/**
 * @brief Writes a word length of data using i2c
 *
 * Given the device address, register address and the data,
 * it will write the word length of data to the register specified.
 *
 * @param address Device slave address
 * @param cmd Register address
 * @param data Data byte to be written
 *
 * @return Error code (0 Success)
 */
int8_t i2c_write_word(uint8_t address, const uint8_t cmd, 
								const uint16_t data);
/**
 * @brief Reads a byte of data using i2c
 *
 * Given the device address, register address and data pointer,
 * it will read the byte of data from the register specified and
 * write it to the data variable.
 *
 * @param address Device slave address
 * @param cmd Register address
 * @param data Dat pointer
 *
 * @return Error code (0 Success)
 */
int8_t i2c_read_byte(uint8_t address, const uint8_t cmd, 
								uint8_t * data);
/**
 * @brief Reads a series of data using i2c
 *
 * Given the device address, register address , data pointer and length,
 * it will read the length of bytes of data from the register specified 
 * and write it to the data buffer.
 *
 * @param address Device slave address
 * @param cmd Register address
 * @param data Dat pointer
 *
 * @return Error code (0 Success)
 */
int8_t i2c_read_bytes(uint8_t address, const uint8_t cmd, 
								uint8_t * data, int length);

/* Exit routine */
static int8_t i2c_exit(mraa_i2c_context i2c, mraa_result_t status);


#endif	/* __I2C_WRAPPER_H__ */