/* Light task */

#include "light_task.h"




int8_t write_control_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}


int8_t read_control_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}

int8_t write_timing_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

int8_t read_timing_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

int8_t write_intthresh_low_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHLOW_REG), data);
}

int8_t read_intthresh_low_reg(uint16_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTTHLOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    val = ((uint16_t)arr[1] << 8 | arr[0]);
    *data = val;
    return ret;
}

int8_t write_intthresh_high_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHHIGH_REG), data);
}

int8_t read_intthresh_high_reg(uint8_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTTHHIGH_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    val = ((uint16_t)arr[1] << 8 | arr[0]);
    *data = val;
    return ret;
}

int8_t write_intcontrol_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

int8_t read_intcontrol_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

int8_t read_id_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_ID_REG), data);
}

int8_t read_sensor_lux(float * data)
{
	int8_t ret;
	float s_lux;
	uint8_t arr[2];
	uint16_t ch0, ch1;

	ret = i2c_read_bytes(APDS_SENS_DEV_ADDR, 
    			(APDS_CMD_BYTE_REG | APDS_D0LOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    ch0 = ((uint16_t)arr[1] << 8 | arr[0]);

    printf("ch0 : %0.4x\n", ch0);

    ret = i2c_read_bytes(APDS_SENS_DEV_ADDR, 
    			(APDS_CMD_BYTE_REG | APDS_D1LOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    ch1 = ((uint16_t)arr[1] << 8 | arr[0]);

    printf("ch1 : %0.4x\n", ch1);

    if(((float)ch1/ch0) <= 0.5 &&  ((float)ch1/ch0) > 0)
   		s_lux = (0.0304 * ch0) - (0.062 * ch0 * pow(((float)ch1/ch0), 1.4));

   	else if(((float)ch1/ch0) <= 0.61 &&  ((float)ch1/ch0) > 0.5)
   		s_lux = (0.0224 * ch0) - (0.031 * ch1);

	else if(((float)ch1/ch0) <= 0.80 &&  ((float)ch1/ch0) > 0.61)
   		s_lux = (0.0128 * ch0) - (0.0153 * ch1);

	else if(((float)ch1/ch0) <= 1.3 &&  ((float)ch1/ch0) > 0.80)
   		s_lux = (0.00146 * ch0) - (0.00112 * ch1);
   	else
   		s_lux = 0;

   	*data = s_lux;

    return ret;

}


void * light_thread(void * data)
{
    /* Test code */

    int8_t ret;
    uint8_t temp;
    float lux;
    uint16_t thr;

    write_control_reg(POWER_ON);

    read_control_reg(&temp);
   
    printf("Control reg: 0x%0.2x\n", temp);

    read_id_reg(&temp);

    printf("ID reg: %0.2x\n", temp);

    write_intthresh_low_reg(0x0101);

    read_intthresh_low_reg(&thr);
    printf("Threshold: 0x%0.4x\n", thr);

    

    while(1)
    {
        read_sensor_lux(&lux);

        printf("Sensor LUX: %f\n", lux);
        sleep(1);
        //break;
    }
        
    
    pthread_exit(NULL);
}


