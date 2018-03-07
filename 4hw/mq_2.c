/* Message Queue process 2
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
};

int main()
{
	char tmp_buff[256];
	char msg1[128];
	struct my_message * msg1_ptr;
	struct my_message * tmp_ptr;

	mqd_t qd_proc1, qd_proc2;

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = sizeof(void *);
    attr.mq_curmsgs = 0;

    if((qd_proc2 = mq_open(PROC2_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &attr)) == -1)
    {
    	perror("mq_open");
        exit (1);
    }

    
    if((qd_proc1 = mq_open(PROC1_QUEUE_NAME, O_WRONLY)) == -1)
    {
    	perror("mq_open");
        exit (1);
    }

    msg1_ptr = (struct my_message *)malloc(sizeof(struct my_message));

    msg1_ptr->led_state = false;

    while(1)
    {
    	
    	printf("Waiting for message\n");

        if((mq_receive(qd_proc2, msg1, 32, NULL)) == -1)
        {
        	perror("mq_receive\n");
        	exit(1);
        }

        memcpy(&tmp_ptr, msg1, sizeof(void *));

        printf("Message from process 1:\n");
		printf("%s\n", tmp_ptr->buff);

		printf("LED State: %s\n", (tmp_ptr->led_state ? "ON" : "OFF"));

		printf("Changing LED State to OFF\n");
		printf("-------------------\n");

		tmp_ptr->led_state = true;

	label:
    	printf("Please type a message to send\n");

		fgets(tmp_buff, 100, stdin);

		int len = strlen(tmp_buff);
		msg1_ptr->length = len;

		if(tmp_buff[len - 1] == '\n')
			tmp_buff[len - 1] = '\0';
        
        strcpy(msg1_ptr->buff, tmp_buff);

        memcpy(msg1, &msg1_ptr, sizeof(void *));

        if((mq_send(qd_proc1, msg1, sizeof(void *), 0)) == -1)
        {
        	perror("Unable to send message to proc 2\n");
        	goto label;
        }

        sleep(1); 

    }

    if((mq_close(qd_proc2)) == -1)
    {
    	perror("mq_close");
    	exit(1);
    }

    return 0;

}

