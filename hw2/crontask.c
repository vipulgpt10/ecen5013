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
    /* Current process id and user id */
    printf("Process id: %d\n", (int)getpid());
    printf("User id :   %d\n", (int)getuid());

    /* Date and time */
    time_t t = time(0);
    struct tm *tm = localtime(&t);
    char s[64];
    strftime(s, sizeof(s), "%c", tm);
    printf("%s\n\n", s);

    /* My sort syscall */
    uint16_t i;
    uint8_t * buff = (uint8_t *)malloc(256 * sizeof(uint8_t));
    uint8_t * sort_buff = (uint8_t *)malloc(256 * sizeof(uint8_t));

    printf("Randomly generated string to sort:\n\n");

    srand(time(0));

    for(i = 0; i < 256; i++)
    {
        *(buff + i) = (rand() % 96) + 32;
        printf("%c", *(buff + i));

    }
    printf("\n\nMaking system call \n\n");

    /* System call handler */
    long res = syscall(SYS_MYSORT, buff, strlen(buff), sort_buff); 
    printf("System call returned %ld.\n", res);

    printf("\nSorted string:\n\n");
    printf("%s\n", sort_buff);

    return 0;
}
