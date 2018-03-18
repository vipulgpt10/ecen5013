/**
 * @file unittest_light.c
 * @brief This file contains unit test functions for light sensor.
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
#include "light_task.h"

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************************
 * @brief test_light_regCheck
 * unit test function to test all the light sensor register functions
 *****************************************************************************/
void test_light_regCheck(void **state)
{
	uint8_t  data;
	int8_t ret;
	
	/******* Test Control register *********/
	data=0xFF;
	ret=write_control_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_control_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test Timing register *********/
	data=0xFF;
	ret=write_timing_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_timing_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test tlow register *********/
	data=0xFF;
	ret=write_intthresh_low_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_intthresh_low_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test thigh register *********/
	data=0xFF;
	ret=write_intthresh_high_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_intthresh_high_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test intcontrol register *********/
	data=0xFF;
	ret=write_intcontrol_reg(data);
	assert_int_equal(ret, EXIT_SUCCESS);
	
	ret=read_intcontrol_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0xFF);
	
	/******* Test id register *********/
	ret=read_id_reg(&data);
	assert_int_equal(ret, EXIT_SUCCESS);
	assert_int_equal(data, 0x50);
}

