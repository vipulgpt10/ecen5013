#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>


#define PORT_ADR    2000
#define IP_ADR      "127.0.0.1" /* Loppback IP Address*/

int main()
{
	int client_socket = 0;
	struct sockaddr_in serv_addr = {0};
	size_t len, sent_b;
	
	char in_buff[100];
	char out_buff[100];

	

	while(1)
	{
	 	
		/* create socket */
		if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("[Client] [ERROR] Socket creation Error\n");
			return -1;
		}
		else
			// printf("[Client] Socket Created Successfully\n");

		/* Fill the socket address structure */
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT_ADR);
		  
		/* convert the IP ADDR to proper format */
		if(inet_pton(AF_INET, IP_ADR, &serv_addr.sin_addr)<=0) 
		{
			printf("[Client] [ERROR] Address Conversion Error\n");
			return -1;	
		}

		printf("List of APIs available:\n");
		printf(" get_temp_cel\t -Temperature in Celsius\n");
		printf(" get_temp_fah\t -Temperature in Fahrenheit\n");
		printf(" get_temp_kel\t -Temperature in Kelvin\n");
		printf(" get_sensor_lux\t -Light Sensor Luminosity\n");
		printf(" is_it_day?\n");
		printf(" is_it_night?\n");
		printf(" LED_ON\n");
		printf(" LED_OFF\n");
		printf(" \n");

		printf("[Client] Enter the API: ");
	 	fgets(in_buff, 100, stdin);

	 	len = strlen (in_buff);
		if (in_buff[len - 1] == '\n')
			in_buff[len - 1] = '\0';

		/* connect the socket before sending the data */
  		if(connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  		{
    		printf("[Client] [ERROR] Connection Failed \n");
    		return -1;
  		}

  		/* Size of the payload */
  		send(client_socket, &len ,sizeof(size_t), 0);
  		/* Send actual message request (API) */
  		sent_b = send(client_socket, (void *)&in_buff, sizeof(in_buff), 0);

  		if(sent_b < sizeof(in_buff))
  		{
    		printf("[Client] [ERROR] Complete data not sent\n");
    		return -1;
 		}

 		printf("[Client] Message sent to Server: %s\n", in_buff);

 		read(client_socket, out_buff, 100);

 		printf("[Client] Message received: %s\n\n", out_buff);

 		close(client_socket);

 		memset(out_buff, 0, sizeof(out_buff));

 		sleep(1);
        
	}

	
  
  	return 0;
 
 }