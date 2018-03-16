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
#include "my_time.h"

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* message size */
#define LT_MSG_SIZE       32
/* logger message queue name */
#define LOGTASK_MQ_NAME   "/logTask_mq"
/* logger file name */
#define LOGFILE_NAME      "/my_log.txt"
/* shared memory name and size */
#define LOGTASK_SM_NAME	   "logTask_sharedMemory"
#define SM_SIZE     sizeof(Task_Status_t)

/* Log level  type*/
typedef enum
{
  LOG_ERR,
  LOG_STATUS,
  LOG_INFO,
  LOG_ALL
}Log_Level_t;

/* logger message structure*/
typedef struct
{
   char             time_stamp[20];
   Log_Level_t      log_level;
   Task_Id_t        source_ID;
   char             lmq_data[LT_MSG_SIZE];
}logTask_Msg_t;


/******************************************************************//**********
 * @brief prints messages to standard output(screen).
 *****************************************************************************/
#define   LOG_STD(format, ...)  \
          printf("[PID:%d] [TID:%ld] [FUNC:%s]",getpid(), syscall(SYS_gettid), __func__); \
          printf(format, ##__VA_ARGS__)

/******************************************************************//**********
 * @brief enters message queue data into log file.
 *****************************************************************************/
#define   LOG_TO_FILE(mq_data)  \
          char buf[200]; \
          log_fp=fopen("my_log.txt", "a"); \
          snprintf( buf, sizeof(buf), "[PID:%d] [TID:%ld] time_stamp:%s log_level:%s" \
                        " source_ID:%s lmq_data:%s\n", getpid(), syscall(SYS_gettid), \
                    mq_data.time_stamp, logLevel_nameString[mq_data.log_level], \
                    taskId_nameString[mq_data.source_ID], mq_data.lmq_data ); \
          fprintf(log_fp, buf); \
          fclose(log_fp);          
  
/******************************************************************//**********
 * @brief enters data into logger message queue.
 *****************************************************************************/
#define LOG_TO_QUEUE(name,logLevel,sourceId,format, ...) \
    name.log_level= logLevel; \
    name.source_ID= sourceId; \
    set_Log_currentTimestamp(&name);  \
    snprintf(name.lmq_data,sizeof(name.lmq_data),format, ##__VA_ARGS__); \
    write_message_LogQueue(logTask_mq_d,&name,sizeof(name)); 

//***********************************************************************************
// Global Variables 
//***********************************************************************************
extern const char* taskId_nameString[MAIN_TASK_ID+1];

//***********************************************************************************
// Function Prototypes
//***********************************************************************************

/******************************************************************//**********
 * @brief set_Log_currentTimestamp()
 * This function gets current timestamp and converts it into string.
 *****************************************************************************/
static inline void set_Log_currentTimestamp(logTask_Msg_t *log_msg)   
{
    get_time_string(log_msg->time_stamp,sizeof(log_msg->time_stamp));
}

#endif /* __LOGGER_H__ */
