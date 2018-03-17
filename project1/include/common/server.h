#ifndef __SERVER_H__
#define __SERVER_H__

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#define PORT_ADR    2000

void * server(void * data);

#endif