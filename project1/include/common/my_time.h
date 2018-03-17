/**
* @file my_time.h
* @brief This file contains timer functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Kovuru Nagarjuna Reddy
* @date March 16 2018
*
*/
 
#ifndef TIME_H
#define TIME_H
//***********************************************************************************
// Include files
//***********************************************************************************


//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************


//***********************************************************************************
// Global Variables 
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//****
 * @brief get_time_string()
 * This function gets the current time and converts into a string to be 
 * printed in logger message 
 * @timestring: Pointer to buffer to store the time as a string
 * @len       : length of buffer
 * @return: status(SUCCESS/FAILURE)
 ***********************************************************************/
int get_time_string(char *timeString, int len);

#endif
