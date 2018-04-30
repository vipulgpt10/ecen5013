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
const char *threadfun_name[]= { "logger_task_thread"};
/* array of task id strings*/
const char* taskId_nameString[MAIN_TASK_ID]= { "LOGGER_TASK_ID" };
/* barrier variable to synchronize all running tasks */
pthread_barrier_t tasks_barrier;

/******* Function Prototypes **************/
extern void logger_task_thread();

/* array of function pointers*/
static void (*thread_fun[NUM_THREADS]) = { logger_task_thread };


//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief main()
 * This function creates creates multiple threads such as logger_task, 
 * temperature_task, light_task, socket_task & status read and 
 * waits for its completion
 *****************************************************************************/
int main( int argc, char** argv )
{
  pthread_t threads[NUM_THREADS];
  int ret;
  int index;

  /* create barrier for all threads and main */
  pthread_barrier_init( &tasks_barrier, NULL, NUM_THREADS+1);
    
  LOG_STD("[INFO] UART INITIALIZED\n");

  sleep(1);

  /**** Thread Creation *******/
  printf("[INFO] CREATING THREADS \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_create(&threads[index], NULL, thread_fun[index], NULL);
    if(ret==0)
    {
      printf("[INFO] THREAD %s CREATED \n", threadfun_name[index]);
    }
    else
    {
      printf("[ERROR] THREAD %s NOT CREATED: %s\n", threadfun_name[index], strerror(errno));
    }
  }

  pthread_barrier_wait(&tasks_barrier);
  
    /************** Signal Handler Linking to SIGUSR1 & SIGUSR2 *******/

  /***** Waiting for threads to complete ******/
  printf("[INFO] WAITING FOR THREADS COMPLETION \n");
  /* run loop required thread number of times */
  for (index = 0; index< NUM_THREADS; index++) 
  {
    ret=pthread_join(threads[index], NULL);
    if(ret==0)
    {
      printf("[INFO] THREAD %s COMPLETED\n", threadfun_name[index]);
    }
    else
    {
      printf("[ERROR] THREAD %s NOT COMPLETED: %s\n", threadfun_name[index], strerror(errno));
    }
  }

    exit(EXIT_SUCCESS);
}
