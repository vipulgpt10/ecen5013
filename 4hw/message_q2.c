/* Message Queue process 1
 * Passing the structure pointer in the another process's queue
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define PROC1_QUEUE_NAME   "/mq-exproc1"
#define PROC2_QUEUE_NAME   "/mq-exproc2"

#define MAX_MESSAGES 10

struct my_message
{
	char buff[256];
	uint8_t length;
	bool led_state;
}msg;

int main()
{
	char tmp_buff[256];
	
	mqd_t qd_proc1, qd_proc2;

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = sizeof(struct my_message);
    attr.mq_curmsgs = 0;

    size_t rec_bufflen = sizeof(struct my_message) + 8;

    if((qd_proc1 = mq_open(PROC1_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &attr)) == -1)
    {
    	perror ("mq_open");
        exit (1);
    }
   
    msg.led_state = false;

    while(1)
    {
  //   	printf("Please type a message to send\n");

		// fgets(tmp_buff, 255, stdin);

		// int len = strlen(tmp_buff);
		// msg.length = len;

		// if(tmp_buff[len - 1] == '\n')
		// 	tmp_buff[len - 1] = '\0';
        
  //       strcpy(msg.buff, tmp_buff);

  //       if((qd_proc2 = mq_open(PROC2_QUEUE_NAME, O_WRONLY)) == -1)
  //   	{
	 //    	perror ("mq_open");
	 //        exit(1);
  //  		}

  //       if((mq_send(qd_proc2, (char *)&msg, sizeof(struct my_message), 0)) == -1)
  //       {
  //       	perror("Unable to send message to proc 2\n");
  //       	exit(1);
  //       }

        sleep(1);
       
        strcpy(msg.buff, " ");

        printf("Waiting for message\n");

        if((mq_receive(qd_proc1, (char *)&msg, rec_bufflen, NULL)) == -1)
        {
        	perror("mq_receive\n");
        	exit(1);
        }


        printf("Message from process 2:\n");
		printf("%s\n", msg.buff);

		printf("LED State: %s\n", (msg.led_state ? "ON" : "OFF"));

		printf("Changing LED State to OFF\n");
		printf("-------------------\n");

		msg.led_state = false;

    }

    if((mq_close(qd_proc1)) == -1)
    {
    	perror("mq_close");
    	exit(1);
    }


    return 0;
}

