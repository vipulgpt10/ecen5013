#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
  char    buf[20];
  size_t  buf_len;
  bool    usrLED_OnOff;
}payload_t;


int main()
{
  int P_C[2]; /* pipe descriptor from parent to child */
  int C_P[2]; /* pipe descriptor from child to parent */
  /* child parameters */
  const char* msg_c = "Message from Child";
  payload_t payloadSend_c = {0}, *payloadptr_c;
  char readbuf_c[sizeof(payload_t)] = {0};
  /* parent parameters */
  const char* msg_p  = "Message from Parent";
  payload_t payloadSend_p = {0}, *payloadptr_p;
  char readbuf_p[sizeof(payload_t)] = {0};

  /*** pipe Creation *****/
  printf("Creating Pipes ..\n");
  /* 1st pipe creation */
  if (0==pipe(P_C))
  {
    printf("Pipe Parent->Child creation Successful\n");   
  }
  else
  {
    printf("Pipe Parent->Child creation Successful\n");
    return -1;
  }
  /* 2nd pipe creation*/
  if (0==pipe(C_P))
  {
    printf("Pipe C_P creation Successful\n");   
  }
  else
  {
    printf("Pipe C_P creation Successful\n");
    return -1;
  }

  /* fork() returns 0:child, -1:error, pid:parent(>0) */
  switch( fork() ){
    case -1:
        printf("Child process creation error\n");
        break;

    case 0: /*child process*/
        close(P_C[1]); /*close parent to child write*/
        /* read message from pipe */
        read(P_C[0], readbuf_c, sizeof(payload_t));
        payloadptr_c = (payload_t*)readbuf_c;
        printf("[CHILD] [INFO] Message Recvd\n{\n  Message: %s\n  MessageLen: %d\n  USRLED: %d\n}\n", \
                    payloadptr_c->buf,payloadptr_c->buf_len,payloadptr_c->usrLED_OnOff);
        close(P_C[0]);/* close parent to child read */

        close(C_P[0]); /* close child to parent read */
        /* Enter payload: message, len, usrled*/
        memcpy(payloadSend_c.buf,msg_c,strlen(msg_c)+1);
        payloadSend_c.buf_len = strlen(payloadSend_c.buf);
        payloadSend_c.usrLED_OnOff = 0;
        /* write payload structure to pipe */
        write(C_P[1], (char*)&payloadSend_c, sizeof(payloadSend_c));
        printf("[CHILD] [INFO] Message sent to parent from child\n");
        close(C_P[1]); /* close child to parent read */

        break;

    default: /*parent*/
        /* Enter payload: message, len, usrled*/
        memcpy(payloadSend_p.buf,msg_p,strlen(msg_p)+1);
        payloadSend_p.buf_len = strlen(payloadSend_p.buf);
        payloadSend_p.usrLED_OnOff = 1;
 
        close(P_C[0]);  
        /* write payload structure to pipe*/
        write(P_C[1], (char*)&payloadSend_p, sizeof(payloadSend_p));
        printf("[PARENT] [INFO] Message sent to child from parent\n");
        close(P_C[1]);
 
        /* Wait for child to send a message */
        wait(NULL);
 
        close(C_P[1]); 
        /* read message from pipe*/
        read(C_P[0], readbuf_p, sizeof(payload_t));
        payloadptr_p = (payload_t*)readbuf_p;
        printf("[PARENT] [INFO] Message Recvd\n{\n  Message: %s\n  MessageLen: %d\n  USRLED: %d\n}\n", \
                     payloadptr_p->buf,payloadptr_p->buf_len,payloadptr_p->usrLED_OnOff);
        close(C_P[0]);
        exit(0);

        break;
   }
}