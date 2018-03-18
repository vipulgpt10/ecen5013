/**
* @file logger_task.h
* @brief This file contains logger task functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Kovuru Nagarjuna Reddy
* @date March 14 2018
*
*/
#ifndef __SOCKET_TASK_H__
#define __SOCKET_TASK_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
#define PORT_ADR    2000
/* shared memory name and size */
#define SOCKETTASK_SM_NAME	   "socketTask_sharedMemory"

#define NIGHT_DAY_TH	(15.0)
//***********************************************************************************
// Global Variables 
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************

/******************************************************************//**********
 * @brief set_Log_currentTimestamp()
 * This function gets current timestamp and converts it into string.
 *****************************************************************************/
/******************************************************************//**********
 * @brief socket_task_thread()
 * This thread initializes socket connection
 *****************************************************************************/
void socket_task_thread(void);

#endif
