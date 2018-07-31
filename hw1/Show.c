#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage long sys_Show(void)
{
	printk("B04902103 Tsai Yun Da \nB04902015 Wang Che Kai \n");
	return 0;
}
