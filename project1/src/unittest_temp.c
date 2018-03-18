/**
 * @file unittest_temp.c
 * @brief This file contains unit test functions for temp sensor.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date March 17, 2018
 *
 */

//***********************************************************************************
// Include files
//**********************************************************************************
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <pthread.h>
#include "main_task.h"
#include "logger_task.h"
#include "temp_task.h"

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************************
 * @brief test_temp_regCheck
 * unit test function to test all the temperature sensor register functions
 *****************************************************************************/
void test_temp_regCheck(void **state)
{
	uint8_t  data;
	int8_t ret;
	
	/******* Test config register *********/
	data=0xFF;
	ret=write_config_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_config_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test tlow register *********/
	ret=read_tlow_celsius(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	//assert_int_equal(data, 0xFF);
	
	/******* Test thigh register *********/
	ret=read_thigh_celsius(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	//assert_int_equal(data, 0xFF);
}

