/**
 * @file main_task.c
 * @brief This file contains main task which creates multiple threads such as
 * logger_task, temperature_task, light_task, socket_task & status read.
 
 * @author Kovuru Nagarjuna Reddy
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>f
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include "main_task.h"
#include "logger_task.h"
#include "temp_task.h"
#include "light_task.h"
#include "socket_task.h"
#include "led_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* array of task status strings */
const char* task_statusString[]={"DEAD", "ALIVE"};
/* array of function name strings*/
const char *threadfun_name[]= { "logger_task_thread", "temp_task_thread","light_task_thread","socket_task_thread","status_read_thread"};
/* array of task id strings*/
const char* taskId_nameString[MAIN_TASK_ID+1]= { "LOGGER_TASK_ID", "TEMP_TASK_ID", \
                                "LIGHT_TASK_ID", "SOCKET_TASK_ID", "MAIN_TASK_ID" };
/* barrier variable to synchronize all running tasks */
pthread_barrier_t tasks_barrier;
pthread_barrier_t init_barrier;
/* logger message queue descriptor*/
extern mqd_t logTask_mq_d;

/***** Shared Memories variables for all tasks ****/
/* logger task shared memory*/
extern void *logTask_sh_mem;
extern int logTask_sm_fd;
/* temperature task shared memory*/
extern void *tempTask_sh_mem;
extern int tempTask_sm_fd;
/* light task shared memory*/
extern void *lightTask_sh_mem;
extern int lightTask_sm_fd;
/* socket task shared memory*/
extern void *socketTask_sh_mem;
extern int socketTask_sm_fd;

/**** KILL Flags for different tasks/threads*****/
int logTask_kill=0;
int status_thread_kill=0;
int tempTask_kill=0;
int lightTask_kill=0;
int socketTask_kill=0;

/**** Function prototypes ****/
void status_read_thread();
extern void logger_task_thread();
extern void temperature_task_thread();
extern void light_task_thread();
extern void socket_task_thread();

/* array of function pointers*/
static void (*thread_fun[NUM_THREADS]) = { logger_task_thread, status_read_thread, temperature_task_thread, light_task_thread, socket_task_thread };
timer_t main_timerid;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//****
 * @brief signal_handler()
 * This is a signal_handler for SIGUSR1 and SIGUSR2
 * @signal: occured signal number
 * @return: None
 ***********************************************************************/
void signal_handler(int signal)
{
  /* check the occured signal */
  switch(signal)
  {
    case SIGUSR1:
      /* Set flag to kill logger task */
      logTask_kill=1;
      lightTask_kill=1;
      tempTask_kill=1;
      socketTask_kill=1;      
      LOG_STD("[INFO] [SIG_HANDLER] USR1 RECEIVED\n");
      break;
    case SIGUSR2:
      status_thread_kill=1;
      LOG_STD("[INFO] [SIG_HANDLER] USR2 RECEIVED\n");
      break;   
    default:
      LOG_STD("[INFO] [SIG_HANDLER] UNKNOWN SIGNAL\n");  
   }
}

/******************************************************************//****
 * @brief timer_handler()
 * This is a timer_handler occurs every 3s and gets the status of all 
 * running threads(DEAD/ALIVE) by reading each tasks shared memory. 
 * And logs these statusses into logger message queue
 * @signal: occured signal number
 * @return: None
 ***********************************************************************/
void timer_handler(int signal)
{
  logTask_Msg_t logData;
  Task_Status_t logger_status, light_status, temp_status, socket_status;

  /******** Read Shared memories of all the tasks to get the statusses *****/
  /* read status from logger task shared memory */
  memcpy((char*)&logger_status,(char*)logTask_sh_mem,SM_SIZE);
  /* read status from light task shared memory */
  memcpy((char*)&light_status,(char*)lightTask_sh_mem,SM_SIZE);
  /* read status from logger task shared memory */
  memcpy((char*)&temp_status,(char*)tempTask_sh_mem,SM_SIZE);
  /* read status from light task shared memory */
  memcpy((char*)&socket_status,(char*)socketTask_sh_mem,SM_SIZE);
  

	LOG_STD("[INFO] [STATUS_HANDLER] READING TASK STATUS\n" );
	LOG_STD("[INFO] [STATUS_HANDLER] LOGGER TASK STATUS: [%s]\n",task_statusString[logger_status] );
	LOG_STD("[INFO] [STATUS_HANDLER] LIGHT TASK STATUS: [%s]\n", task_statusString[light_status]); 
	LOG_STD("[INFO] [STATUS_HANDLER] TEMPERATURE TASK STATUS: [%s]\n",task_statusString[temp_status] );
	LOG_STD("[INFO] [STATUS_HANDLER] SOCKET TASK STATUS: [%s]\n", task_statusString[socket_status]); 
   
  if( logger_status != DEAD ) //if logger DEAD no logger queue
  {
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"LOGGER TASK STATUS: [%s]\n", \
                                            task_statusString[logger_status]);
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"LIGHT TASK STATUS: [%s]\n", \
                                              task_statusString[light_status]); 
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"TEMPERATURE TASK STATUS: [%s]\n", \
                                              task_statusString[temp_status]);
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"SOCKET TASK STATUS: [%s]\n", \
                                              task_statusString[socket_status]);       
  }

}

/******************************************************************//**********
 * @brief status_read_thread()
 * This thread setups POSIX timer to expire every 3s inside which we read 
 * each task statuses 
 *****************************************************************************/
void status_read_thread(void) 
{
  struct itimerval timer;
  logTask_Msg_t logData;
  int ret;

	struct sigevent sev;
    struct itimerspec its;
    long long freq_nanosecs;
    sigset_t mask;
    struct sigaction sa;
    
	LOG_STD("[INFO] [MAIN_STATUS] WAITING AT TASK BARRIER\n");
	pthread_barrier_wait(&tasks_barrier);
	LOG_STD("[INFO] [MAIN_STATUS] CROSSED TASK BARRIER\n");

    /************** POSIX Timer setup *******/
	LOG_TO_QUEUE(logData,LOG_INFO,MAIN_TASK_ID,"SETTING TIMER HANDLER");
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_value.sival_ptr = &main_timerid;
    timer_create(CLOCK_REALTIME, &sev, &main_timerid);
    
    /* Start the timer */
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(main_timerid, 0, &its, NULL);
	LOG_TO_QUEUE(logData,LOG_INFO,MAIN_TASK_ID,"TIMER SETUP DONE");
	LOG_STD("[INFO] [MAIN_STATUS] POSIX TIMER SETUP DONE\n");

    while(!status_thread_kill);
    
	/*********** KILL Signal Received ***********/
	LOG_STD("[INFO] [MAIN_STATUS] KILL SIGNAL RECEIVED\n");
	timer_delete(main_timerid);
	LOG_STD("[INFO] [MAIN_STATUS] TIMER DELETED\n");

  /***** Unlink Shared Memories of all tasks ******/
  /* free logger task shared memory */
  ret= close(logTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] LOGTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] LOGTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(LOGTASK_SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] LOGTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] LOGTASK SHARED MEMORY UNLINKED\n");

  /* free light task shared memory */
  ret= close(lightTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] LIGHTTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] LIGHTTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(LIGHTTASK_SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] LIGHTTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] LIGHTTASK SHARED MEMORY UNLINKED\n");
  
  /* free temp task shared memory */
  ret= close(tempTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] TEMPTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] TEMPTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(TEMPTASK_SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] TEMPTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] TEMPTASK SHARED MEMORY UNLINKED\n");
  
  /* free socket task shared memory */
  ret= close(socketTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] SOCKETTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] SOCKETTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(SOCKETTASK_SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] [MAIN_STATUS] SOCKETTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
    LED_ON();
  }
  LOG_STD("[INFO] [MAIN_STATUS] SOCKETTASK SHARED MEMORY UNLINKED\n");  
  
  pthread_exit(NULL);
}

int system_test(void)
{
	Task_Status_t logger_status, light_status, temp_status, socket_status;

	/******** Read Shared memories of all the tasks to get the statusses *****/
	/* read status from logger task shared memory */
	memcpy((char*)&logger_status,(char*)logTask_sh_mem,SM_SIZE);
	/* read status from light task shared memory */
	memcpy((char*)&light_status,(char*)lightTask_sh_mem,SM_SIZE);
	/* read status from logger task shared memory */
	memcpy((char*)&temp_status,(char*)tempTask_sh_mem,SM_SIZE);
	/* read status from light task shared memory */
	memcpy((char*)&socket_status,(char*)socketTask_sh_mem,SM_SIZE);

	/* Start-up tests */

	if(light_start_test())
	{
		/* Enter log message */
		LED_ON();
		return ERROR;
	}
	if(temp_start_test())
	{
		/* Enter log message */
		LED_ON();
		return ERROR;
	}
	if(!(logger_status & light_status & temp_status & socket_status))
	{
		LED_ON();
		return ERROR;
	}

	return SUCCESS;
}

/******************************************************************//**********
 * @brief main()
 * This function creates creates multiple threads such as logger_task, 
 * temperature_task, light_task, socket_task & status read and 
 * waits for its completion
 *****************************************************************************/
int main(int argc, char** argv)
{
  pthread_t threads[NUM_THREADS];
  logTask_Msg_t logData;
  struct sigaction user_sig;
  int ret;
  int index;

  /* create barrier for all threads and main */
  pthread_barrier_init( &tasks_barrier, NULL, NUM_THREADS+1);

  pthread_barrier_init( &init_barrier, NULL, 3); //light+temp+main


  LED_INIT();
  LED_OFF();

  /**** Thread Creation *******/
  LOG_STD("[INFO] [MAIN] CREATING THREADS \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_create(&threads[index], NULL, thread_fun[index], NULL);
    if(ret==0)
    {
      LOG_STD("[INFO] [MAIN] THREAD %s CREATED \n", threadfun_name[index]);
    }
    else
    {
      LOG_STD("[ERROR] [MAIN] THREAD %s NOT CREATED: %s\n", threadfun_name[index], strerror(errno));
      LED_ON();
    }
  }

  LOG_STD("[INFO] [MAIN] WAITING AT TASK BARRIER \n");
  pthread_barrier_wait(&tasks_barrier);
  LOG_STD("[INFO] [MAIN] CROSSED TASK BARRIER \n");

  int sem_val;
	sem_t * sem_start;
	/* Start semaphore initialized to 2 */
	sem_start = sem_open(SEM_START, O_CREAT, 0660, 0);

  pthread_barrier_wait(&init_barrier);

	
	LOG_STD("[INFO] [MAIN] START UP TEST\n");
	//////startup here ////

	ret = system_test();
	if(ret == ERROR)
	{
		/* LOG message - Start up test failed */
    signal_handler(SIGUSR1);  //kill all threads
		return ERROR;
	}
	else if(ret == SUCCESS)
	{
		LOG_STD("[INFO] SYSTEM TEST SUCCESS! \n");
		sem_post(sem_start);
		sem_post(sem_start);
	}

  /****** Signal Handler Linking to SIGUSR1 & SIGUSR2 *******/
  user_sig.sa_handler= &signal_handler;
  /* link USR1 signal: kills all threads except status_read thread*/
  if( sigaction(SIGUSR1, &user_sig, NULL)<0 )
  {
    LOG_STD("[ERROR] [MAIN] USR1 NOT LINKED\n" );
    LED_ON();
    return 1;
  }
  /* link USR2 signal: kills status_read thread*/  
  if( sigaction(SIGUSR2, &user_sig, NULL)<0 )
  {
    LOG_STD("[ERROR] [MAIN] USR2 NOT LINKED\n" );
    LED_ON();
    return 1;
  }

  /***** Waiting for threads to complete ******/
  LOG_STD("[INFO] [MAIN] WAITING FOR THREADS COMPLETION \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_join(threads[index], NULL);
    if(ret==0)
    {
      LOG_STD("[INFO] [MAIN] THREAD %s COMPLETED\n", threadfun_name[index]);
    }
    else
    {
      LOG_STD("[ERROR] [MAIN] THREAD %s NOT COMPLETED: %s\n", threadfun_name[index], strerror(errno));
      LED_ON();
    }
  }

  exit(EXIT_SUCCESS);
}
