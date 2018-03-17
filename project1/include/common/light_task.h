

#ifndef __LIGHT_TASK_H__
#define __LIGHT_TASK_H__

#include "i2c_wrapper.h"
#include <math.h>
#include <pthread.h>

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


int8_t write_control_reg(const uint8_t data);

int8_t read_control_reg(uint8_t * data);

int8_t write_timing_reg(const uint8_t data);

int8_t read_timing_reg(uint8_t * data);

int8_t write_intthresh_low_reg(const uint16_t data);

int8_t read_intthresh_low_reg(uint16_t * data);

int8_t write_intthresh_high_reg(const uint16_t data);

int8_t read_intthresh_high_reg(uint8_t * data);

int8_t write_intcontrol_reg(const uint8_t data);

int8_t read_intcontrol_reg(uint8_t * data);

int8_t read_id_reg(uint8_t * data);

int8_t read_sensor_lux(float * data);

void * light_thread(void * data);

#endif

