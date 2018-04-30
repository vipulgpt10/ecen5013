/**
 * @file pedometer.c
 * @brief The implementation file for pedometer sensor APIs
 *
 * This implementation file provides the function definitions for
 * various pedometer sensor APIs.
 *
 * @author Vipul Gupta
 * @date 25 April 2018
 *
 */


#include "pedometer.h"

extern xSemaphoreHandle i2c_lock;

void pedometer_write(uint8_t reg_address, uint8_t data)
{
    xSemaphoreTake(i2c_lock, portMAX_DELAY);
    /* write register to be written*/
    i2c_write(LSM6DS3_PEDO_SLAVE_ADDR,reg_address);
    /* write data*/
    i2c_write_temp(LSM6DS3_PEDO_SLAVE_ADDR,data);
    xSemaphoreGive(i2c_lock);

}


uint8_t pedometer_read(uint8_t reg_address, uint8_t * data)
{
    xSemaphoreTake(i2c_lock, portMAX_DELAY);
    /* write register to be read*/
    i2c_write_single(LSM6DS3_PEDO_SLAVE_ADDR,reg_address);
    /* read data*/
    i2c_read_byte(LSM6DS3_PEDO_SLAVE_ADDR, data);
    xSemaphoreGive(i2c_lock);
    return LSM6DS_SUCCESS;


}

void init_pedometer(void)
{
    /* Setup the Accelerometer */
    pedometer_write(LSM6DS3_ACC_GYRO_CTRL1_XL, (LSM6DS_3FS_XL_2g | LSM6DS_ODR_XL_26Hz));

    /* Reset Pedometer count and enable embedded functions */
    pedometer_write(LSM6DS3_ACC_GYRO_CTRL10_C, LSM6DS_PEDO_RESET_GYRO_EN);

    /* Enable Pedometer algorithm */
    pedometer_write(LSM6DS3_ACC_GYRO_TAP_CFG1, LSM6DS_PEDO_EN);

}

uint8_t test_pedometer(void)
{
    uint8_t device_ID = 0;

    /* Read WHO_AM_I register */
    pedometer_read(LSM6DS3_ACC_GYRO_WHO_AM_I_REG, &device_ID);

    if(device_ID == 0x69)
        return LSM6DS_SUCCESS;
    else
        return LSM6DS_FAILED;
}

uint16_t read_stepCounter(void)
{
    uint8_t dataRead;
    uint16_t stepsTaken;

    pedometer_read(LSM6DS3_ACC_GYRO_STEP_COUNTER_H, &dataRead);
    stepsTaken = ((uint16_t)dataRead) << 8;

    pedometer_read(LSM6DS3_ACC_GYRO_STEP_COUNTER_L, &dataRead);
    stepsTaken |= dataRead;

    return (stepsTaken);
}

int32_t read_temperature(void)
{
    uint8_t dataRead;
    int16_t temp;
    float temperature = 0;

    pedometer_read(LSM6DS3_ACC_GYRO_OUT_TEMP_H, &dataRead);
    temp = ((int16_t)dataRead) << 8;

    pedometer_read(LSM6DS3_ACC_GYRO_OUT_TEMP_L, &dataRead);
    temp |= dataRead;

    temperature = (temp * 0.0625) + 25.0;

    return ((int32_t)temperature);
}

