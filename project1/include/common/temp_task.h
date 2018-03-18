/**
* @file temp_task.h
* @brief This file contains temperature task functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Vipul Gupta
* @date March 14 2018
*
*/
#ifndef __TEMP_TASK_H__
#define __TEMP_TASK_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c_wrapper.h"
#include "main_task.h"
#include <pthread.h>

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* shared memory name and size */
#define TEMPTASK_SM_NAME	   "tempTask_sharedMemory"

#define TMP102_DEV_ADDR		0x48

#define TMP102_TEMP_REG		0x00
#define TMP102_CONF_REG		0x01
#define TMP102_TLOW_REG		0x02
#define TMP102_THIGH_REG	0x03

//***********************************************************************************
// Global Variables 
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//**********
 * @brief read_temp_celsius()
 * This function reads temperature in celcius
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_celsius(float * data);

/******************************************************************//**********
 * @brief read_temp_fahrenheit()
 * This function reads temperature in farenheit
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_fahrenheit(float * data);

/******************************************************************//**********
 * @brief read_temp_kelvin()
 * This function reads temperature in kelvins
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_kelvin(float * data);

/******************************************************************//**********
 * @brief write_config_reg()
 * This function writes configuration register
 * @data: data to write
 *****************************************************************************/
int8_t write_config_reg(const uint16_t data);

/******************************************************************//**********
 * @brief read_config_reg()
 * This function reads configuration register
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_config_reg(uint16_t * data);

/******************************************************************//**********
 * @brief read_tlow_celsius()
 * This function reads tlow
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_tlow_celsius(float * data);

/******************************************************************//**********
 * @brief read_thigh_celsius()
 * This function reads thigh
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_thigh_celsius(float * data);

/******************************************************************//**********
 * @brief temp_start_test()
 * This function checks the Config register default value and return 0
 * on success and -1 on failure.
 *****************************************************************************/
int temp_start_test(void);

int get_temp_cel(API_message_t * ptr);

int get_temp_kel(API_message_t * ptr);

int get_temp_fah(API_message_t * ptr);

/******************************************************************//**********
 * @brief temperature_task_thread()
 * This thread initializes the temperature task resources such as shared memory
 *****************************************************************************/
void temperature_task_thread(void);

#endif
