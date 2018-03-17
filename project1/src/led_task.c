/**
 * @file led_task.c
 * @brief The implementation file for USR LED switching functions
 *
 * This implementation file provides the function definitions for
 * switching USR LED0 on Beaglebone Green.
 *
 * @author Vipul Gupta
 * @date 17 March 2018
 *
 */

#include "led_task.h"

void LED_ON(void)
{
	FILE * fptr = fopen(LED0_PATH, "w+");
	
	if(fptr == NULL)
		return;
	else
	{
		fputc('1', fptr);
		fclose(fptr);
		return;	
	}
}

void LED_OFF(void)
{
	FILE * fptr = fopen(LED0_PATH, "w+");
	
	if(fptr == NULL)
		return;
	else
	{
		fputc('0', fptr);
		fclose(fptr);
		return;	
	}
}

void LED_INIT(void)
{
	FILE * fptr = fopen(LED0_INITPATH, "w+");
	
	if(fptr == NULL)
		return;
	else
	{
		fprintf(fptr, "none");
		fclose(fptr);
		return;	
	}	
}


