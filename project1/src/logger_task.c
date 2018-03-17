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
static FILE* log_fp;
/* logger shared memory*/
void *logTask_sh_mem;
int logTask_sm_fd;
/* logger task kill flag*/
extern int logTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
/* loglevel string names array*/
const char* logLevel_nameString[LOG_ALL+1]= {"LOG_ERR","LOG_STATUS","LOG_INFO","LOG_ALL"};

/***** Mutex Locks ****/
pthread_mutex_t logQueue_mutex= PTHREAD_MUTEX_INITIALIZER;

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
  struct mq_attr logTaskQ_attr;
  logTask_Msg_t logData;
  int ret;
  Task_Status_t logger_status;

  /********* Create Logger Message Queue *************/
  /* initialize message queue attribute structure */
  logTaskQ_attr.mq_msgsize= sizeof(logTask_Msg_t);

  logTaskQ_attr.mq_maxmsg= 128;
  logTaskQ_attr.mq_flags= 0;
  logTaskQ_attr.mq_curmsgs= 0;

  logTask_mq_d =mq_open(LOGTASK_MQ_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &logTaskQ_attr);
  /* check for error in creation*/
  if(logTask_mq_d == ERROR)
  {
    LOG_STD("[ERROR] MESSAGE QUEUE NOT CREATED:%s\n", strerror(errno));
    return ERROR;
  }
  /* as logger queue initialized in above step, log into message queue 
     instead of std output*/
  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"MESSAGE QUEUE CREATED");

  /********* Create Log File *************/
  log_fp= fopen("my_log.txt", "w");
  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"LOG FILE CREATED");
  fclose(log_fp);


  /************Create Shared Memory to share task status with main ******/	
  logTask_sm_fd = shm_open(LOGTASK_SM_NAME, O_CREAT | O_RDWR, 0666);
  if(logTask_sm_fd == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LOGGER_TASK_ID,"SHARED MEMORY NOT CREATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"SHARED MEMORY CREATED");

  /* truncate shared memory with required size */
  ret = ftruncate(logTask_sm_fd, SM_SIZE);
  if(ret == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LOGGER_TASK_ID,"SHARED MEMORY NOT TRUNCATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"SHARED MEMORY TRUNCATED");

  /*map the shared memory */
  logTask_sh_mem = mmap(NULL, SM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , logTask_sm_fd, 0);
  if(logTask_sh_mem == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LOGGER_TASK_ID,"SHARED MEMORY NOT MAPPED");
    return -1;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"SHARED MEMORY MAPPED");

  /**** Enter the task status into shared memory *****/
  logger_status=ALIVE;
  /* Copy the contents of payload into the shared memory */
  memcpy((char*)logTask_sh_mem, (char*)&logger_status, SM_SIZE);

  LOG_STD("[INFO] LOGGER TASK INITIALIZED QUEUE, SHARED MEMORY & LOGFILE\n");

  return logTask_mq_d;
}

/******************************************************************//**********
 * @brief logger_task_process_MQ()
 * This function processes logger queue i.e removes data from the logger queue 
 * and enters in logger file.
 *****************************************************************************/
void logger_task_process_MQ(void)
{
  logTask_Msg_t  mq_data={0};
  int ret, msg_prio;
  logTask_Msg_t logData;

  LOG_TO_QUEUE(logData,LOG_INFO,LOGGER_TASK_ID,"MESSAGE QUEUE PROCESS STARTED");
  LOG_STD("[INFO] MESSAGE QUEUE PROCESS STARTED\n");
  
  /* process the data until kill signal to logger task is received */
  while(!logTask_kill)
  {
    ret=mq_receive(logTask_mq_d, (char*)&(mq_data), sizeof(mq_data), &msg_prio );
    /* ret: Received number of bytes if SUCCESS
            -1 if ERROR */
    if(ERROR !=ret)
    {
      /* log into file */
      LOG_TO_FILE(mq_data);
    }
  } 

}

/******************************************************************//**********
 * @brief logger_task_thread()
 * This thread initializes the logger task resources and process data in 
 * logger queue.
 *****************************************************************************/
void logger_task_thread(void) 
{
  logTask_Msg_t logData;
  Task_Status_t logger_status;
  int ret;

  LOG_STD("[INFO] LOGGER TASK STARTED\n");
  ret= logger_task_init();
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LOGGER TASK INIT: %s\n", strerror(errno));
    exit(ERROR);
  }

  /* wait logger task so that other tasks are synchronized with it*/
  pthread_barrier_wait(&tasks_barrier);
  /* start logger queue processing */
  logger_task_process_MQ();

  /*********** KILL Signal Received ***********/
  LOG_STD("[INFO] LOGGER TASK KILL SIGNAL RECEIVED\n");
  /* flush logfile */
  fflush(log_fp);
  /* Update task status in shared memory */
  logger_status=DEAD;
  /* Copy the contents of payload into the share memory */
  memcpy((char*)logTask_sh_mem, (char*)&logger_status, SM_SIZE);
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
