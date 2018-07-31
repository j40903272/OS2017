#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage long sys_Min(long a, long b)
{
	return (a<b)? a : b ;
}
