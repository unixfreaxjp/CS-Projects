/*
 * Sample_LKM.c
 *
 *  Undated on: Jan 26, 2014
 *      Author: Craig Shue
 *      Updated: Hugh C. Lauer
 */

// We need to define __KERNEL__ and MODULE to be in Kernel space
// If they are defined, undefined them and define them again:
#undef __KERNEL__
#undef MODULE

#define __KERNEL__ 
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/prinfo.h>
#include <linux/sched.h>
#include <linux/list.h>



unsigned long **sys_call_table;

long *ref_syscall1;


asmlinkage long sys_getprinfo(struct prinfo *info){
	struct prinfo kinfo;
	struct task_struct *youngest_child;
	struct task_struct *younger_sibling;
	struct task_struct *older_sibling;
	struct task_struct *pos;
	
	youngest_child = NULL;
	younger_sibling = NULL;
	older_sibling = NULL;
	
	kinfo.state = current->state;		
	kinfo.pid = current->pid;
	kinfo.parent_pid = current->parent->pid;
	kinfo.uid = current_uid();
	
	kinfo.cutime = 0;
	kinfo.cstime = 0;

	list_for_each_entry(pos , &(current->children), children){
		kinfo.cutime += cputime_to_usecs(pos->utime);
		kinfo.cstime += cputime_to_usecs(pos->stime);
		if(youngest_child == NULL || timespec_compare(&pos->start_time, &youngest_child->start_time) > 0){
			youngest_child = pos;
		}			
	}
	if(youngest_child != NULL && youngest_child->pid != 0)
		kinfo.youngest_child = youngest_child->pid;
	else 
		kinfo.youngest_child = -1;
		
	list_for_each_entry(pos, &(current->sibling), sibling){
		if((timespec_compare(&pos->start_time, &current->start_time) > 0) && (younger_sibling == NULL || timespec_compare(&pos->start_time, &younger_sibling->start_time) < 0)){
			younger_sibling = pos;
		}
		
		if((timespec_compare(&pos->start_time, &current->start_time) < 0) && (older_sibling == NULL || timespec_compare(&pos->start_time, &older_sibling->start_time) > 0)){
			older_sibling = pos;
		}
		
	}
	
	if(younger_sibling != NULL && younger_sibling->pid != 0)
		kinfo.younger_sibling = younger_sibling->pid;
	else 
		kinfo.younger_sibling = -1;
		
	if(older_sibling != NULL && older_sibling->pid != 0)
		kinfo.older_sibling = older_sibling->pid;
	else 
		kinfo.older_sibling = -1;
		
	kinfo.start_time = timespec_to_ns(&current->real_start_time);
	kinfo.user_time = cputime_to_usecs(current->utime);
	kinfo.sys_time = cputime_to_usecs(current->stime);
	
	
		
	

	printk(KERN_INFO "Got prinfo for %d", kinfo.pid);
	
	if (copy_to_user(info, &kinfo, sizeof(kinfo))) {
		return -EFAULT;
	}
	
	return 0;
}


static unsigned long **find_sys_call_table(void) {
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) {
			printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX", (unsigned long) sct);
			return sct;
		}

	offset += sizeof(void *);
	}

	return NULL;
}	// static unsigned long **find_sys_call_table(void)


static void disable_page_protection(void) {
	/*
	Control Register 0 (cr0) governs how the CPU operates.

	Bit #16, if set, prevents the CPU from writing to memory marked as
	read only. Well, our system call table meets that description.
	But, we can simply turn off this bit in cr0 to allow us to make
	changes. We read in the current value of the register (32 or 64
	bits wide), and AND that with a value where all bits are 0 except
	the 16th bit (using a negation operation), causing the write_cr0
	value to have the 16th bit cleared (with all other bits staying
	the same. We will thus be able to write to the protected memory.

	It's good to be the kernel!
	*/

	write_cr0 (read_cr0 () & (~ 0x10000));

}	//static void disable_page_protection(void)


static void enable_page_protection(void) {
	/*
	See the above description for cr0. Here, we use an OR to set the
	16th bit to re-enable write protection on the CPU.
	*/

	write_cr0 (read_cr0 () | 0x10000);

}	// static void enable_page_protection(void)


static int __init interceptor_start(void) {
	/* Find the system call table */
	if(!(sys_call_table = find_sys_call_table())) {
		/* Well, that didn't work.
		Cancel the module loading step. */
		return -1;
	}


	/* Store a copy of all the existing functions */
	ref_syscall1 = (void *)sys_call_table[__NR_cs3013_syscall1];

	/* Replace the existing system calls */
	disable_page_protection();

	sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)sys_getprinfo;

	enable_page_protection();

	/* And indicate the load was successful */
	printk(KERN_INFO "Loaded interceptor!");

	return 0;
}	// static int __init interceptor_start(void)


static void __exit interceptor_end(void) {
	/* If we don't know what the syscall table is, don't bother. */
	if(!sys_call_table)
		return;

	/* Revert all system calls to what they were before we began. */
	disable_page_protection();
	sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)ref_syscall1;
	enable_page_protection();

	printk(KERN_INFO "Unloaded interceptor!");
}	// static void __exit interceptor_end(void)

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);


