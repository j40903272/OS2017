#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage long sys_Multiply(long a, long b)
{
	return a*b;
}
