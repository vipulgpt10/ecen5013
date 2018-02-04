/*
 * Test the mysort syscall (#333)
 */
#define _GNU_SOURCE

#include <linux/kernel.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


#define SYS_MYSORT 333

int main()
{
    uint16_t i;
//    uint8_t * buff = (uint8_t *)malloc(256 * sizeof(uint8_t));
    uint8_t * sort_buff = (uint8_t *)malloc(256 * sizeof(uint8_t));
 
	uint8_t * buff = NULL; 

    printf("Randomly generated string to sort:\n\n");

    srand(time(0));

    // for(i = 0; i < 256; i++)
    // {
    //     *(buff + i) = (rand() % 96) + 32;
    //     printf("%c", *(buff + i));

    // }
    // printf("\n\nMaking system call \n\n");

    scanf("%[^\n]s", buff);

    /* System call handler */
    long res = syscall(SYS_MYSORT, buff, strlen(buff), sort_buff); 
    printf("System call returned %ld.\n", res);

    printf("\nSorted string:\n\n");
    printf("%s\n", sort_buff);

    return 0;
}
