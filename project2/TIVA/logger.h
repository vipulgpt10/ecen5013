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
#ifndef __LOGGER_H__
#define __LOGGER_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* message size */
#define LT_MSG_SIZE       32
/* logger message queue name */
#define LOGTASK_MQ_NAME   "/logTask_mq"
/* logger file name */
#define LOGFILE_NAME      "/my_log.txt"

#define LOG_STRING        0x02
#define LOGGER_TASK_HEARTBEAT      0x1
/* Log level  type*/
typedef enum
{
  LOG_ERR,
  LOG_STATUS,
  LOG_INFO,
  LOG_DATA,
  LOG_ALL
}Log_MessageType_t;

//#define LOG_ERR   0
//#define LOG_STATUS   1
//#define LOG_INFO   2
//#define LOG_DATA   3
//#define LOG_ALL   4

/* Log level  type*/
typedef enum
{
  LOGGER_TASK_INITIALIZED,
  HEARTBEAT_TASK_INITIALIZED,
  PEDOMETER_TASK_INITIALIZED,
  PULSERATE_TASK_INITIALIZED,
  LOGGER_QUEUE_INITIALIZED,
  AVERAGE_BEATS_PER_MINUTE,
  NUMBER_OF_STEPS,
  MESSAGE_MAX
}Log_Message_t;

//#define LOGGER_TASK_INITIALIZED   0
//#define HEARTBEAT_TASK_INITIALIZED   1
//#define PEDOMETER_TASK_INITIALIZED   2
//#define PULSERATE_TASK_INITIALIZED   3
//#define LOGGER_QUEUE_INITIALIZED   4
//#define AVERAGE_BEATS_PER_MINUTE    (5)
//#define NUMBER_OF_STEPS   6
//#define MESSAGE_MAX   7

/* logger message structure*/
typedef struct
{
    Log_MessageType_t    message_type;
    Task_Id_t            task_ID;
    uint8_t              pedometer_data;
    uint8_t              pulserate_data;
    Log_Message_t        log_message;
    uint8_t        task_status[NUM_TASKS];
}logTask_Msg_t;


//Message queue length
#define mainQUEUE_LENGTH            100
//Size of each element in message queue
#define mainQUEUE_ELEMENT_SIZE      sizeof(logTask_Msg_t)
/******************************************************************//**********
 * @brief prints messages to standard output(screen).
 *****************************************************************************/
#define LOG_STD(format, ...)    \
    xSemaphoreTake(uart_lock, portMAX_DELAY);   \
    UARTprintf(format, ##__VA_ARGS__);   \
    xSemaphoreGive(uart_lock)

/******************************************************************//**********
 * @brief enters data into logger message queue.
 *****************************************************************************/
#define LOG_TO_QUEUE(name,messageType,taskId, stepcount, pulserate, message) \
    xSemaphoreTake( logQueue_mutex, portMAX_DELAY);\
    name.message_type= messageType; \
    name.task_ID= taskId; \
    name.pedometer_data = stepcount; \
    name.pulserate_data = pulserate; \
    name.log_message= message; \
    write_message_LogQueue( g_xLoggerQueueHandle,&name,sizeof(name)); \
    xSemaphoreGive( logQueue_mutex )


//***********************************************************************************
// Global Variables 
//***********************************************************************************
extern const char* taskId_nameString[MAIN_TASK_ID+1];

//***********************************************************************************
// Function Prototypes
//***********************************************************************************

void logger_init(void);

void write_message_LogQueue(QueueHandle_t queue, const logTask_Msg_t *logstruct, \
                                                                 size_t log_struct_size);

#endif /* __LOGGER_H__ */
