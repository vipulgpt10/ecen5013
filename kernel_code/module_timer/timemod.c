/* Kernel module using a kernel timer */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/timer.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vipul");
MODULE_DESCRIPTION("Kernel module executing every __x milliseconds");

/* Module parameters */
static char * myname = "default";
/* Timer expiration time in milliseconds */
static unsigned long exptime = 500;
/* Parameters permission : Readable */
module_param(myname, charp, S_IRUGO);
module_param(exptime, ulong, S_IRUGO);

/* Timer initialization */
static struct timer_list ktimer;

static size_t count = 0;	/* Counter variable */

void timer_callback(unsigned long data)
{
	int tmp;
	count++;

	printk(KERN_INFO "Name [%s]	Up-Time in msecs [%d]	Count [%d].\n", 
			myname, jiffies_to_msecs(jiffies), (int)count);
	tmp = mod_timer(&ktimer, jiffies + msecs_to_jiffies(exptime));
	if(tmp)
		printk(KERN_ERR "Error in timer!\n");
	return;
}

static int __init timemod_init(void)
{
	int tmp;

	printk(KERN_INFO "Hello %s! Expiration time - %ld msecs \n", 
			myname, exptime);

	/* setup_timer(const struct timer_list *, void (*func)(ulong), data); */
	setup_timer(&ktimer, timer_callback, 0);

	printk(KERN_INFO "Starting timer: (%d) \n", jiffies_to_msecs(jiffies));

	tmp = mod_timer(&ktimer, jiffies + msecs_to_jiffies(exptime));
	if(tmp)
		printk(KERN_ERR "Error in timer!\n");

	return 0;
}

static void __exit timemod_cleanup(void)
{
	int tmp;
	printk(KERN_INFO "Cleaning up! Bye %s. \n", myname); 
	
	tmp = del_timer(&ktimer);
	if (tmp)
		printk(KERN_ALERT "The timer was still in use!\n");

	printk("Uninstalling timer module. \n");
}

module_init(timemod_init);
module_exit(timemod_cleanup);

