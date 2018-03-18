/**
* @file light_task.h
* @brief This file contains light task functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Kovuru Nagarjuna Reddy
* @date March 14 2018
*
*/
#ifndef __LIGHT_TASK_H__
#define __LIGHT_TASK_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c_wrapper.h"
#include "main_task.h"
#include <math.h>
#include <pthread.h>

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* shared memory name and size */
#define LIGHTTASK_SM_NAME	   "lightTask_sharedMemory"

/* Macro definitions */
#define APDS_SENS_DEV_ADDR  0x39

#define APDS_CMD_BYTE_REG   0x80
#define APDS_CMD_WORD_REG   0xA0


#define APDS_CONTROL_REG    0x00
#define APDS_TIMING_REG		0x01
#define APDS_INTTHLOW_REG	0x02
#define APDS_INTTHHIGH_REG	0x04
#define APDS_INTCTRL_REG	0x06
#define APDS_ID_REG         0x0A
#define APDS_D0LOW_REG      0x0C
#define APDS_D1LOW_REG      0x0E

#define POWER_ON			0x03
#define POWER_OFF			0x00
#define	INT_TIME_00			0x00	/* 13.7 ms */
#define	INT_TIME_01			0x01	/* 101 ms */
#define	INT_TIME_02			0x02	/* 402 ms (default) */
#define LOW_GAIN			(0<<4)
#define HIGH_GAIN			(1<<4)
#define INTR_ON				(1<<4)
#define INTR_OFF			(0<<4)

//***********************************************************************************
// Global Variables 
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//**********
 * @brief write_control_reg()
 * This function writes sensor control register with data,
 * @data: data to write
 *****************************************************************************/
int8_t write_control_reg(const uint8_t data);

/******************************************************************//**********
 * @brief read_control_reg()
 * This function reads sensor control register,
 * @data: pointer to store read data
 *****************************************************************************/
int8_t read_control_reg(uint8_t * data);

/******************************************************************//**********
 * @brief write_timing_reg()
 * This function writes to timing register,
 * @data: data to write
 *****************************************************************************/
int8_t write_timing_reg(const uint8_t data);

/******************************************************************//**********
 * @brief read_timing_reg()
 * This function reads timing register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_timing_reg(uint8_t * data);

/******************************************************************//**********
 * @brief write_intthresh_low_reg()
 * This function writes to intthreshold low register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intthresh_low_reg(const uint16_t data);

/******************************************************************//**********
 * @brief read_intthresh_low_reg()
 * This function reads intthreshold register,
 * @data: pointer to stor ethe read data
 *****************************************************************************/
int8_t read_intthresh_low_reg(uint16_t * data);

/******************************************************************//**********
 * @brief write_intthresh_high_reg()
 * This function writes to inthreshold register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intthresh_high_reg(const uint16_t data);

/******************************************************************//**********
 * @brief read_intthresh_high_reg()
 * This function reads intthreshold register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_intthresh_high_reg(uint8_t * data);

/******************************************************************//**********
 * @brief write_intcontrol_reg()
 * This function writes to intcontrol register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intcontrol_reg(const uint8_t data);

/******************************************************************//**********
 * @brief read_intcontrol_reg()
 * This function reads intcontrol register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_intcontrol_reg(uint8_t * data);

/******************************************************************//**********
 * @brief read_id_reg()
 * This function reads id register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_id_reg(uint8_t * data);

/******************************************************************//**********
 * @brief read_sensor_lux()
 * This function reads the sensor lux,
 * @data: pointer to strore the read data
 *****************************************************************************/
int8_t read_sensor_lux(float * data);

/******************************************************************//**********
 * @brief light_start_test()
 * This function checks the ID register and return 0 on success and -1 
 * failure.
 *****************************************************************************/
int light_start_test(void);

int sensor_lux_req(API_message_t * ptr);

/******************************************************************//**********
 * @brief light_task_thread()
 * This thread initializes the light task resources such as shared memory
 *****************************************************************************/
void light_task_thread(void);

#endif

