//*****************************************************************************
//
// APES HW5 Problem 2
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <limits.h>
#include "main.h"
#include "pulserate.h"
#include "pedometer.h"
#include "logger.h"
#include "tcp_wrapper.h"
#include "uart_wrapper.h"

//***********************************************************************************
//Global variables/structures
//***********************************************************************************
/* Global variable to store the set system clock frequency */
uint32_t g_ui32SysClock;
uint8_t rates[4]; //Array of heart rates
uint8_t rateSpot = 0;
uint32_t lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

xSemaphoreHandle uart_lock;
xSemaphoreHandle i2c_lock;
xSemaphoreHandle logQueue_mutex;

/* array of task status strings */
const char* task_statusString[]={"DEAD", "ALIVE"};
/* array of function name strings*/
const char *task_name[]= { "logger_task", "heartBeat_task", "pedometer_task", "pulserate_task"};
/* */
uint8_t stack_depth[NUM_TASKS]={ mainLOGGER_STACK_DEPTH, mainHEARTBEAT_STACK_DEPTH, \
                                 mainPEDOMETER_STACK_DEPTH, mainPULSERATE_STACK_DEPTH };
/* */
uint8_t task_priority[NUM_TASKS]={ mainLOGGER_TASK_PRIORITY, mainHEARTBEAT_TASK_PRIORITY , \
                                   mainPEDOMETER_TASK_PRIORITY, mainPULSERATE_TASK_PRIORITY};
/* */
TaskHandle_t task_handle[NUM_TASKS]={ NULL, NULL, NULL, NULL };
/* array of task id strings*/
const char* taskId_nameString[MAIN_TASK_ID+1]= { "LOGGER_TASK_ID", "HEARTBEAT_TASK_ID", \
                                                 "PEDOMETER_TASK_ID", "PULSERATE_TASK_ID", "MAIN_TASK_ID" };
extern const char* logmessageType_nameString[LOG_ALL+1];
extern const char* logmessage_nameString[MESSAGE_MAX];

/**** Function prototypes ****/
void logger_task(void * pvParameters);
void heart_beat_task(void * pvParameters);
void pedometer_task(void * pvParameters);
void pulserate_task(void * pvParameters);

/* array of function pointers*/
static void (*task[NUM_TASKS]) = { logger_task, heart_beat_task, pedometer_task, pulserate_task };

extern QueueHandle_t g_xLoggerQueueHandle;
/******************************************************************//****
 * The error routine that is called if the driver library encounters
 * an error
 *
 ***********************************************************************/
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

/******************************************************************//****
 * @brief vApplicationStackOverflowHook()
 * This hook is called by FreeRTOS when an stack overflow error is
 * detected
 *
 ***********************************************************************/
void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

/******************************************************************//****
 * @brief Task2Timer_Callback()
 * This is a callback function for timer of Task2 which is set to 4Hz(250ms).
 * This function sends LOG_STRING notification to Task3.
 *
 ***********************************************************************/
#if 1
static void HeartBeatTimer_Callback(TimerHandle_t xTimer)
{
    logTask_Msg_t   logHeartBeat_Data={0};
    uint32_t    u32Events;
    BaseType_t NotifyWait_Return;

    //Wait on notifications TOGGLE_LED and LOG_STRING
    NotifyWait_Return = xTaskNotifyWait(0, ULONG_MAX, &u32Events, pdMS_TO_TICKS(1000)); //timeout 10,000ms=10s
    //Check for TOGGLE_LED event
    if( !(u32Events&LOGGER_TASK_HEARTBEAT) )
    {
      LOG_STD("[LOGGER] HEARTBEAT MISSED");
      logHeartBeat_Data.task_status[0] = MISSED_HEARTBEAT;
    }
    //else
    {
        logHeartBeat_Data.task_status[0] = RECEIVED_HEARTBEAT;
    }

    LOG_TO_QUEUE(logHeartBeat_Data, LOG_STATUS, HEARTBEAT_TASK_ID, DEFAULT_STEP_COUNT, DEFAULT_PULSERATE, LOGGER_TASK_INITIALIZED);

}
#endif
void heart_beat_task(void * pvParameters)
{
    TimerHandle_t xHeartBeatTimerHandle;
#if 0
    //create a timer which runs continuously with frequency 2Hz
    xHeartBeatTimerHandle= xTimerCreate("heartbeat_timer", \
                                    mainHEARTBEAT_TIMER_PERIOD, \
                                    pdTRUE, \
                                    0, \
                                    HeartBeatTimer_Callback);
    if(xHeartBeatTimerHandle != NULL)
    {
        LOG_STD("[INFO] [HEARTBEAT] Timer created successfully\n");
    }
    else
    {
        LOG_STD("[ERROR] [HEARTBEAT]  Timer creation error\n");
    }

    //Start the timer without any timeout
    xTimerStart(xHeartBeatTimerHandle,0);
#endif
    while(1)
    {
    }
}

void logger_task(void * pvParameters)
{
    logTask_Msg_t logMesg_recv={0}, logMesg_uart_recv={0};
    uint8_t index;


    LOG_STD("[INFO] LOGGER TASK INITIALIZED\n");
    LOG_TO_QUEUE(logMesg_recv, LOG_INFO, LOGGER_TASK_ID, DEFAULT_STEP_COUNT, DEFAULT_PULSERATE, LOGGER_TASK_INITIALIZED);

    while(1)
    {
        //Receive message from queue
        if( pdTRUE == xQueueReceive(g_xLoggerQueueHandle, &logMesg_recv, portMAX_DELAY) )
        {

            #ifdef TCP_COMMUNICATION
                TCP_Send((void *)&logMesg_recv, sizeof(logMesg_recv));
            #endif

            #ifdef  UART_COMMUNICATION
                uart_send_packet(&logMesg_recv, sizeof(logMesg_recv));
            #endif

            LOG_STD("\n[%s] [%s] [STEP_COUNT: %d] [PULSERATE: %d] [MESSAGE: %s] \n", \
                        logmessageType_nameString[logMesg_recv.message_type], \
                        taskId_nameString[logMesg_recv.task_ID], \
                        logMesg_recv.pedometer_data, logMesg_recv.pulserate_data, \
                        logmessage_nameString[logMesg_recv.log_message] );

        }
  }
}

void pulserate_task(void * pvParameters)
{
    uint32_t irValue = 0;
    uint8_t x;
    volatile uint32_t delta = 0;
    logTask_Msg_t logData={0};

    /* Init */
    #ifdef TCP_COMMUNICATION
    Configure_TCP();
    #endif
    vTaskDelay(500);
    init_pulserate();
    vTaskDelay(500);

    if(test_pulserate())
    {
        LOG_STD("[INFO] [PULSERATE] Startup Test: Passed!\n\r");
    }
    else
    {
        LOG_STD("[DATA] [PULSERATE] Startup Test: Failed!===\n\r");
    }


    while(1)
    {
        irValue = getIR();

        if (checkForBeat(irValue))
        {
            //We sensed a beat!
            delta = xTaskGetTickCount() - lastBeat;

            lastBeat = xTaskGetTickCount();

            beatsPerMinute = 60 / (delta / 1000.0);

            if (beatsPerMinute < 255 && beatsPerMinute > 40)
            {
                rates[rateSpot++] = (uint8_t)beatsPerMinute;
                rateSpot %= 4; //Wrap variable

                //Take average of readings
                beatAvg = 0;
                for (x = 0 ; x < 4 ; x++)
                    beatAvg += rates[x];

                beatAvg /= 4;
            }

            //LOG_STD("[DATA] [PULSERATE] Avg BPM = %d\n\r", beatAvg);
            LOG_STD("Heart rate\n");
            LOG_TO_QUEUE(logData, LOG_DATA, PULSERATE_TASK_ID, DEFAULT_STEP_COUNT, beatAvg, AVERAGE_BEATS_PER_MINUTE);

        }
    }
    /* Task should be deleted and should not return */
    vTaskDelete(NULL);
}

void pedometer_task(void * pvParameters)
{
    uint16_t count = 0;
    int32_t temp = 0;
    logTask_Msg_t logData={0};

    init_pedometer();
    vTaskDelay(500);

    if(test_pedometer())
    {
        LOG_STD("[INFO] [PEDOMETER] Startup Test: Passed!\n\r");
    }
    else
    {
        LOG_STD("[ERROR] [PEDOMETER] Startup Test: Failed!===\n\r");
    }


    while(1)
    {

        count = read_stepCounter();
//        LOG_STD("[DATA] [PEDOMETER] Step Count = %d\t", count);
        LOG_TO_QUEUE(logData, LOG_DATA, PEDOMETER_TASK_ID, count, DEFAULT_STEP_COUNT, NUMBER_OF_STEPS);
        LOG_STD("Pedometer\n");

        temp = read_temperature();
        LOG_STD(" Temperature = %d\n\r", temp);

        vTaskDelay(1000);
    }

    vTaskDelete(NULL);
}

/******************************************************************//****
 * @brief main()
 * This function enables system clock, peripherals and creates tasks.
 *
 ***********************************************************************/
int main(void)
{
    uint8_t index;
    logTask_Msg_t logData={0};

    // Initialize system clock to 120 MHz
    g_ui32SysClock= SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               mainSYSTEM_CLOCK);
    ASSERT(g_ui32SysClock == SYSTEM_CLOCK);

    uart_lock = xSemaphoreCreateMutex();
    i2c_lock = xSemaphoreCreateMutex();
    logQueue_mutex = xSemaphoreCreateMutex();

    ConfigureUART();
    logger_init();

    i2c_init();
    SysCtlDelay(g_ui32SysClock/3);


   /**** Thread Creation *******/
   LOG_STD("[INFO] [MAIN] CREATING THREADS \n");
   /* run loop required thread number of times */
   for (index = 0; index< NUM_TASKS; index++)
   {
     if( pdTRUE == xTaskCreate( task[index], task_name[index], stack_depth[index], NULL, task_priority[index], &(task_handle[index]) ) )
     {
       LOG_STD("[INFO] [MAIN] TASK %s CREATED \n", task_name[index]);
     }
     else
     {
       LOG_STD("[ERROR] [MAIN] TASK %s NOT CREATED\n", task_name[index]);
     }
   }

   //Start scheduler
   vTaskStartScheduler();
}




