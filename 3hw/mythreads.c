
#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <zconf.h>
#include <signal.h>
#include <semaphore.h>
#include <memory.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>


clock_t t1;
clock_t t2;
clock_t delay;
static uint32_t th2_count;

pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;



long length;
timer_t timer_id;
struct itimerspec itimerspec1;
struct sigevent se;

struct th_info
{
	pthread_t tid;
	char * fname;
}* tinfo1, * tinfo2;

struct node
{
	char ch;
	uint32_t count;
	struct node * next;
};

struct periodic_info {
    int sig;
    sigset_t alarm_sig;
};




static void * child1(void * tinfo);

static void * child2(void * tinfo);

void insert_beg(struct node ** head_ref, char ch_data, uint32_t cnt_data);

uint8_t isthere(struct node * head, char ch_data);

void inc_count(struct node * head, char ch_data);

void traverse_3(struct node * head, FILE * fptr);

void delete_list(struct node ** head);

static int periodic(int unsigned period, struct periodic_info *info);

static void wait_period(struct periodic_info *info);

void signal_handler(int arg);

int main(int argc, char * argv[])
{

	/* Date and time */
	time_t mytime;
	struct tm * tm_info;
	char time_buff[30];

    time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    sigset_t alarm_sig;
    sigemptyset(&alarm_sig);
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
        sigaddset(&alarm_sig, i);
    sigprocmask(SIG_BLOCK, &alarm_sig, NULL);

	if(argc == 1)
	{
		printf("Please enter the logfile name while executing\n");
		exit(1);
	}

	FILE * fptr = fopen(argv[1], "w+");

	fprintf(fptr, "Master:\t\tEntering master thread\n");
	fprintf(fptr, "Master:\t\tStart time : %s\n", time_buff);
	fprintf(fptr, "Master:\t\tPID: %d\n", getpid());
	fprintf(fptr, "Master:\t\tLinux thread id: %ld\n", syscall(SYS_gettid));
	fclose(fptr);

	printf("Master:\t\tEntering master thread\n");
	printf("Master:\t\tStart time : %s\n", time_buff);
	printf("Master:\t\tPID: %d\n", getpid());
	printf("Master:\t\tLinux thread id: %ld\n", syscall(SYS_gettid));


	/* Create threads */
	tinfo1 = (struct th_info *)malloc(sizeof(struct th_info));
	tinfo2 = (struct th_info *)malloc(sizeof(struct th_info));

	tinfo1->fname = argv[1];
	tinfo2->fname = argv[1];

	float th_time1 = clock();

	if(pthread_create(&tinfo1->tid, NULL, child1, (void *)tinfo1))
	{
		printf("Master:\t\tThread 1 creation failed\n");
		exit(1);
	}
	th_time1 = clock() - th_time1;

	float th_time2 = clock();

	if(pthread_create(&tinfo2->tid, NULL, child2, (void *)tinfo2))
	{
		printf("Master:\t\tThread 2 creation failed\n");
		exit(1);
	}
	th_time2 = clock() - th_time2;

	sleep(3);

	pthread_join(tinfo1->tid, NULL);
	pthread_join(tinfo2->tid, NULL);

	double tot_time1 = ((double)th_time1)/CLOCKS_PER_SEC;
    double tot_time2 = ((double)th_time2)/CLOCKS_PER_SEC;

    fptr = fopen(argv[1], "a+");

    fprintf(fptr, "Master:\t\tTime by child 1: %f secs\n", tot_time1);
    fprintf(fptr, "Master:\t\tTime by child 2: %f secs\n", tot_time2);

    time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	fprintf(fptr, "Master:\t\tLeaving master thread\n");
	fprintf(fptr, "Master:\t\tExit time : %s\n", time_buff);

	fclose(fptr);

	printf("Master:\t\tTime by child 1: %f secs\n", tot_time1);
	printf("Master:\t\tTime by child 2: %f secs\n", tot_time2);
	printf("Master:\t\tLeaving master thread\n");
	printf("Master:\t\tExit time : %s\n", time_buff);

	return 0;

}


static void * child1(void * tinfo)
{

	/* Date and time */
	time_t mytime;
	struct tm * tm_info;
	char time_buff[30];
	time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	pthread_mutex_lock(&my_mutex);

	struct th_info * thinfo = (struct th_info *)tinfo;
	FILE * fptr1 = fopen(thinfo->fname, "a+");

	fprintf(fptr1, "Child 1:\tEntering child 1 thread\n");
	fprintf(fptr1, "Child 1:\tStart time : %s\n", time_buff);
	fprintf(fptr1, "Child 1:\tPID: %d\n", getpid());
	fprintf(fptr1, "Child 1:\tLinux thread id: %ld\n", syscall(SYS_gettid));
	fprintf(fptr1, "Child 1:\tPOSIX thread id: %ld\n", (long)thinfo->tid);

	printf("Child 1:\tEntering child 1 thread\n");
	printf("Child 1:\tStart time : %s\n", time_buff);
	printf("Child 1:\tPID: %d\n", getpid());
	printf("Child 1:\tLinux thread id: %ld\n", syscall(SYS_gettid));
	printf("Child 1:\tPOSIX thread id: %ld\n", (long)thinfo->tid);

	/* Char - Linked list code */
	char ch;
	size_t i;
	struct node * head = NULL;
	FILE * fptr3 = fopen("Valentinesday.txt", "r+");

	assert(fptr3);
	fseek(fptr3, 0, SEEK_END);
	size_t length = ftell(fptr3);
	fseek(fptr3, 0, SEEK_SET);
	char * ch_buff = (char *)malloc(__size_t (length + 1));
	ch_buff[length] = '\0';
	fread(ch_buff, 1, __size_t length, fptr3);
	fclose(fptr3);

	for(i = 0; i < length; i++)
	{
		ch = *(ch_buff + i);
		if(isalpha(ch))
		{
			ch = tolower(ch);
			if(isthere(head, ch))
				inc_count(head, ch);
			else
				insert_beg(&head, ch, 1);

		}

	}
	traverse_3(head, fptr1);
	delete_list(&head);

	time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	fprintf(fptr1, "Child 1:\tLeaving child 1 thread\n");
	fprintf(fptr1, "Child 1:\tExit time : %s\n", time_buff);

	fclose(fptr1);

	printf("Child 1:\tLeaving child 1 thread\n");
	printf("Child 1:\tExit time : %s\n", time_buff);

	pthread_mutex_unlock(&my_mutex);

	pthread_exit(NULL);
}

static void * child2(void * tinfo)
{

	/* Date and time */
	time_t mytime;
	struct tm * tm_info;
	char time_buff[30];
	time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	struct th_info * thinfo = (struct th_info *)tinfo;
	FILE * fptr1 = fopen(thinfo->fname, "a+");

	fprintf(fptr1, "Child 2:\tEntering child 2 thread\n");
	fprintf(fptr1, "Child 2:\tStart time : %s\n", time_buff);
	fprintf(fptr1, "Child 2:\tPID: %d\n", getpid());
	fprintf(fptr1, "Child 2:\tLinux thread id: %ld\n", syscall(SYS_gettid));
	fprintf(fptr1, "Child 2:\tPOSIX thread id: %ld\n", (long)thinfo->tid);

	printf("Child 2:\tEntering child 2 thread\n");
	printf("Child 2:\tStart time : %s\n", time_buff);
	printf("Child 2:\tPID: %d\n", getpid());
	printf("Child 2:\tLinux thread id: %ld\n", syscall(SYS_gettid));
	printf("Child 2:\tPOSIX thread id: %ld\n", (long)thinfo->tid);


	/* POSIX Timer */
	struct periodic_info info1;
    periodic(100000, &info1);

    while(1)
    {
        th2_count++;
        wait_period(&info1);
        
        /* Caculate CPU Utilization */
        long double arr1[4], arr2[4], avg_load;
        FILE * fptr5;

        fptr5 = fopen("/proc/stat","r");
        fscanf(fptr5, "%*s %Lf %Lf %Lf %Lf", &arr1[0], &arr1[1], &arr1[2], &arr1[3]);
        fclose(fptr5);
        sleep(1);

        fptr5 = fopen("/proc/stat","r");
        fscanf(fptr5, "%*s %Lf %Lf %Lf %Lf", &arr2[0], &arr2[1], &arr2[2], &arr2[3]);
        fclose(fptr5);

        avg_load = ((arr2[0]+arr2[1]+arr2[2])-(arr1[0]+arr1[1]+arr1[2]))/
        			((arr2[0]+arr2[1]+arr2[2]+arr2[3])-(arr1[0]+arr1[1]+arr1[2]+arr1[3]));
        fprintf(fptr1, "Child 2:\tCPU utilization is %Lf\n", avg_load);
        printf("Child 2:\tCPU utilization is %Lf\n",avg_load);
        fflush(fptr1);
    }
	
	time(&mytime);
    tm_info = localtime(&mytime);
	strftime(time_buff, 30, "%Y-%m-%d %H:%M:%S", tm_info);

	fprintf(fptr1, "Child 2:\tLeaving child 2 thread\n");
	fprintf(fptr1, "Child 2:\tExit time : %s\n", time_buff);

	fclose(fptr1);

	printf("Child 2:\tLeaving child 2 thread\n");
	printf("Child 2:\tExit time : %s\n", time_buff);

	pthread_exit(NULL);
}


void insert_beg(struct node ** head_ref, char ch_data, uint32_t cnt_data)
{
	/* new node */
	struct node * new = (struct node *)malloc(sizeof(struct node));
	new->ch = ch_data;
	new->count = cnt_data;
	new->next = *head_ref;
	
	/* update head */
	*head_ref = new;
}

uint8_t isthere(struct node * head, char ch_data)
{

	while(head != NULL && head->ch != ch_data)
		head = head->next;

	if(head == NULL)
		return 0;
	else
		return 1;
}

void inc_count(struct node * head, char ch_data)
{
	while(head->ch != ch_data)
		head = head->next;

	/* update cnt */
	(head->count)++;
}

void traverse_3(struct node * head, FILE * fptr)
{
	
	fprintf(fptr, "Child 1:\tChar \t Occurence\n");
	printf("Child 1:\tChar \t Occurence\n");

	while(head != NULL)
	{
		if(head->count == 3)
		{
			printf("Child 1:\t%c \t %d\n", head->ch, head->count);
			fprintf(fptr, "Child 1:\t%c \t %d\n", head->ch, head->count);
		}
		head = head->next;
	}
}

void delete_list(struct node ** head)
{
	struct node * tmp = NULL;

	while(*head != NULL)
	{
		tmp = *head;
		*head = (*head)->next;
		free(tmp); 
	}
}

static int periodic(int unsigned period, struct periodic_info *info)
{
    static int next_sig;
    int ret;
    unsigned int ns;
    unsigned int sec;
    struct sigevent sigev;
    timer_t timer_id;
    struct itimerspec itval;

 
    if (next_sig == 0)
        next_sig = SIGRTMIN;
   
    if (next_sig > SIGRTMAX)
        return -1;
    info->sig = next_sig;
    next_sig++;
  
    sigemptyset(&(info->alarm_sig));
    sigaddset(&(info->alarm_sig), info->sig);

  
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = info->sig;
    sigev.sigev_value.sival_ptr = (void *)&timer_id;
    ret = timer_create(CLOCK_MONOTONIC, &sigev, &timer_id);
    if (ret == -1)
        return ret;

 
    sec = period / 1000000;
    ns = (period - (sec * 1000000)) * 1000;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = ns;
    itval.it_value.tv_sec = sec;
    itval.it_value.tv_nsec = ns;
    ret = timer_settime(timer_id, 0, &itval, NULL);
    return ret;
}

static void wait_period(struct periodic_info *info)
{
    
    float delay = clock();
    int sig;
    sigwait(&(info->alarm_sig), &sig);
    
    
}

void signal_handler(int arg)
{
    
	FILE * fptr4 = fopen("logfile.txt", "a+");

    if(arg == SIGUSR2)
    {
        fprintf(fptr4, "SIG_HL:\t\tUSR2 signal received\n");
        printf("SIG_HL:\t\tUSR2 signal received\n");
        fprintf(fptr4, "SIG_HL:\t\tLeaving child thread\n");
        printf("SIG_HL:\t\tLeaving child threads\n");

        fclose(fptr4);
        pthread_cancel(tinfo1->tid);
        pthread_cancel(tinfo2->tid);

    }
    else if(arg == SIGUSR1)
    {
        fprintf(fptr4, "SIG_HL:\t\tUSR1 signal received\n");
        printf("SIG_HL:\t\tUSR1 signal received\n");
        fprintf(fptr4, "SIG_HL:\t\tLeaving child thread\n");
        printf("SIG_HL:\t\tLeaving child threads\n");

        fclose(fptr4);
        pthread_cancel(tinfo1->tid);
        pthread_cancel(tinfo2->tid);
    }
}