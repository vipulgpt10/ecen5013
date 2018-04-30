#ifndef UART_NEW_H
#define UART_NEW_H 

#include<stdint.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>



typedef enum {HEARTBEAT_MISSED=0, HEARTBEAT_RECEIVED=1} heartbeat_t;

#define NUMBER_OF_THREADS_TIVA      4

/* options for serial port config */
void tty_config(struct termios *con, int descriptor);

/* read UART byte */
void read_byte(int fd,char *received);

/* read string */
void read_string(int fd,char *string);

/* read the structure */
void read_struct(int descriptor, logTask_Msg_t* recvd_struct);

/* initialize UART */
void uart_init(void);

#endif
