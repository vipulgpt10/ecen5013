/**
 * @file i2c_wrapper.c
 * @brief The implementation file for i2c wrapper functions
 *
 * This implementation file provides the function definitions for
 * various i2c wrapper operations.
 *
 * @author Vipul Gupta
 * @date 09 March 2018
 *
 */

#include "i2c_wrapper.h"


int8_t i2c_write_byte(uint8_t address, const uint8_t cmd, 
								const uint8_t data)
{
	mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;


    i2c = mraa_i2c_init_raw(I2C_BUS);
    if (i2c == NULL) 
    {
        fprintf(stderr, "Failed to initialize I2C\n");
   
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, address);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    status = mraa_i2c_write_byte_data(i2c, data, cmd);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform  */

    return EXIT_SUCCESS;

}


int8_t i2c_write_word(uint8_t address, const uint8_t cmd, 
								const uint16_t data)
{
	mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;


    i2c = mraa_i2c_init_raw(I2C_BUS);
    if (i2c == NULL) 
    {
        fprintf(stderr, "Failed to initialize I2C\n");
   
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, address);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    status = mraa_i2c_write_word_data(i2c, data, cmd);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform  */

    return EXIT_SUCCESS;
}


int8_t i2c_read_byte(uint8_t address, const uint8_t cmd,
								uint8_t * data)
{
	mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;
    int8_t ret;


    i2c = mraa_i2c_init_raw(I2C_BUS);
    if (i2c == NULL) 
    {
        fprintf(stderr, "Failed to initialize I2C\n");
   
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, address);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    ret = mraa_i2c_read_byte_data(i2c, cmd);
    if (ret == -1)
    	return i2c_exit(i2c, status);

    *data = ret;

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform  */

    return EXIT_SUCCESS;
}


int8_t i2c_read_bytes(uint8_t address, const uint8_t cmd, uint8_t * data,
								 int length)
{
	mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;
    int8_t ret;


    i2c = mraa_i2c_init_raw(I2C_BUS);
    if (i2c == NULL) 
    {
        fprintf(stderr, "Failed to initialize I2C\n");
   
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, address);
    if (status != MRAA_SUCCESS)
    	return i2c_exit(i2c, status);

    ret = mraa_i2c_read_bytes_data(i2c, cmd, data, length);
    if (ret == -1)
    	return i2c_exit(i2c, status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform  */

    return EXIT_SUCCESS;
}

static int8_t i2c_exit(mraa_i2c_context i2c, mraa_result_t status)
{
	mraa_result_print(status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform */

    return EXIT_FAILURE;
}