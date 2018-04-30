/**
 * @file tcp_wrapper.c
 * @brief This file contains TCP/IP (Sockets) task functionality.
 *
 * @author Vipul Gupta
 * @date 28 Apr 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "main.h"
#include "tcp_wrapper.h"
#include "logger.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************

Socket_t xClientSocket;
xSemaphoreHandle tcp_init;

/* MaC Address hard coded */
static const uint8_t ucMACAddress[6] = {0x00, 0x1a, 0xb6, 0x03, 0x2b, 0xde};
static const uint8_t ucIPAddress[4] = {10, 0, 0, 12};
static const uint8_t ucNetMask[4] = {255, 255, 255, 0};
static const uint8_t ucGatewayAddress[4] = {10, 0, 0, 1};
static const uint8_t ucDNSServerAddress[4] = {8, 8, 8, 8};

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//***********/

void ClientSocketConnection(void)
{

    static const TickType_t xTimeOut = pdMS_TO_TICKS(5000);
    struct freertos_sockaddr xServerAddress;
    socklen_t xSize = sizeof(struct freertos_sockaddr);

    /* Server IP and port address */
    xServerAddress.sin_port = FreeRTOS_htons(2000);
    xServerAddress.sin_addr = FreeRTOS_inet_addr("10.0.0.14");

    /* Attempt to open the socket. */
    xClientSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                     FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
                                     FREERTOS_IPPROTO_TCP );

    /* Check the socket was created. */
    configASSERT(xClientSocket != FREERTOS_INVALID_SOCKET);

    /* Set send and receive time outs. */
    FreeRTOS_setsockopt( xClientSocket,
                         0,
                         FREERTOS_SO_RCVTIMEO,
                         &xTimeOut,
                         sizeof( xTimeOut ) );

    FreeRTOS_setsockopt( xClientSocket,
                         0,
                         FREERTOS_SO_SNDTIMEO,
                         &xTimeOut,
                         sizeof( xTimeOut ) );

    /* Bind the socket, but pass in NULL to let FreeRTOS+TCP choose the port number.
    See the next source code snipped for an example of how to bind to a specific
    port number. */
    FreeRTOS_bind(xClientSocket, &xServerAddress, xSize);

    LOG_STD("Bound and waiting to connect\n\r");

    /* try connecting multiple times before sending anything */
    while(!(FreeRTOS_connect(xClientSocket, &xServerAddress, sizeof(xServerAddress))));

    LOG_STD("Connected!\n\r");

}


void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    /* Do nothing! */
    return;
}

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent)
{
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the TCP/IP stack if they have not already
        been created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+TCP can be created here
             * to ensure they are not created before the network is usable.
             */
            LOG_STD("ClientSocketConnection() Calling..\n\r");
            xSemaphoreGive(tcp_init);
//            ClientSocketConnection(); //call in main (not executed here)
            xTasksAlreadyCreated = pdTRUE;
        }
    }
}

void Configure_TCP(void)
{
    tcp_init = xSemaphoreCreateBinary();

    /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
    are created in the vApplicationIPNetworkEventHook() hook function
    below.  The hook function is called when the network connects. */
    FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress);

    xSemaphoreTake(tcp_init, portMAX_DELAY);
    ClientSocketConnection();
}

uint32_t TCP_Send(void * buffer, size_t datalength)
{
    int32_t ret = 0;
    ret = FreeRTOS_send(xClientSocket, buffer, datalength, 0);
    return ret;
}

uint32_t TCP_Receive(void * buffer, size_t datalength)
{
    int32_t ret = 0;
    ret = FreeRTOS_recv(xClientSocket, buffer, datalength, 0);
    return ret;
}
