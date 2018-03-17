

#ifndef __TEMP_TASK_H__
#define __TEMP_TASK_H__

#include "i2c_wrapper.h"
#include <pthread.h>

#define TMP102_DEV_ADDR		0x48

#define TMP102_TEMP_REG		0x00
#define TMP102_CONF_REG		0x01
#define TMP102_TLOW_REG		0x02
#define TMP102_THIGH_REG	0x03


int8_t read_temp_celsius(float * data);

int8_t read_temp_fahrenheit(float * data);

int8_t read_temp_kelvin(float * data);

int8_t write_config_reg(const uint16_t data);

int8_t read_config_reg(uint16_t * data);

int8_t read_tlow_celsius(float * data);

int8_t read_thigh_celsius(float * data);

void * temp_thread(void * data);

#endif