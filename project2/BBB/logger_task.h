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
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/mman.h>
#include "main_task.h"

#if USE_TCP
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#endif

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* message size */
#define LT_MSG_SIZE       32
/* logger message queue name */
#define LOGTASK_MQ_NAME   "/logTask_mq"
/* logger file name */
#define LOGFILE_NAME      "/my_log.txt"

#define PORT_ADR    2000


/* Log level  type*/
typedef enum
{
  LOG_ERR,
  LOG_STATUS,
  LOG_INFO,
  LOG_DATA,
  LOG_ALL
}Log_MessageType_t;


/* Log level  type*/
typedef enum
{
  LOGGER_TASK_INITIALIZED,
  UART_TASK_INITIALIZED,
  PEDOMETER_TASK_INITIALIZED,
  PULSERATE_TASK_INITIALIZED,
  LOGGER_QUEUE_INITIALIZED,
  MESSAGE_MAX
}Log_Message_t;

 #define NUM_TASKS      3
/* logger message structure*/
typedef struct
{
   uint8_t    message_type;
   uint8_t    task_ID;
   uint8_t    pedometer_data;
   uint8_t    pulserate_data;
   uint8_t    log_message;
   uint8_t    task_status[4];
}logTask_Msg_t;



/******************************************************************//**********
 * @brief enters message queue data into log file.
 *****************************************************************************/

#define   LOG_TO_FILE(mq_data)  \
          char buf[200]; \
          log_fp=fopen("my_log.txt", "a"); \
          snprintf( buf, sizeof(buf), "[%s]" "[%s]" "[STEP COUNT: %d]" "[PULSERATE COUNT: %d]" "[%s]\n", \
             TIVA_messageType_nameString[mq_data.message_type], TIVA_taskId_nameString[mq_data.task_ID], \
             mq_data.pedometer_data, mq_data.pulserate_data, TIVAMessage_nameString[mq_data.log_message] ); \
          fprintf(log_fp, buf); \
          fclose(log_fp);          

/******************************************************************//**********
 * @brief prints messages to standard output(screen).
 *****************************************************************************/
#define   LOG_STD(format, ...)    printf(format, ##__VA_ARGS__)

//***********************************************************************************
// Global Variables 
//***********************************************************************************
extern const char* taskId_nameString[MAIN_TASK_ID];

//***********************************************************************************
// Function Prototypes
//***********************************************************************************

void write_message_LogQueue(mqd_t queue, const logTask_Msg_t *logstruct, \
                                                                 size_t log_struct_size);

#endif /* __LOGGER_H__ */
