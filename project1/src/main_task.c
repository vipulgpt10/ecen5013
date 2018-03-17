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
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include "main_task.h"
#include "logger_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* array of task status strings */
const char* task_statusString[]={"DEAD", "ALIVE"};
/* array of function name strings*/
const char *threadfun_name[]= { "logger_task_thread", "status_read_thread"};
/* array of task id strings*/
const char* taskId_nameString[MAIN_TASK_ID+1]= { "LOGGER_TASK_ID", "SOCKET_TASK_ID", \
            "TEMP_TASK_ID", "LIGHT_TASK_ID", "GPIO_TASK_ID", "MAIN_TASK_ID" };
/* barrier variable to synchronize all running tasks */
pthread_barrier_t tasks_barrier;
/* logger message queue descriptor*/
extern mqd_t logTask_mq_d;
/* logger task shared memory*/
extern void *logTask_sh_mem;
extern int logTask_sm_fd;

/**** KILL Flags for different tasks/threads*****/
int logTask_kill=0;
int status_thread_kill=0;

/**** Function prototypes ****/
void status_read_thread();
extern void logger_task_thread();
extern void socket_task_thread();

/* array of function pointers*/
static void (*thread_fun[NUM_THREADS]) = { logger_task_thread, status_read_thread };
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
      LOG_STD("[INFO] USR1 RECEIVED\n");
      break;
    case SIGUSR2:
      status_thread_kill=1;
      LOG_STD("[INFO] USR2 RECEIVED\n");
      break;   
    default:
      LOG_STD("[INFO] UNKNOWN SIGNAL\n");  
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
  Task_Status_t logger_status;

  /* read status from logger task shared memory */
  memcpy((char*)&logger_status,(char*)logTask_sh_mem,SM_SIZE);

  /*** similarly read other task statusses from their shared memory ****/

  LOG_STD("[INFO] READING TASK STATUS\n" );
  printf("Logger Status:%s\n",task_statusString[logger_status]);
  if( logger_status != DEAD )
  {
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"LOGGER TASK STATUS: [%s]\n", \
                                            task_statusString[logger_status]);
  }
  else //logger DEAD so no logging to logger queue instead to standard output
  {
    LOG_STD("[INFO] LOGGER TASK STATUS: [%s]\n",task_statusString[logger_status] );
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
  struct sigaction timer_sig;
  logTask_Msg_t logData;
  int ret;

  pthread_barrier_wait(&tasks_barrier);

  /************** Signal Handler Linking to POSIX timer *******/
  memset( &timer_sig, 0, sizeof(timer_sig) );
  timer_sig.sa_handler= &timer_handler;
  if( sigaction( SIGVTALRM, &timer_sig, NULL)<0 )
  {
    LOG_TO_QUEUE(logData,LOG_ERR,MAIN_TASK_ID,"POSIX TIMER CAN'T BE LINKED");
    return 1;
  }

  LOG_TO_QUEUE(logData,LOG_INFO,MAIN_TASK_ID,"POSIX TIMER LINKED");

  /* set up timer to expire every 100ms */
  timer.it_value.tv_sec= 1;
  timer.it_value.tv_usec=0;  
  timer.it_interval.tv_sec=1;
  timer.it_interval.tv_usec=0; 
  setitimer( ITIMER_VIRTUAL, &timer, NULL);

  LOG_TO_QUEUE(logData,LOG_INFO,MAIN_TASK_ID,"POSIX TIMER SETUP DONE");
  LOG_STD("[INFO] POSIX TIMER SETUP DONE\n");

  while(!status_thread_kill);
  
  LOG_STD("[INFO] USR2: STATUS READ THREAD KILL SIGNAL RECEIVED\n");

  /***** Unlink Shared Memories of all tasks ******/
  ret= close(logTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LOGTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
  }
  LOG_STD("[INFO] LOGTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LOGTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
  }
  LOG_STD("[INFO] LOGTASK SHARED MEMORY UNLINKED\n");


  /***** similarly unlink each task shared memories ******/
  pthread_exit(NULL);
}

/******************************************************************//**********
 * @brief main()
 * This function creates creates multiple threads such as logger_task, 
 * temperature_task, light_task, socket_task & status read and 
 * waits for its completion
 *****************************************************************************/
int main( int argc, char** argv )
{
  pthread_t threads[NUM_THREADS];
  logTask_Msg_t logData;
  struct sigaction user_sig;
  int ret;
  int index;

  /* create barrier for all threads and main */
  pthread_barrier_init( &tasks_barrier, NULL, NUM_THREADS+1);

  /**** Thread Creation *******/
  LOG_STD("[INFO] CREATING THREADS \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_create(&threads[index], NULL, thread_fun[index], NULL);
    if(ret==0)
    {
      LOG_STD("[INFO] THREAD %s CREATED \n", threadfun_name[index]);
    }
    else
    {
      LOG_STD("[ERROR] THREAD %s NOT CREATED: %s\n", threadfun_name[index], strerror(errno));
    }
  }

  pthread_barrier_wait(&tasks_barrier);

  /************** Signal Handler Linking to SIGUSR1 & SIGUSR2 *******/
  user_sig.sa_handler= &signal_handler;
  
  if( sigaction(SIGUSR1, &user_sig, NULL)<0 )
  {
    LOG_STD("[ERROR] USR1 NOT LINKED\n" );
    return 1;
  }

  /***** Waiting for threads to complete ******/
  LOG_STD("[INFO] WAITING FOR THREADS COMPLETION \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_join(threads[index], NULL);
    if(ret==0)
    {
      LOG_STD("[INFO] THREAD %s COMPLETED\n", threadfun_name[index]);
    }
    else
    {
      LOG_STD("[ERROR] THREAD %s NOT COMPLETED: %s\n", threadfun_name[index], strerror(errno));
    }
  }

  exit(EXIT_SUCCESS);
}
