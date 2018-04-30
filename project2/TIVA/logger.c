/**
 * @file logger_task.c
 * @brief This file contains logger task functionality.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <logger.h>
#include "main.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* loglevel string names array*/
const char* logmessageType_nameString[LOG_ALL+1]= {"ERROR","STATUS","INFO","DATA","ALL"};
const char* logmessage_nameString[MESSAGE_MAX]= {"LOGGER_TASK_INITIALIZED", "HEARTBEAT_TASK_INITIALIZED", \
                                                 "PEDOMETER_TASK_INITIALIZED", "PULSERATE_TASK_INITIALIZED", \
                                                 "LOGGER_QUEUE_INITIALIZED", "AVERAGE_BEATS_PER_MINUTE", \
                                                 "NUMBER_OF_STEPS"};

/* Queue handle*/
QueueHandle_t g_xLoggerQueueHandle;

extern TaskHandle_t task_handle[NUM_TASKS];
//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief write_message_LogQueue()
 * This function writes message to logger queue. 
 * @queue     : Message queue to write into 
 * @logstruct : Logger message
 * @log_struct_size : Message size 
 *****************************************************************************/
void write_message_LogQueue( QueueHandle_t queue, const logTask_Msg_t *logstruct, \
                                                                 size_t log_struct_size)
{

  if(pdTRUE==xQueueSend(queue, (void*)logstruct, portMAX_DELAY))
  {
      LOG_STD("[LOG_STRING] Message queued successfully\n");
  }
  else
  {
      LOG_STD("[LOG_STRING] Message queue Error\n");
  }

}

static void loggerTimer_Callback(TimerHandle_t xTimer)
{
    logTask_Msg_t   logHeartBeat_Data;
    uint32_t    u32Events;

    //Notify Task3 with LOG_STRING event & check the return value
    if( pdTRUE==xTaskNotify(task_handle[1], LOGGER_TASK_HEARTBEAT, eSetBits))
    {
        UARTprintf("[LOGGER] Task Notify Success\n");
    }
    else
    {
        UARTprintf("[LOGGER] Task Notify Failed\n");
    }

}

/******************************************************************//**********
 * @brief logger_task_init()
 * This function creates below,
 * Message Queue: To log messages from all the running tasks.
 * Shared Memory: To share logger task's status(DEAD/ALIVE) with main_task.
 * LogFile      : To store messages received from its message queue.
 *****************************************************************************/
void logger_init(void)
{
    TimerHandle_t xLoggerTimerHandle;

    //Create a queue
  g_xLoggerQueueHandle = xQueueCreate(mainQUEUE_LENGTH, mainQUEUE_ELEMENT_SIZE);
  if(g_xLoggerQueueHandle != NULL)
  {
      LOG_STD("[INFO] [LOGGER] Queue created successfully\n");
  }
  else
  {
      LOG_STD("[ERROR] [LOGGER] Queue creation error\n");
  }


#if 0
  //create a timer which runs continuously with frequency 2Hz
  xLoggerTimerHandle= xTimerCreate("Task1_timer", \
                                  mainLOGGERTASK_TIMER_PERIOD, \
                                  pdTRUE, \
                                  0, \
                                  loggerTimer_Callback);
  if(xLoggerTimerHandle != NULL)
  {
      UARTprintf("[LOGGER] Timer created successfully\n");
  }
  else
  {
      UARTprintf("[LOGGER] Timer creation error\n");
  }

  //Start the timer without any timeout
  xTimerStart(xLoggerTimerHandle,0);
#endif
}

/*void standard_display(logTask_Msg_t *logMessage)
{
    LOG_STD("[%s] [%s] [STEP_COUNT: %d] [PULSERATE: %d] [MESSAGE: %s] \n", \
            logmessageType_nameString[logMessage->message_type], \
            taskId_nameString[logMessage->task_ID], \
            logMessage->pedometer_data, logMessage->pulserate_data, \
            logmessageType_nameString[logMessage->pulserate_data] \
            logmessageType_nameString[logMessage->message] );
}*/
