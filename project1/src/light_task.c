/* Light task */

#include "light_task.h"

/* logger shared memory*/
void *lightTask_sh_mem;
int lightTask_sm_fd;
/* logger task kill flag*/
extern int lightTask_kill;
/* task barrier to synchronize tasks*/
extern pthread_barrier_t tasks_barrier;


int8_t write_control_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}


int8_t read_control_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_CONTROL_REG), data);
}

int8_t write_timing_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

int8_t read_timing_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_TIMING_REG), data);
}

int8_t write_intthresh_low_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHLOW_REG), data);
}

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

int8_t write_intthresh_high_reg(const uint16_t data)
{
	return i2c_write_word(APDS_SENS_DEV_ADDR,
			(APDS_CMD_WORD_REG | APDS_INTTHHIGH_REG), data);
}

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

int8_t write_intcontrol_reg(const uint8_t data)
{
	return i2c_write_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

int8_t read_intcontrol_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_INTCTRL_REG), data);
}

int8_t read_id_reg(uint8_t * data)
{
	return i2c_read_byte(APDS_SENS_DEV_ADDR,
			(APDS_CMD_BYTE_REG | APDS_ID_REG), data);
}

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
 * @brief logger_task_init()
 * This function creates below,
 * Message Queue: To log messages from all the running tasks.
 * Shared Memory: To share logger task's status(DEAD/ALIVE) with main_task.
 * LogFile      : To store messages received from its message queue.
 *****************************************************************************/
int light_task_init(void) 
{
  logTask_Msg_t logData;
  int ret;
  Task_Status_t light_status;

  /************Create Shared Memory to share task status with main ******/  
  lightTask_sm_fd = shm_open(SM_NAME, O_CREAT | O_RDWR, 0666);
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

  return logTask_mq_d;
}
/******************************************************************//****
 * @brief timer_handler()
 * This is a timer_handler occurs every 3s and gets the status of all 
 * running threads(DEAD/ALIVE) by reading each tasks shared memory. 
 * And logs these statusses into logger message queue
 * @signal: occured signal number
 * @return: None
 ***********************************************************************/
void timer_handler(int signal)
{
  logTask_Msg_t logData;
  Task_Status_t logger_status;

  /* read status from logger task shared memory */
  memcpy((char*)&logger_status,(char*)logTask_sh_mem,SM_SIZE);

  /*** similarly read other task statusses from their shared memory ****/

  LOG_STD("[INFO] READING TASK STATUS\n" );
  printf("Logger Status:%s\n",task_statusString[logger_status]);
  if( logger_status != DEAD )
  {
    LOG_TO_QUEUE(logData,LOG_STATUS,MAIN_TASK_ID,"LOGGER TASK STATUS: [%s]\n", \
                                            task_statusString[logger_status]);
  }
  else //logger DEAD so no logging to logger queue instead to standard output
  {
    LOG_STD("[INFO] LOGGER TASK STATUS: [%s]\n",task_statusString[logger_status] );
  }
}

/******************************************************************//**********
 * @brief light_task_thread()
 * This thread initializes the light task resources such as shared memory
 *****************************************************************************/
void light_task_thread(void) 
{
  logTask_Msg_t logData;
  Task_Status_t light_status;
  int ret;

  LOG_STD("[INFO] LIGHT TASK STARTED\n");
  ret= light_task_init();
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LIGHT TASK INIT: %s\n", strerror(errno));
    exit(ERROR);
  }

  /* wait logger task so that other tasks are synchronized with it*/
  pthread_barrier_wait(&tasks_barrier);

  /*********** KILL Signal Received ***********/
  LOG_STD("[INFO] LIGHT TASK KILL SIGNAL RECEIVED\n");
  /* Update task status in shared memory */
  light_status=DEAD;
  /* Copy the contents of payload into the share memory */
  memcpy((char*)lightTask_sh_mem, (char*)&light_status, SM_SIZE);

  pthread_exit(NULL);
}

void * light_thread(void * data)
{
    
    struct itimerval timer;
    struct sigaction timer_sig;
    logTask_Msg_t logData;
    int ret;

    pthread_barrier_wait(&tasks_barrier);

    /************** Signal Handler Linking to POSIX timer *******/
    memset(&timer_sig, 0, sizeof(timer_sig));
    timer_sig.sa_handler= &timer_handler;
    if(sigaction( SIGVTALRM, &timer_sig, NULL)<0 )
    {
        LOG_TO_QUEUE(logData,LOG_ERR, LIGHT_TASK_ID,"POSIX TIMER CAN'T BE LINKED");
        return 1;
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

    while(!status_thread_kill);

    LOG_STD("[INFO] USR2: LIGHT THREAD KILL SIGNAL RECEIVED\n");

  /***** Unlink Shared Memories of all tasks ******/
  ret= close(logTask_sm_fd);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LOGTASK SHARED MEMORY CLOSE FAILED:%s\n", strerror(errno));
  }
  LOG_STD("[INFO] LOGTASK SHARED MEMORY CLOSED\n");

  ret=shm_unlink(SM_NAME);
  if(ERROR == ret)
  {
    LOG_STD("[ERROR] LOGTASK SHARED MEMORY UNLINK FAILED:%s\n", strerror(errno));
  }
  LOG_STD("[INFO] LOGTASK SHARED MEMORY UNLINKED\n");


  /***** similarly unlink each task shared memories ******/
  pthread_exit(NULL);



    /* Test code */

    int8_t ret;
    uint8_t temp;
    float lux;
    uint16_t thr;

    write_control_reg(POWER_ON);

    read_control_reg(&temp);
   
    printf("Control reg: 0x%0.2x\n", temp);

    read_id_reg(&temp);

    printf("ID reg: %0.2x\n", temp);

    write_intthresh_low_reg(0x0101);

    read_intthresh_low_reg(&thr);
    printf("Threshold: 0x%0.4x\n", thr);

    

    while(1)
    {
        read_sensor_lux(&lux);

        printf("Sensor LUX: %f\n", lux);
        sleep(1);
        //break;
    }
        
    
    pthread_exit(NULL);
}


