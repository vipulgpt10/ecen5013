 /**
  * @file mysort.c
  * @brief The implementation file for string sort system call
  *
  * This implementation file provides the function definition for string 
  * sort system call.
  *
  * @author Vipul Gupta
  * @date 31 January 2018
  *
  */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>


SYSCALL_DEFINE3(mysort, uint8_t __user *, str, size_t, len, uint8_t __user *, sort_str)
{
    size_t i, cnt, j = 0;
    long copied;
    
    /* Allocate memory in kernel space */
    uint8_t * hash_tbl = (uint8_t *)kcalloc(128, sizeof(uint8_t), GFP_KERNEL);
    uint8_t * tmp_buff = (uint8_t *)kmalloc(len * sizeof(uint8_t), GFP_KERNEL);

    printk(KERN_INFO "Executing system call: sys_mysort\n");

    printk(KERN_INFO "Size of the buffer: %d Bytes\n", (int)len);

    if(hash_tbl == NULL || tmp_buff ==NULL)
    {
    	printk(KERN_ERR "ENOMEM: Cannot allocate memory\n");
        return -ENOMEM;
    }

    /* Checks and copies data */
    copied = copy_from_user(tmp_buff, str, len);
    if(copied < 0 || copied == len)
    {
    	printk(KERN_ERR "EFAULT: Bad Address\n");
    	return -EFAULT;
    }        

    printk(KERN_INFO "Sorting started\n");

    /* Store the count of each char in hash_tbl based on ASCII value */
    for(i = 0; i < len; i++)
    {
        *(hash_tbl + (*(tmp_buff + i))) += 1;
    }

    for(i = 128; i > 0; i--)
    {
        cnt = *(hash_tbl + i - 1);
        if(cnt)
        {
            while(cnt--)
            {
                *(tmp_buff + j) = (i - 1);
                j++;
            }
        }
    }

    printk(KERN_INFO "Sorting completed\n");

    if(copy_to_user(sort_str, tmp_buff, len))
    {
        printk(KERN_ERR "EFAULT: Bad Address");
        return -EFAULT;
    }

    kfree((void * )tmp_buff);
    kfree((void * )hash_tbl);

    printk(KERN_INFO "System call sys_mysort successful");
    return 0;

}
