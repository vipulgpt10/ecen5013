/**
 * @file light_task.c
 * @brief This file contains light task functionality.
 *
 * @author Vipul Gupta
 * @date Mar 14, 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "logger_task.h"
#include "light_task.h"
#include <fcntl.h>
#include <errno.h>
#include "main_task.h"


//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
/* light shared memory*/
void *lightTask_sh_mem;
int lightTask_sm_fd;
/* light task kill flag*/
extern int lightTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;
extern mqd_t logTask_mq_d;

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief write_control_reg()
 * This function writes sensor control register with data,
 * @data: data to write
 *****************************************************************************/
int8_t write_control_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}

/******************************************************************//**********
 * @brief read_control_reg()
 * This function reads sensor control register,
 * @data: pointer to store read data
 *****************************************************************************/
int8_t read_control_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}

/******************************************************************//**********
 * @brief write_timing_reg()
 * This function writes to timing register,
 * @data: data to write
 *****************************************************************************/
int8_t write_timing_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

/******************************************************************//**********
 * @brief read_timing_reg()
 * This function reads timing register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_timing_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

/******************************************************************//**********
 * @brief write_intthresh_low_reg()
 * This function writes to intthreshold low register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intthresh_low_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHLOW_REG), data);
}

/******************************************************************//**********
 * @brief read_intthresh_low_reg()
 * This function reads intthreshold register,
 * @data: pointer to stor ethe read data
 *****************************************************************************/
int8_t read_intthresh_low_reg(uint16_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTTHLOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

	val = ((uint16_t)arr[1] << 8 | arr[0]);
 	*data = val;
	return ret;
}

/******************************************************************//**********
 * @brief write_intthresh_high_reg()
 * This function writes to inthreshold register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intthresh_high_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHHIGH_REG), data);
}

/******************************************************************//**********
 * @brief read_intthresh_high_reg()
 * This function reads intthreshold register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_intthresh_high_reg(uint8_t * data)
{
	int8_t ret;
	uint16_t val;
	uint8_t arr[2];

	ret =  i2c_read_bytes(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTTHHIGH_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

	val = ((uint16_t)arr[1] << 8 | arr[0]);
	*data = val;
	return ret;
}

/******************************************************************//**********
 * @brief write_intcontrol_reg()
 * This function writes to intcontrol register,
 * @data: data to write
 *****************************************************************************/
int8_t write_intcontrol_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

/******************************************************************//**********
 * @brief read_intcontrol_reg()
 * This function reads intcontrol register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_intcontrol_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

/******************************************************************//**********
 * @brief read_id_reg()
 * This function reads id register,
 * @data: pointer to store the read data
 *****************************************************************************/
int8_t read_id_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_ID_REG), data);
}

/******************************************************************//**********
 * @brief read_sensor_lux()
 * This function reads the sensor lux,
 * @data: pointer to strore the read data
 *****************************************************************************/
int8_t read_sensor_lux(float * data)
{
	int8_t ret;
	float s_lux;
	uint8_t arr[2];
	uint16_t ch0, ch1;

	ret = i2c_read_bytes(APDS_SENS_DEV_ADDR, 
    			(APDS_CMD_BYTE_REG | APDS_D0LOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

	ch0 = ((uint16_t)arr[1] << 8 | arr[0]);

	printf("ch0 : %0.4x\n", ch0);

	ret = i2c_read_bytes(APDS_SENS_DEV_ADDR, 
    			(APDS_CMD_BYTE_REG | APDS_D1LOW_REG), arr, 2);
	if(ret == EXIT_FAILURE)
    	return EXIT_FAILURE;

	ch1 = ((uint16_t)arr[1] << 8 | arr[0]);

	printf("ch1 : %0.4x\n", ch1);

	if(((float)ch1/ch0) <= 0.5 &&  ((float)ch1/ch0) > 0)
   		s_lux = (0.0304 * ch0) - (0.062 * ch0 * pow(((float)ch1/ch0), 1.4));

   	else if(((float)ch1/ch0) <= 0.61 &&  ((float)ch1/ch0) > 0.5)
   		s_lux = (0.0224 * ch0) - (0.031 * ch1);

	else if(((float)ch1/ch0) <= 0.80 &&  ((float)ch1/ch0) > 0.61)
   		s_lux = (0.0128 * ch0) - (0.0153 * ch1);

	else if(((float)ch1/ch0) <= 1.3 &&  ((float)ch1/ch0) > 0.80)
   		s_lux = (0.00146 * ch0) - (0.00112 * ch1);
   	else
   		s_lux = 0;

   	*data = s_lux;

	return ret;
}

/******************************************************************//**********
 * @brief light_task_init()
 * This function creates shared memory to share the task status.
 * Shared Memory: To share light task's status(DEAD/ALIVE) with main_task.
 *****************************************************************************/
int light_task_init(void) 
{
  logTask_Msg_t logData;
  int ret;
  Task_Status_t light_status;

  /************Create Shared Memory to share task status with main ******/  
  lightTask_sm_fd = shm_open(LIGHTTASK_SM_NAME, O_CREAT | O_RDWR, 0666);
  if(lightTask_sm_fd == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LIGHT_TASK_ID,"SHARED MEMORY NOT CREATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LIGHT_TASK_ID,"SHARED MEMORY CREATED");

  /* truncate shared memory with required size */
  ret = ftruncate(lightTask_sm_fd, SM_SIZE);
  if(ret == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LIGHT_TASK_ID,"SHARED MEMORY NOT TRUNCATED");
    return ERROR;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LIGHT_TASK_ID,"SHARED MEMORY TRUNCATED");

  /*map the shared memory */
  lightTask_sh_mem = mmap(NULL, SM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , lightTask_sm_fd, 0);
  if(lightTask_sh_mem == ERROR)
  {
    LOG_TO_QUEUE(logData,LOG_ERR,LIGHT_TASK_ID,"SHARED MEMORY NOT MAPPED");
    return -1;
  }
  LOG_TO_QUEUE(logData,LOG_INFO,LIGHT_TASK_ID,"SHARED MEMORY MAPPED");

  /**** Enter the task status into shared memory *****/
  light_status=ALIVE;
  /* Copy the contents of payload into the shared memory */
  memcpy((char*)lightTask_sh_mem, (char*)&light_status, SM_SIZE);

  LOG_STD("[INFO] LIGHT TASK INITIALIZED SHARED MEMORY\n");

  return SUCCESS;
}

/******************************************************************//****
 * @brief light_timer_handler()
 * This is a timer_handler occurs every 3s and updates the light parameters
 * @signal: occured signal number
 * @return: None
 ***********************************************************************/
void light_timer_handler(int signal)
{
  logTask_Msg_t logData;
  float lux;

  /***** Read the sensor and display****/
  LOG_STD("[INFO] READING LIGHT SENSOR\n" );
  read_sensor_lux(&lux);
  LOG_STD("[INFO] SENSOR LUX: %f\n", lux );
}

/******************************************************************//**********
 * @brief light_task_thread()
 * This thread initializes the light task resources such as shared memory
 *****************************************************************************/
void light_task_thread(void) 
{
	logTask_Msg_t logData;
	Task_Status_t light_status;
	struct itimerval timer;
    struct sigaction timer_sig;
    int ret;
	
	/* wait light task so that other tasks(logger task queue) are synchronized with it*/
	pthread_barrier_wait(&tasks_barrier);
	
	LOG_STD("[INFO] LIGHT TASK STARTED\n");
	LOG_TO_QUEUE(logData,LOG_INFO, LIGHT_TASK_ID,"LIGHT TASK STARTED");	
	ret= light_task_init();
	if(ERROR == ret)
	{
		LOG_STD("[ERROR] LIGHT TASK INIT: %s\n", strerror(errno));
		exit(ERROR);
	}
	LOG_TO_QUEUE(logData,LOG_INFO, LIGHT_TASK_ID,"LIGHT TASK INITIALIZED");
	
	/* Turning on Light sensor*/
	write_control_reg(POWER_ON);
	LOG_TO_QUEUE(logData,LOG_INFO, LIGHT_TASK_ID,"TURNED ON LIGHT SENSOR");

    /************** POSIX Timer setup *******/
    memset(&timer_sig, 0, sizeof(timer_sig));
    timer_sig.sa_handler= &light_timer_handler;
    if(sigaction( SIGVTALRM, &timer_sig, NULL)<0 )
    {
        LOG_TO_QUEUE(logData,LOG_ERR, LIGHT_TASK_ID,"POSIX TIMER CAN'T BE LINKED");
    }

    LOG_TO_QUEUE(logData,LOG_INFO,LIGHT_TASK_ID,"POSIX TIMER LINKED");

    /* set up timer to expire every 100ms */
    timer.it_value.tv_sec= 1;
    timer.it_value.tv_usec=0;  
    timer.it_interval.tv_sec=1;
    timer.it_interval.tv_usec=0; 
    setitimer( ITIMER_VIRTUAL, &timer, NULL);

    LOG_TO_QUEUE(logData,LOG_INFO,LIGHT_TASK_ID,"POSIX TIMER SETUP DONE");
    LOG_STD("[INFO] POSIX TIMER SETUP DONE\n");

    while(!lightTask_kill);

    LOG_STD("[INFO] USR1:LIGHT THREAD KILL SIGNAL RECEIVED\n");
    
	/*********** KILL Signal Received ***********/
	LOG_STD("[INFO] LIGHT TASK KILL SIGNAL RECEIVED\n");
	/* Update task status in shared memory */
	light_status=DEAD;
	/* Copy the contents of payload into the share memory */
	memcpy((char*)lightTask_sh_mem, (char*)&light_status, SM_SIZE);

	pthread_exit(NULL);
}
