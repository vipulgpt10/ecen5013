/**
* @file logger_task.h
* @brief This file contains logger task functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Kovuru Nagarjuna Reddy
* @date March 14 2018
*
*/
#ifndef __UART_H__
#define __UART_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************

//***********************************************************************************
// Global Variables 
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
void ConfigureUART(void);
void uart_send_string(uint8_t *str);
void uart_send_byte(uint8_t byte);
void uart_receive_string(void);
void uart_send_packet( logTask_Msg_t *Sendpacket, size_t packet_size);
void uart_receive_packet( logTask_Msg_t *Recvpacket, size_t packet_size);

#endif /* __LOGGER_H__ */
