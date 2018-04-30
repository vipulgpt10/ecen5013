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
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include "main_task.h"
#include "logger_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* logger task message queue descriptor */
mqd_t logTask_mq_d;
/* logger file pointer */
FILE* log_fp;
/* logger shared memory*/
void *logTask_sh_mem;
int logTask_sm_fd;
/* logger task kill flag*/
int logTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
/* loglevel string names array*/
const char* TIVA_messageType_nameString[5]= {"ERROR","STATUS","INFO","DATA","LOG_ALL"};
/* array of task id strings*/
const char* TIVAMessage_nameString[8]= { "LOGGER_TASK_INITIALIZED", "HEARTBEAT_TASK_INITIALIZED","PEDOMETER_TASK_INITIALIZED", "PULSERATE_TASK_INITIALIZED", "LOGGER_QUEUE_CREATED", "AVERAGE_BEATS_PER_MINUTE", "NUMBER_OF_STEPS", "MESSAGE_MAX" };
const char* TIVA_taskId_nameString[5]= { "LOGGER_TASK_ID","HEARTBEAT_TASK_ID", \
 "PEDOMETER_TASK_ID","PULSERATE_TASK_ID", "MAIN_TASK_ID"};
/***** Mutex Locks ****/
pthread_mutex_t logQueue_mutex= PTHREAD_MUTEX_INITIALIZER;
extern int fd;
//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief logger_task_init()
 * This function creates below,
 * Message Queue: To log messages from all the running tasks.
 * Shared Memory: To share logger task's status(DEAD/ALIVE) with main_task.
 * LogFile      : To store messages received from its message queue.
 *****************************************************************************/
int logger_task_init(void) 
{
  /********* Create Log File *************/
  log_fp= fopen("my_log.txt", "w");
  fclose(log_fp);

  return logTask_mq_d;
}


/******************************************************************//**********
 * @brief logger_task_thread()
 * This thread initializes the logger task resources and process data in 
 * logger queue.
 *****************************************************************************/
void logger_task_thread(void) 
{
    logTask_Msg_t logData={0};
    int ret;

    LOG_STD("[INFO] LOGGER TASK STARTED\n");
    ret= logger_task_init();
    if(ERROR == ret)
    {
        LOG_STD("[ERROR] LOGGER TASK INIT: %s\n", strerror(errno));
        exit(ERROR);
    }

    LOG_STD("[INFO] [LOGGER] WAITING AT BARRIER\n");
    /* wait logger task so that other tasks are synchronized with it*/
    pthread_barrier_wait(&tasks_barrier);
    LOG_STD("[INFO] [LOGGER] CROSSED BARRIER\n");
 
    printf("size: %d\n", sizeof(logData));

    #if USE_UART
    uart_init();
    #endif

    #if USE_TCP
    struct sockaddr_in addr, peer_addr;
    int addr_len = sizeof(peer_addr);
    char rdbuff[1024] = {0};
    int server_socket, accepted_soc, opt = 1;
    int i = 0;
    int read_b;
    size_t pload_len = 0;

    /* create socket */
    if((server_socket = socket(AF_INET,SOCK_STREAM,0)) == 0)
    {
        printf("[Server] [ERROR] Socket Creation Error\n");
        return 1;
    }
    else
        printf("[Server] Socket Created Successfully\n");

    /* set socket options */
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(opt), sizeof(opt)))
    {
        printf("[Server] [ERROR] Socket options set error\n");
        return 1;
    }

    /*Set the sockaddr_in structure */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  
    addr.sin_port = htons(PORT_ADR);

    /*bind socket to a address */
    if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
    {
        printf("[Server] [ERROR] Bind socket Error\n");
        return 1;
    }
    else
        printf("[Server] Socket binded Successfully\n");

    /* listen for connections*/
    if(listen(server_socket,5) < 0)
    {
        printf("[Server] [ERROR] Can't listen connection\n");
        return 1;
    } 

    printf("[Server] Listening\n");

    /*accept connection */
    accepted_soc = accept(server_socket, (struct sockaddr*)&peer_addr,(socklen_t*)&addr_len);
    if(accepted_soc < 0)
    {
        printf("[Server] [ERROR] Can't accept connection\n");
        return 1;
    }

    printf("[Server] Accepted!\n");

    #endif   /* __USE_TCP__
 
    /* start logger queue processing */
  	while(1)
    {
		
        #if USE_UART
        read_packet(fd, &logData, sizeof(logData) );
        LOG_STD("Here!\n");
        #endif

        #if USE_TCP
        read(accepted_soc, &logData, sizeof(logData));
        LOG_STD("Here!\n");
        #endif

		LOG_TO_FILE(logData);
    }

    /*********** KILL Signal Received ***********/
    LOG_STD("[INFO] LOGGER TASK KILL SIGNAL RECEIVED\n");
    /* flush logfile */
    fflush(log_fp);
    /* close message queue*/
    ret=mq_close(LOGTASK_MQ_NAME);
    if(ERROR == ret)
    {
        LOG_STD("[ERROR] LOGGER MESSAGE QUEUE CLOSE FAILED:%s\n", strerror(errno));
    }
    LOG_STD("[INFO] LOGGER MESSAGE QUEUE CLOSED\n");

    ret=mq_unlink(LOGTASK_MQ_NAME);
    if(ERROR == ret)
    {
        LOG_STD("[ERROR] LOGGER MESSAGE QUEUE UNLINK FAILED:%s\n", strerror(errno));
    }
    LOG_STD("[INFO] LOGGER MESSAGE QUEUE UNLINKED\n");

    pthread_exit(NULL);
}
