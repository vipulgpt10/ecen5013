/**
* @file main_task.h
* @brief This file contains main task functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Kovuru Nagarjuna Reddy
* @date March 14 2018
*
*/
#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//***********************************************************************************
// Defined Macros and Types
//***********************************************************************************
/* Macro to denote the number of threads */
#define NUM_THREADS     5

/* status macros */
#define SUCCESS        0
#define ERROR         -1

/* task ID type */
typedef enum
{
  LOGGER_TASK_ID,
  TEMP_TASK_ID,
  LIGHT_TASK_ID,
  SOCKET_TASK_ID,  
  //GPIO_TASK_ID,
  MAIN_TASK_ID
}Task_Id_t;

/* task status type*/
typedef enum
{
  DEAD=0,
  ALIVE
}Task_Status_t;

//***********************************************************************************
// Global Variables
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//**********
 * @brief main()
 * This function creates creates multiple threads such as logger_task, 
 * temperature_task, light_task, socket_task & status read and 
 * waits for its completion
 *****************************************************************************/
int main( int argc, char** argv );


#endif /* __MAIN_TASK_H__ */
