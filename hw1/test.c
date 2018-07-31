#include<sys/syscall.h>
#include<unistd.h>
#include<stdio.h>
int main(){
	syscall(337);//Show
	printf("%ld\n",syscall(338,94,87));
	printf("%ld\n",syscall(339,94,87));
	syscall(341);//cpu_util
	return 0;
}
