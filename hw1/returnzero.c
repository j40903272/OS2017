#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage long sys_returnzero(void)
{
	return 0;
}
