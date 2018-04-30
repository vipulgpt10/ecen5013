/**
* @file tcp_wrapper.h
* @brief This file contains TCP/Ip (Sockets) functionalities
*
* This header file provides an abstraction of Macros, variables and
* profiling via function calls.
*
* @author Vipul Gupta
* @date 28 Apr 2018
*
*/

#ifndef __TCP_WRAPPER_H__
#define __TCP_WRAPPER_H__

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

void ClientSocketConnection(void);

void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier);

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent);

void Configure_TCP(void);

uint32_t TCP_Send(void * buffer, size_t datalength);

uint32_t TCP_Receive(void * buffer, size_t datalength);

#endif /* __TCP_WRAPPER_H__ */



