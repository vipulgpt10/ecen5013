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

//***********************************************************************************
// Include files
//***********************************************************************************
#include "led_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************


//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief LED_ON()
 * This function switch ON the LED present on board
 *****************************************************************************/
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

/******************************************************************//**********
 * @brief LED_OFF()
 * This function switch OFF the LED present on board
 *****************************************************************************/
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

/******************************************************************//**********
 * @brief LED_INIT()
 * This function switch initializes the LED present on board
 *****************************************************************************/
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


