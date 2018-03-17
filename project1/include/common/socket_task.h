/**
 * @brief 
 * 
 * @file socket_task.h
 * @author Kovuru Nagarjuna Reddy
 * @date 2018-03-09
 */

#ifndef SOCKET_TASK_H
#define SOCKET_TASK_H

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief 
 * 
 * @param threadparam 
 * @return void* 
 */
void* socket_task_callback(void* threadparam);

#endif
