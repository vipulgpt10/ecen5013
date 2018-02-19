/* Hello world sample module */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vipul");
MODULE_DESCRIPTION("Hello world module");

/* Module parameters */
static char * myname = "default";
static int age = 20;
module_param(myname, charp, S_IRUGO);
module_param(age, int, S_IRUGO);

static int __init hello_init(void)
{
	printk(KERN_INFO "hello %s! You are %d years old. \n", myname, age);
	return 0;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up! Bye %s. \n", myname);
}

module_init(hello_init);
module_exit(hello_cleanup);