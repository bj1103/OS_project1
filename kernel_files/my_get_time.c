#include <linux/module.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_my_get_time(unsigned long *s, unsigned long *n){
	struct timespec now;
	getnstimeofday(&now);
	*s = now.tv_sec;
	*n = now.tv_nsec;
	return 1;
}
