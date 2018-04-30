/**
 * @file pedometer.h
 *
 * This header file provides the prototypes of the function for
 * Pedometer..
 *
 * @author Nagarjuna Reddy Kovuru
 * @date 26 April 2018
 *
 */
#ifndef __MAIN_H__
#define __MAIN_H__
//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"      // Pin mux
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"



//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
//#define UART_COMMUNICATION
#define TCP_COMMUNICATION

#define NUM_TASKS     4

#define DEFAULT_STEP_COUNT (0)
#define DEFAULT_PULSERATE (0)

//Stack depth for each task
#define mainLOGGER_STACK_DEPTH       128
#define mainHEARTBEAT_STACK_DEPTH    128
#define mainPEDOMETER_STACK_DEPTH    128
#define mainPULSERATE_STACK_DEPTH    128

//Priority for each task
#define mainLOGGER_TASK_PRIORITY       1
#define mainHEARTBEAT_TASK_PRIORITY    1
#define mainPEDOMETER_TASK_PRIORITY    1
#define mainPULSERATE_TASK_PRIORITY    1


#define mainHEARTBEAT_TIMER_PERIOD      pdMS_TO_TICKS(1000) //1Hz=1000ms
#define mainLOGGERTASK_TIMER_PERIOD     pdMS_TO_TICKS(500)

//System Clock
#define mainSYSTEM_CLOCK         120000000
#define mainI2C_CLOCK            40000000

/* task ID type */
typedef enum
{
  LOGGER_TASK_ID=0,
  HEARTBEAT_TASK_ID,
  PEDOMETER_TASK_ID,
  PULSERATE_TASK_ID,
  MAIN_TASK_ID
}Task_Id_t;

//#define LOGGER_TASK_ID   0
//#define HEARTBEAT_TASK_ID   1
//#define PEDOMETER_TASK_ID   2
//#define PULSERATE_TASK_ID   3
//#define MAIN_TASK_ID   4

/* task status type*/
typedef enum
{
    MISSED_HEARTBEAT=0,
    RECEIVED_HEARTBEAT
}Task_Status_t;

//#define MISSED_HEARTBEAT  0
//#define MISSED_HEARTBEAT  1
//***********************************************************************************
// Global Variables
//***********************************************************************************
extern xSemaphoreHandle uart_lock;
extern xSemaphoreHandle i2c_lock;
extern xSemaphoreHandle logQueue_mutex;

//***********************************************************************************
// Function Prototypes
//***********************************************************************************


#endif /* __PEDOMETER_H__ */

