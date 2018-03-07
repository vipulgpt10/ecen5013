
/* Kfifo hello world module */

#include <linux/kernel.h>		// included for KERN_INFO
#include <linux/module.h>		// included for all kernel modules
#include <linux/moduleparam.h>
#include <linux/init.h>			// included for __init and __exit macros

#include <linux/kfifo.h>
#include <linux/list.h>

#include <linux/sched.h>   		//wake_up_process()
#include <linux/kthread.h> 		//kthread_create(), kthread_run()
#include <linux/err.h> 			//IS_ERR(), PTR_ERR()
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vipul");
MODULE_DESCRIPTION("kfifo module");



/* fifo size in elements (bytes) */
#define FIFO_SIZE	32

static struct kfifo my_kfifo;

static struct task_struct * prod, * cons;
static struct task_struct * ptask, * ctask, * tmp_task;

int producer(void * data)
{
	//
	while(1)
	{ 
		set_current_state(TASK_UNINTERRUPTIBLE); 

		if(kthread_should_stop())
		{
			printk("STOP command!\n");
			break;
		}

		ptask = current;

		printk(KERN_INFO "Inside producer\n");
		kfifo_in(&my_kfifo, &ptask, sizeof(ptask));
		
		schedule_timeout(HZ);

	}

	return 0;
}

int consumer(void * data)
{
	
	int i;

	while(1)
	{ 
		set_current_state(TASK_INTERRUPTIBLE); 

		if(kthread_should_stop())
		{
			printk("STOP command!\n");
			break;
		}

		if(!(kfifo_is_empty(&my_kfifo)))
		{
			printk(KERN_INFO "Inside consumer\n");

			/* get max of 5 bytes from the fifo */
			i = kfifo_out(&my_kfifo, &ctask, sizeof(ctask));
			printk(KERN_INFO "Current PID: %d", (int)ctask->pid);
			printk(KERN_INFO "Process: %s", ctask->comm);
			printk(KERN_INFO "Vruntime: %lld", ctask->se.vruntime);

			tmp_task = list_entry(ctask->tasks.prev, struct task_struct, tasks);
			printk(KERN_INFO "Previous PID: %d", (int)tmp_task->pid);
			printk(KERN_INFO "Process: %s", tmp_task->comm);
			printk(KERN_INFO "Vruntime: %lld", tmp_task->se.vruntime);

			tmp_task = list_entry(ctask->tasks.next, struct task_struct, tasks);
			printk(KERN_INFO "Previous PID: %d", (int)tmp_task->pid);
			printk(KERN_INFO "Process: %s", tmp_task->comm);
			printk(KERN_INFO "Vruntime: %lld", tmp_task->se.vruntime);

		}
		
		schedule_timeout(HZ/20);

	}

	return 0;
}


static int __init mykthread_init(void)
{
	int err;
	unsigned int ret;

	ret = kfifo_alloc(&my_kfifo, FIFO_SIZE, GFP_KERNEL);

	if(ret)
	{
		printk(KERN_ERR "error kfifo_alloc\n");
		return ret;
	}

	prod = kthread_run(producer, NULL, "producer");

	if(IS_ERR(prod))
	{
		printk("Unable to start kthread.\n");
		err = PTR_ERR(prod);
		cons = NULL;
		return err;
	}

	cons = kthread_run(consumer, NULL, "consumer");

	if(IS_ERR(cons))
	{
		printk("Unable to start kthread.\n");
		err = PTR_ERR(cons);
		cons = NULL;
		return err;
	}

	
	return 0;
}

static void __exit mykthread_cleanup(void)
{
	printk(KERN_INFO "Cleaning up! Bye.\n");
	kfifo_free(&my_kfifo);

	if(prod)
	{
		kthread_stop(prod);
		printk("Producer Thread stopped!\n");
		prod = NULL;
	}

	if(cons)
	{
		kthread_stop(cons);
		printk("Consumer thread stopped!\n");
		cons = NULL;
	}
}

module_init(mykthread_init);
module_exit(mykthread_cleanup);