/**
 * @file temp_task.c
 * @brief This file contains temp task functionality.
 *
 * @author Vipul Gupta
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "temp_task.h"
#include <fcntl.h>
#include <errno.h>
#include "main_task.h"
#include "logger_task.h"
#include "led_task.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* temp shared memory*/
void *tempTask_sh_mem;
int tempTask_sm_fd;
/* temp task kill flag*/
extern int tempTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
extern pthread_barrier_t init_barrier;
extern mqd_t logTask_mq_d;
timer_t temp_timerid;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief read_temp_celsius()
 * This function reads temperature in celcius
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_TEMP_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

/******************************************************************//**********
 * @brief read_temp_fahrenheit()
 * This function reads temperature in farenheit
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_fahrenheit(float * data)
{
	float fah_temp;
	int8_t ret;

	ret = read_temp_celsius(&fah_temp);
	*data = (fah_temp * 1.8) + 32;
	return ret;
}

/******************************************************************//**********
 * @brief read_temp_kelvin()
 * This function reads temperature in kelvins
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_temp_kelvin(float * data)
{
	float kel_temp;
	int8_t ret;

	ret = read_temp_celsius(&kel_temp);
	*data = kel_temp + 273.15;
	return ret;
}

/******************************************************************//**********
 * @brief write_config_reg()
 * This function writes configuration register
 * @data: data to write
 *****************************************************************************/
int8_t write_config_reg(const uint16_t data)
{
	uint16_t tmp1, tmp2, msb_val;
	tmp1 = data & 0xFF00;
	tmp2 = data & 0x00FF;
	msb_val = (tmp1 >> 8 | tmp2 << 8);

	return i2c_write_word(TMP102_DEV_ADDR, TMP102_CONF_REG, msb_val);
}

/******************************************************************//**********
 * @brief read_config_reg()
 * This function reads configuration register
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_config_reg(uint16_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(TMP102_DEV_ADDR, TMP102_CONF_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    val = ((uint16_t)arr[0] << 8 | arr[1]);
    *data = val;
    return ret;
}

/******************************************************************//**********
 * @brief read_tlow_celsius()
 * This function reads tlow
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_tlow_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_TLOW_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

/******************************************************************//**********
 * @brief read_thigh_celsius()
 * This function reads thigh
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_thigh_celsius(float * data)
{
	int8_t ret;
	float temp;
	uint8_t arr[2];
	uint16_t temp_reg;

	ret = i2c_read_bytes(TMP102_DEV_ADDR, TMP102_THIGH_REG, arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

    temp_reg = ((uint16_t)arr[0] << 8 | arr[1]);

    /* 12 higher bits temperature data */
    temp_reg = temp_reg >> 4;

    if(!(temp_reg & (1 << 11)))
    {
    	temp = temp_reg * 0.0625;
    	*data = temp;
    }
    else
    {
    	/* Two's complement of only 12 lower bits*/
    	temp_reg = temp_reg | (0xF000);
    	temp_reg = ~(temp_reg) + 1;
    	temp = temp_reg * (-0.0625);
    	*data = temp;
    }

    return ret;
}

/******************************************************************//**********
 * @brief temp_task_init()
 * This function creates shared memory to share the task status.
 * Shared Memory: To share temp task's status(DEAD/ALIVE) with main_task.
 *****************************************************************************/
int temp_task_init(void) 
{
  logTask_Msg_t logData;
  int ret;
  Task_Status_t temp_status;

  /************Create Shared Memory to share task status with main ******/  
  tempTask_sm_fd = shm_open(TEMPTASK_SM_NAME, O_CREAT | O_RDWR, 0666);
  if(tempTask_sm_fd == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,TEMP_TASK_ID,"SHARED MEMORY NOT CREATED");
    LED_ON();
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"SHARED MEMORY CREATED");

  /* truncate shared memory with required size */
  ret = ftruncate(tempTask_sm_fd, SM_SIZE);
  if(ret == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,TEMP_TASK_ID,"SHARED MEMORY NOT TRUNCATED");
    LED_ON();
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"SHARED MEMORY TRUNCATED");

  /*map the shared memory */
  tempTask_sh_mem = mmap(NULL, SM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , tempTask_sm_fd, 0);
  if(tempTask_sh_mem == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,TEMP_TASK_ID,"SHARED MEMORY NOT MAPPED");
    LED_ON();
    return -1;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"SHARED MEMORY MAPPED");

  /**** Enter the task status into shared memory *****/
  temp_status=ALIVE;
  /* Copy the contents of payload into the shared memory */
  memcpy((char*)tempTask_sh_mem, (char*)&temp_status, SM_SIZE);

  LOG_STD("[INFO] [TEMP] TASK INITIALIZED SHARED MEMORY\n");

  return SUCCESS;
}

/******************************************************************//****
 * @brief temp_timer_handler()
 * This is a timer_handler occurs every 3s and updates the temperature
 * parameters
 * @signal: occured signal number
 * @return: None
 ***********************************************************************/
void temp_timer_handler(int signal)
{
  float temp;

  /***** Read the sensor and display****/
  LOG_STD("[INFO] [TEMP_HANDLER] READING TEMPERATURE SENSOR\n" );
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"READING TEMPERATURE SENSOR");
  
  read_temp_celsius(&temp);
  LOG_STD("[INFO] [TEMP_HANDLER] TEMPERATURE IN CELSIUS: %f\n", temp );
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"TEMPERATURE IN CELSIUS: %f\n", temp);
  
  read_temp_fahrenheit(&temp);
  LOG_STD("[INFO] [TEMP_HANDLER] TEMPERATURE IN FARENHEIT: %f\n", temp );
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"TEMPERATURE IN FARENHEIT: %f\n", temp);
  
  read_temp_kelvin(&temp);
  LOG_STD("[INFO] [TEMP_HANDLER] TEMPERATURE IN KELVIN: %f\n", temp );
  LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"TEMPERATURE IN KELVIN: %f\n", temp);
}

/******************************************************************//**********
 * @brief temperature_task_thread()
 * This thread initializes the temperature task resources such as shared memory
 *****************************************************************************/
void temperature_task_thread(void) 
{
	logTask_Msg_t logData;
	Task_Status_t temp_status;
	struct itimerval timer;
    struct sigaction timer_sig;
    int ret;
	struct sigevent sev;
    struct itimerspec its;
    long long freq_nanosecs;
    sigset_t mask;
    struct sigaction sa;
	
	LOG_STD("[INFO] [TEMP] WAITING AT TASK BARRIER \n");
	/* wait temp task so that other tasks(temp task queue) are synchronized with it*/
	pthread_barrier_wait(&tasks_barrier);
	LOG_STD("[INFO] [TEMP] CROSSED TASK BARRIER \n");
	
	LOG_STD("[INFO] [TEMP] TASK STARTED\n");
	LOG_TO_QUEUE(logData,LOG_INFO, TEMP_TASK_ID,"TEMP TASK STARTED");	
	ret= temp_task_init();
	if(ERROR == ret)
	{
		LOG_STD("[ERROR] [TEMP] TASK INIT: %s\n", strerror(errno));
		LED_ON();
		exit(ERROR);
	}
	LOG_TO_QUEUE(logData,LOG_INFO, TEMP_TASK_ID,"TEMP TASK INITIALIZED");

	pthread_barrier_wait(&init_barrier);

	int sem_val;
	sem_t * sem_start;
	/* Start semaphore initialized to 2 */
	sem_start = sem_open(SEM_START, O_CREAT);
	sem_wait(sem_start);

    /************** POSIX Timer setup *******/
	LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"SETTING TIMER HANDLER");
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = temp_timer_handler;
    sev.sigev_value.sival_ptr = &temp_timerid;
    timer_create(CLOCK_REALTIME, &sev, &temp_timerid);
    
    /* Start the timer */
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(temp_timerid, 0, &its, NULL);
	LOG_TO_QUEUE(logData,LOG_INFO,TEMP_TASK_ID,"TIMER SETUP DONE");
	LOG_STD("[INFO] [TEMP] POSIX TIMER SETUP DONE\n");
    
    while(!tempTask_kill);
    
	/*********** KILL Signal Received ***********/
	LOG_STD("[INFO] [TEMP] TASK KILL SIGNAL RECEIVED\n");
	timer_delete(temp_timerid);
	LOG_STD("[INFO] [TEMP] TIMER DELETED\n");
	/* Update task status in shared memory */
	temp_status=DEAD;
	/* Copy the contents of payload into the share memory */
	memcpy((char*)tempTask_sh_mem, (char*)&temp_status, SM_SIZE);

	pthread_exit(NULL);
}

