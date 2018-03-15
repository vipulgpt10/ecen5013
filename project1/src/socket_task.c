/**
 * @file socket_task.c
 * @brief This file contains socket task functionality.
 *
 * @author Vipul Gupta
 * @date Mar 16, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "socket_task.h"
#include "light_task.h"
#include "temp_task.h"
#include "led_task.h"
#include "logger_task.h"
#include "main_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* logger shared memory*/
void *socketTask_sh_mem;
int socketTask_sm_fd;
/* logger task kill flag*/
extern int socketTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
extern mqd_t logTask_mq_d;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief socket_task_init()
 * This function creates shared memory to share the task status.
 * Shared Memory: To share logger task's status(DEAD/ALIVE) with main_task.
 *****************************************************************************/
int socket_task_init(void) 
{
  logTask_Msg_t logData;
  int ret;
  Task_Status_t socket_status;

  /************Create Shared Memory to share task status with main ******/  
  socketTask_sm_fd = shm_open(SOCKETTASK_SM_NAME, O_CREAT | O_RDWR, 0666);
  if(socketTask_sm_fd == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,SOCKET_TASK_ID,"SHARED MEMORY NOT CREATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,SOCKET_TASK_ID,"SHARED MEMORY CREATED");

  /* truncate shared memory with required size */
  ret = ftruncate(socketTask_sm_fd, SM_SIZE);
  if(ret == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,SOCKET_TASK_ID,"SHARED MEMORY NOT TRUNCATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,SOCKET_TASK_ID,"SHARED MEMORY TRUNCATED");

  /*map the shared memory */
  socketTask_sh_mem = mmap(NULL, SM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , socketTask_sm_fd, 0);
  if(socketTask_sh_mem == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,SOCKET_TASK_ID,"SHARED MEMORY NOT MAPPED");
    return -1;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,SOCKET_TASK_ID, "SHARED MEMORY MAPPED");

  /**** Enter the task status into shared memory *****/
  socket_status=ALIVE;
  /* Copy the contents of payload into the shared memory */
  memcpy((char*)socketTask_sh_mem, (char*)&socket_status, SM_SIZE);

  LOG_STD("[INFO] SOCKET TASK INITIALIZED SHARED MEMORY\n");

  return SUCCESS;
}

/******************************************************************//**********
 * @brief socket_task_thread()
 * This thread initializes socket connection
 *****************************************************************************/
void socket_task_thread(void) 
{
	logTask_Msg_t logData;
	Task_Status_t socket_status;
	struct itimerval timer;
    struct sigaction timer_sig;
    int ret;
    struct sockaddr_in addr, peer_addr;
    int addr_len = sizeof(peer_addr);
    int i =0;
    size_t len, read_b;
    char read_buff[100];
    char write_buff[100];
    int server_socket, accepted_soc, opt = 1;
    
    /* wait socket task so that other tasks(logger task queue) are synchronized with it*/
	pthread_barrier_wait(&tasks_barrier);
	
	LOG_STD("[INFO] SOCKET TASK STARTED\n");
	LOG_TO_QUEUE(logData,LOG_INFO, SOCKET_TASK_ID,"SOCKET TASK STARTED");	
	ret= socket_task_init();
	if(ERROR == ret)
	{
		LOG_STD("[ERROR] SOCKET TASK INIT: %s\n", strerror(errno));
		exit(ERROR);
	}
	LOG_TO_QUEUE(logData,LOG_INFO, SOCKET_TASK_ID,"SOCKET TASK INITIALIZED");
	

    while(!socketTask_kill)
    {
      
      /* create socket */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // printf("[Server] [ERROR] Socket creation Error\n");
        // return -1;
    }
    else
      // printf("[Server] Socket Created Successfully\n");

    /* set socket options */
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(opt), sizeof(opt)))
    {
        // printf("[Server] [ERROR] Socket options set error\n");
        // return -1;
    }

    /*Set the sockaddr_in structure */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  
    addr.sin_port = htons(PORT_ADR);

    /*bind socket to a address */
    if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
    {
        // printf("[Server] [ERROR] Bind socket Error\n");
        // return -1;
    }
    else
      // printf("[Server] Socket binded Successfully\n");

    /* listen for connections*/
    if(listen(server_socket, 5) < 0)
    {
        // printf("[Server] [ERROR] Can't listen connection\n");
        // return -1;
    }
    
    /*accept connection */
    accepted_soc = accept(server_socket, (struct sockaddr*)&peer_addr,
                (socklen_t*)&addr_len);
    if(accepted_soc < 0)
    {
        // printf("[Server] [ERROR] Can't accept connection\n");
        // return -1;
    }

    
    read_b = read(accepted_soc, &len, sizeof(size_t));
    if(!(read_b == sizeof(size_t)))
    {
        // printf("[Server] [ERROR] Invalid data\n");
        // return -1;
    }

    while((read_b = read(accepted_soc, read_buff+i, 100)) < len)
    {
        i += read_b;  
    }

    printf("==== SERVER thread ++++\n");
    printf("[Server] Message received from Client: %s\n", read_buff);

    float temp;
    float lux;
    char tmp_msg[100];

    if((strcmp(read_buff, "is_it_day?")) == 0)
    {

        read_sensor_lux(&lux);
        printf("Function call in light thread!\n");
        sprintf(tmp_msg, "Sensor LUX = %f \n", lux);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
     
    }
    else if((strcmp(read_buff, "is_it_night?")) == 0)
    {
        read_sensor_lux(&lux);
        printf("Function call in light thread!\n");
        sprintf(tmp_msg, "Sensor LUX = %f \n", lux);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "get_sensor_lux")) == 0)
    {
        read_sensor_lux(&lux);
        printf("Function call in light thread!\n");
        sprintf(tmp_msg, "Sensor LUX = %f \n", lux);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "get_temp_cel")) == 0)
    {
        read_temp_celsius(&temp);
        printf("Function call in temperature thread!\n");
        sprintf(tmp_msg, "Temperature = %f deg C\n", temp);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "get_temp_fah")) == 0)
    {
        read_temp_fahrenheit(&temp);
        printf("Function call in temperature thread!\n");
        sprintf(tmp_msg, "Temperature = %f deg F\n", temp);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "get_temp_kel")) == 0)
    {
        read_temp_kelvin(&temp);
        printf("Function call in temperature thread!\n");
        sprintf(tmp_msg, "Temperature = %f deg K\n", temp);
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "LED_ON")) == 0)
    {
        LED_ON();
        sprintf(tmp_msg, "USR_LED0 = ON\n");
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
    else if((strcmp(read_buff, "LED_OFF")) == 0)
    {
        LED_OFF();
        sprintf(tmp_msg, "USR_LED0 = OFF\n");
        send(accepted_soc, tmp_msg, sizeof(tmp_msg), 0);
    }
        
    printf("==== SERVER thread ++++\n");

    close(accepted_soc);

    close(server_socket);
    }

    LOG_STD("[INFO] USR1:SOCKET THREAD KILL SIGNAL RECEIVED\n");
    
	/*********** KILL Signal Received ***********/
	LOG_STD("[INFO] SOCKET TASK KILL SIGNAL RECEIVED\n");
	/* Update task status in shared memory */
	socket_status=DEAD;
	/* Copy the contents of payload into the share memory */
	memcpy((char*)socketTask_sh_mem, (char*)&socket_status, SM_SIZE);
	
    pthread_exit(NULL);
}

