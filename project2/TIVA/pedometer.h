/**
 * @file pedometer.h
 * @brief An abtraction for pedometer sensor functions
 *
 * This header file provides the prototypes of the function for
 * various pedometer sensor operations such as step counts.
 *
 * @author Vipul Gupta
 * @date 25 April 2018
 *
 */

#ifndef __PEDOMETER_H__
#define __PEDOMETER_H__

/* Include Files */
#include <stdint.h>
#include "i2c_wrapper.h"

#include "FreeRTOS.h"
#include "semphr.h"

/************ Macro definitions *********************/

#define LSM6DS3_PEDO_SLAVE_ADDR                 (0x6B)

/******* Device Registers ************/
#define LSM6DS3_ACC_GYRO_INT1_CTRL              (0x0D)
#define LSM6DS3_ACC_GYRO_WHO_AM_I_REG           (0x0F)
#define LSM6DS3_ACC_GYRO_CTRL1_XL               (0x10)
#define LSM6DS3_ACC_GYRO_CTRL4_C                (0x13)
#define LSM6DS3_ACC_GYRO_CTRL10_C               (0x19)
#define LSM6DS3_ACC_GYRO_OUT_TEMP_L             (0x20)
#define LSM6DS3_ACC_GYRO_OUT_TEMP_H             (0x21)
#define LSM6DS3_ACC_GYRO_TAP_CFG1               (0x58)
#define LSM6DS3_ACC_GYRO_STEP_COUNTER_L         (0x4B)
#define LSM6DS3_ACC_GYRO_STEP_COUNTER_H         (0x4C)

/******* Values **********************/
#define LSM6DS_3FS_XL_2g                        (0x00)
#define LSM6DS_ODR_XL_26Hz                      (0x20)
#define LSM6DS_XL_BW_SCAL_ODR_EN                (0x80)
#define LSM6DS_PEDO_RESET_GYRO_EN               (0x3E)
#define LSM6DS_PEDO_EN                          (0x40)
#define LSM6DS_INT1_STEP_DETECTOR               (0x10)

#define LSM6DS_SUCCESS                          (1)
#define LSM6DS_FAILED                           (0)

/******************************************************************//**********
 * @brief pedometer_write()
 * This function writes data to register.
 * @reg_address: Address of the register to be written
 * @data: Data to be written
 * @return: void
 *****************************************************************************/
void pedometer_write(uint8_t reg_address, uint8_t data);

/******************************************************************//**********
 * @brief pedometer_read()
 * This function reads data from register.
 * @reg_address: Address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
uint8_t pedometer_read(uint8_t reg_address, uint8_t* data);

/**
 * @brief Initialize the pedometer
 *
 * Initialize the pedometer IMU for counting steps.
 *
 * @return void
 */
void init_pedometer(void);

/**
 * @brief Start-up test for the sensor
 *
 * After enabling the sensor, read Device ID for testing.
 *
 * @return 1 = SUCCESS, 0 = FAILED
 */
uint8_t test_pedometer(void);

/**
 * @brief Gives the step counts
 *
 * It reads the StepCount registers and returns the step count
 * stored in it.
 *
 * @return step counts
 */
uint16_t read_stepCounter(void);

/**
 * @brief Gives the temperature
 *
 * It reads the temperature registers and returns the temperature
 * in degree Celsius.
 *
 * @return Temperature in degree celsius
 */
int32_t read_temperature(void);

#endif /* __PEDOMETER_H__ */

