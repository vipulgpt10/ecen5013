/**
 * @file main_time.c
 * @brief This file contains timer function to get the real time.
 
 * @author Kovuru Nagarjuna Reddy
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "my_time.h"
#include "main_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
#define GET_TIMEOFDAY(x,y)	gettimeofday(x,y)

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//****
 * @brief get_time_string()
 * This function gets the current time and converts into a string to be 
 * printed in logger message 
 * @timestring: Pointer to buffer to store the time as a string
 * @len       : length of buffer
 * @return: status(SUCCESS/FAILURE)
 ***********************************************************************/
int get_time_string(char *timeString, const int len)
{
  char time_string[20] = {0};
  struct timespec current_timeSpec;
  struct tm *current_time;

  /* get the current time */
  clock_gettime( CLOCK_REALTIME, &current_timeSpec);
  current_time = localtime( &current_timeSpec );
  /* store the time in buffer as a string */
  snprintf(time_string,sizeof(time_string),"%02d:%02d:%02d", \
        current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
  memcpy(timeString,time_string, sizeof(time_string) );
 
  return SUCCESS;
}
