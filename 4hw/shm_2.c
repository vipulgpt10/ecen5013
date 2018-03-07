/* Shared memory process 2 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>			// ftruncate
#include <sys/types.h>		// ftruncate
#include <sys/mman.h>		// shm_open
#include <sys/stat.h>		// constants
#include <fcntl.h>			// shm_open
#include <string.h>
	
#include <semaphore.h>


#define SHARED_MEM_NAME "/posix-shm"

#define SEM_MUTEX_NAME_1 "/sem1-mutex"
#define SEM_MUTEX_NAME_2 "/sem2-mutex"

struct shared_mem
{
	char buff[256];
	uint8_t length;
	bool led_state;
};

int main()
{
	char tmp_buff[256];
	int shm_fd;
	struct shared_mem * shm_mem_ptr;
	sem_t * mutex_sem1, * mutex_sem2;

	if ((mutex_sem1 = sem_open(SEM_MUTEX_NAME_1, 0, 0, 0)) == SEM_FAILED)
		exit(1);

	if ((mutex_sem2 = sem_open(SEM_MUTEX_NAME_2, 0, 0, 0)) == SEM_FAILED)
		exit(1);

	if ((shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR , 0660)) == -1)
	{
		perror("shm_open");
		exit(1);
	}

	if (ftruncate (shm_fd, sizeof(struct shared_mem)) == -1)
	{
		perror("ftruncate");
		exit(1);
	}

	if ((shm_mem_ptr = mmap(NULL, sizeof(struct shared_mem), 
				PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}

	shm_mem_ptr->led_state = false;

	while(1)
	{
		printf("Waiting for message\n");
		sem_wait(mutex_sem1);

		printf("Message from process 1:\n");
		printf("%s\n", shm_mem_ptr->buff);

		printf("LED State: %s\n", (shm_mem_ptr->led_state ? "ON" : "OFF"));

		printf("Changing LED State to ON\n");
		printf("-------------------\n");

		shm_mem_ptr->led_state = true;

		printf("Please type a message to send\n");

		fgets(tmp_buff, 100, stdin);

		int len = strlen(tmp_buff);
		shm_mem_ptr->length = len;

		if(tmp_buff[len - 1] == '\n')
			tmp_buff[len - 1] = '\0';
        
        strcpy(shm_mem_ptr->buff, tmp_buff);

        sem_post(mutex_sem2);

	}

	if (munmap(shm_mem_ptr, sizeof(struct shared_mem)) == -1)
    	exit(1);
    
    return 0;

}
