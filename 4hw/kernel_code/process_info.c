
#include <linux/kernel.h>		// included for KERN_INFO
#include <linux/module.h>		// included for all kernel modules
#include <linux/moduleparam.h>
#include <linux/init.h>			// included for __init and __exit macros
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/list.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vipul");
MODULE_DESCRIPTION("Process info module");


static int __init processinfo_init(void)
{

	int count, pstate;
	struct task_struct * task;
	struct list_head * node_ptr;


	for(task = current; task != &init_task; task = task->parent)
	{
		count = 0;
		printk(KERN_INFO "ProcessName:\t%s\n", task->comm);
		printk(KERN_INFO "PID:\t\t%d\n", (int)task->pid);

		pstate = (int)task->state;

		switch(pstate)
		{
			case 0:
			{
				printk(KERN_INFO "State:\t\tRUNNING\n");
				break;
			}
			case 1:
			{
				printk(KERN_INFO "State:\t\tINTERRUPTIBLE\n");
				break;
			}
			case 2:
			{
				printk(KERN_INFO "State:\t\tUNINTERRUPTIBLE\n");
				break;
			}
			case 4:
			{
				printk(KERN_INFO "State:\t\tSTOPPED\n");
				break;
			}
			case 8:
			{
				printk(KERN_INFO "State:\t\tTRACED\n");
				break;
			}
			default:
				printk(KERN_INFO "State:\t\tUNRUNNABLE\n");
	
		}
	
		/* Iterate over all children of current process */
		list_for_each(node_ptr, &task->children)
			count++;

		printk(KERN_INFO "No. of children:\t%d\n", count);
		/* -20 to +19 maps to 100 to 139 */
		printk(KERN_INFO "Nice Value:\t%d\n", task_nice(task));

		printk(KERN_INFO "-----------------------------------"); 
		
	}
	
	return 0;
}

static void __exit processinfo_cleanup(void)
{
	printk(KERN_INFO "Cleaning up! Bye. \n");
}

module_init(processinfo_init);
module_exit(processinfo_cleanup);