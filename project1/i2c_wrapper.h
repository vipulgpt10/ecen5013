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


mraa_result_t i2c_write_byte(uint8_t address, const uint8_t cmd, 
								const uint8_t data);


mraa_result_t i2c_write_word(uint8_t address, const uint8_t cmd, 
								const uint16_t data);


mraa_result_t i2c_read_byte(uint8_t address, const uint8_t cmd, 
								uint8_t data *);


mraa_result_t i2c_read_bytes(uint8_t address, const uint8_t cmd, 
								uint8_t data *, int length);


static mraa_result_t i2c_exit(mraa_i2c_context i2c, mraa_result_t status);

/**
 * @brief Moves the data from source to destination
 *
 * Given the source and destination pointers and the length of the data,
 * it will move the data from source to destination and return the 
 * pointer to the destination.
 *
 * @param src Pointer to the source
 * @param dst Pointer to the destination
 * @param length Size of the data in bytes
 *
 * @return Pointer to the destination
 */


#endif	/* __I2C_WRAPPER_H__ */