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


struct my_message1
{
	char buff[256];
	uint8_t length;
	bool led_state;
}msg1,msg2;


int main()
{
	char tmp_buff1[100];
	
	mqd_t qd_proc1, qd_proc2;

	struct mq_attr attr1;
	attr1.mq_flags = 0;
	attr1.mq_maxmsg = MAX_MESSAGES;
	attr1.mq_msgsize = sizeof(struct my_message1);
    attr1.mq_curmsgs = 0;

    size_t rec_bufflen = sizeof(struct my_message1) + 8;

    if((qd_proc2 = mq_open(PROC2_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &attr1)) == -1)
    {
    	perror("mq_open");
        exit (1);
    }


    msg2.led_state = false;

    while(1)
    {
    	
  //   	printf("Waiting for message\n");

  //       if((mq_receive(qd_proc2, (char *)&msg1, rec_bufflen, NULL)) == -1)
  //       {
  //       	perror("mq_receive\n");
  //       	exit(1);
  //       }

  //       printf("Message from process 1:\n");
		// printf("%s\n", msg1.buff);

		// printf("LED State: %s\n", (msg1.led_state ? "ON" : "OFF"));

		// printf("Changing LED State to ON\n");
		// printf("-------------------\n");

		// msg2.led_state = true;
        printf("%d\n", (int)sizeof(struct my_message1));
        
    	printf("Please type a message to send\n");

        fgets(tmp_buff1, 255, stdin);

        int len = strlen(tmp_buff1);
        msg1.length = len;

        if(tmp_buff1[len - 1] == '\n')
            tmp_buff1[len - 1] = '\0';
        
        strcpy(msg1.buff, tmp_buff1);

        printf("%d\n", (int)sizeof(tmp_buff1));

        if((qd_proc1 = mq_open(PROC1_QUEUE_NAME, O_WRONLY)) == -1)
        {
            perror ("mq_open");
            exit(1);
        }

        printf("%d\n", (int)sizeof(struct my_message1));
        if((mq_send(qd_proc1, (char *)&msg1, sizeof(struct my_message1), 0)) == -1)
        {
            perror("Unable to send message to proc 1\n");
            exit(1);
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

