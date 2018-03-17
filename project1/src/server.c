

#include "server.h"
#include "light_task.h"
#include "temp_task.h"


void * server(void * data)
{
    struct sockaddr_in addr, peer_addr;
    int addr_len = sizeof(peer_addr);
    int i =0;
    size_t len, read_b;

    char read_buff[100];
    char write_buff[100];
    int server_socket, accepted_soc, opt = 1;

    while(1)
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
        
        

    printf("==== SERVER thread ++++\n");

    close(accepted_soc);

    close(server_socket);
    }

    pthread_exit(NULL);

}

