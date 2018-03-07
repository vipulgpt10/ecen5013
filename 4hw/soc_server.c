#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>


#define PORT_ADR    2000

typedef struct
{
  char    buf[20];
  size_t  buf_len;
  bool    usrLED_OnOff;
}payload_t;


int main()
{
  struct sockaddr_in addr, peer_addr;
  int addr_len = sizeof(peer_addr);
  char rdbuff[1024] = {0};
  int server_socket, accepted_soc, opt = 1;
  int i = 0;
  payload_t *ploadptr;
  int read_b;
  size_t pload_len = 0;

  /* create socket */
  if((server_socket = socket(AF_INET,SOCK_STREAM,0)) == 0)
  {
    printf("[Server] [ERROR] Socket Creation Error\n");
    return 1;
  }
  else
    printf("[Server] Socket Created Successfully\n");

  /* set socket options */
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(opt), sizeof(opt)))
  {
    printf("[Server] [ERROR] Socket options set error\n");
    return 1;
  }

  /*Set the sockaddr_in structure */
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;	
  addr.sin_port = htons(PORT_ADR);

  /*bind socket to a address */
  if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
  {
    printf("[Server] [ERROR] Bind socket Error\n");
    return 1;
  }
  else
    printf("[Server] Socket binded Successfully\n");

  /* listen for connections*/
  if(listen(server_socket,5) < 0)
  {
    printf("[Server] [ERROR] Can't listen connection\n");
    return 1;
  }

  /*accept connection */
  accepted_soc = accept(server_socket, (struct sockaddr*)&peer_addr,(socklen_t*)&addr_len);
  if(accepted_soc < 0)
  {
    printf("[Server] [ERROR] Can't accept connection\n");
    return 1;
  }

  /* read payload length */
  read_b = read(accepted_soc, &pload_len, sizeof(size_t));
  if(read_b == sizeof(size_t))
  {
    printf("[Server] Size of incoming payload: %d\n",pload_len);
  }	
  else
  {
    printf("[Server] [ERROR] Invalid data\n");
    return 1;
  } 

  /* read payload */
  while((read_b = read(accepted_soc, rdbuff+i, 1024)) < pload_len)
  {
    i+=read_b;	
  }
  ploadptr= (payload_t*)rdbuff;
  /* display data */
  printf("[Server] Message Recvd from Client\n{\n Message:%s\n MessageLen:%d\n USRLED:%d\n}\n", \ 
                                        ploadptr->buf, ploadptr->buf_len, ploadptr->usrLED_OnOff);
  
  /* send message from server to client */
  send(accepted_soc , "ACK" , 4, 0);
  printf("[Server] Message sent from Server: ACK\n");

  /*close socket */
  close(accepted_soc);

  return 0;
}