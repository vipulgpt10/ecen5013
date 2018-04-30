/**
 * @file logger_task.c
 * @brief This file contains logger task functionality.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include "main_task.h"
#include "logger_task.h"
#include "uart.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
struct termios *configure;
void tty_config(struct termios *option, int descriptor);
char *device = "/dev/ttyS4";
int fd;

/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
extern mqd_t logTask_mq_d;
extern const char* logMessage_nameString[MESSAGE_MAX+1];
extern const char* messageType_nameString[LOG_ALL+1];
extern const char* task_statusString[];
extern const char* taskId_nameString[MAIN_TASK_ID];
/* logger file pointer */
extern FILE* log_fp;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
void configuration(struct termios *option, int descriptor)
{
    tcgetattr(descriptor, option);
    option->c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    option->c_oflag = 0;
    option->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    option->c_cc[VMIN] = 1;
    option->c_cc[VTIME] = 0;

    if(cfsetispeed(option, B115200) || cfsetospeed(option, B115200))
    {
        perror("Baud Rate Error\n");
    }

    if(tcsetattr(descriptor, TCSAFLUSH, option) < 0)
    {
        perror("Attribute Error\n");
    }
}

void uart_init(void)
{
    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd==-1)
    {
        perror("ERROR opening file descriptor\n");
    }

    configure = (struct termios*)malloc(sizeof(struct termios));
    if(configure==NULL){
        printf("Malloc fail");
    }
    configuration(configure, fd);
}

void read_packet(int descriptor, logTask_Msg_t *readPacket, size_t size )
{  
	// uint8_t *byte = readPacket;
	
	read(descriptor, (void *)readPacket, size);     
}


