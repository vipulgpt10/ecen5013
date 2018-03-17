/* Temperature task */


#include "temp_task.h"

int8_t read_temp_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_TEMP_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

int8_t read_temp_fahrenheit(float * data)
{
	float fah_temp;
	int8_t ret;

	ret = read_temp_celsius(&fah_temp);
	*data = (fah_temp * 1.8) + 32;
	return ret;
}

int8_t read_temp_kelvin(float * data)
{
	float kel_temp;
	int8_t ret;

	ret = read_temp_celsius(&kel_temp);
	*data = kel_temp + 273.15;
	return ret;
}

int8_t write_config_reg(const uint16_t data)
{
	uint16_t tmp1, tmp2, msb_val;
	tmp1 = data & 0xFF00;
	tmp2 = data & 0x00FF;
	msb_val = (tmp1 >> 8 | tmp2 << 8);

	return i2c_write_word(TMP102_DEV_ADDR, TMP102_CONF_REG, msb_val);
}

int8_t read_config_reg(uint16_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(TMP102_DEV_ADDR, TMP102_CONF_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    val = ((uint16_t)arr[0] << 8 | arr[1]);
    *data = val;
    return ret;
}

int8_t read_tlow_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_TLOW_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

int8_t read_thigh_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_THIGH_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

void * temp_thread(void * data)
{
	float temp;

	while(1)
	{
		read_temp_celsius(&temp);
		printf("Temperature = %f celsius\n", temp);

		read_temp_fahrenheit(&temp);
		printf("Temperature = %f F\n", temp);

		read_temp_kelvin(&temp);
		printf("Temperature = %f Kelvin\n", temp);

		sleep(2);

	}

	pthread_exit(NULL);
}