/**
 * @file uart.c
 * @brief This file contains UART task functionality.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <logger.h>
#include <uart_wrapper.h>
#include "main.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
extern uint32_t g_ui32SysClock;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief uart_send_string()
 * This function creates sends string through UART
 *****************************************************************************/

/******************************************************************//****
 * @brief ConfigureUART()
 * This function configures UART and its pins.
 * This must be called before UARTprintf()
 *
 ***********************************************************************/
void ConfigureUART(void)
{
    /********* UART0 Configurations to use UARTprintf **********/
 /* Enable the GPIO Peripheral used by the UART*/
 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

 /* Enable UART*/
 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

 /* Configure GPIO Pins for UART mode*/
 ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
 ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
 ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

 UARTStdioConfig(0, 115200, g_ui32SysClock);

 /*****UART3 Configurations to communicate b/w TIVA and BBB******/
 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
 /* Wait for pheripheral to be ready */
 while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_UART3));

 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
 /* Wait for pheripheral to be ready */
 while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));


 GPIOPinConfigure(GPIO_PA4_U3RX);
 GPIOPinConfigure(GPIO_PA5_U3TX);
 ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4);

ROM_UARTConfigSetExpClk(UART3_BASE, g_ui32SysClock, 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));

LOG_STD("[INFO] [UART] UART Configured\n");
}

void uart_send_string(uint8_t *str)
{
    size_t i = 0 ;

    /* Find position of null terminator */
    while(*(str+i) != '\0' )
    {
      i++;
    };

    uart_send_block((uint8_t*)str, i);
}

/******************************************************************//**********
 * @brief uart_send_string()
 * This function creates sends string through UART
 *****************************************************************************/
void uart_send_byte(uint8_t byte)
{
    ROM_UARTCharPut(UART3_BASE, byte);
}

/******************************************************************//**********
 * @brief uart_send_string()
 * This function creates sends string through UART
 *****************************************************************************/
void uart_receive_string(void)
{
    size_t i = 0 ;
    uint8_t byte=0;

    do
    {
        byte=UARTCharGet(UART3_BASE);
        UARTprintf("%c", byte);
    }while(byte != '\0');

}

void uart_receive_packet( logTask_Msg_t *Recvpacket, size_t packet_size)
{
    uint8_t *byte= Recvpacket;

    while(packet_size--)
    {
        *byte++=UARTCharGet(UART3_BASE);
    }
}

void uart_send_packet( logTask_Msg_t *Sendpacket, size_t packet_size)
{
    uint8_t *byte= Sendpacket;

    while(packet_size--)
    {
        uart_send_byte(*byte++);
    }

#if 0
    //uart_send_block( (uint8_t*)&logMesg_recv, sizeof(logMesg_recv) );
    if( (LOG_INFO ==logMesg_recv.message_type) || (LOG_ERR ==logMesg_recv.message_type) )
    {
        uart_send_byte(logMesg_recv.message_type);
        uart_send_byte(logMesg_recv.task_ID);
        uart_send_byte(logMesg_recv.log_message);
    }
    else if (LOG_STATUS ==logMesg_recv.message_type )
    {
        uart_send_byte(logMesg_recv.message_type);
        uart_send_byte(NUM_TASKS); //send number of tasks
        for(index=0; index<NUM_TASKS; index++)
        {
            uart_send_byte(logMesg_recv.task_status[index]);
        }
    }
    else //(LOG_DATA ==logMesg_recv.message_type)
    {
        uart_send_byte(logMesg_recv.message_type);
        uart_send_byte(logMesg_recv.task_ID);
        uart_send_byte(logMesg_recv.pedometer_data);
        uart_send_byte(logMesg_recv.pulserate_data);
    }
#endif
}
