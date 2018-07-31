#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/types.h>

void *thread_func(void *a){
	for(int i=0;i<3;i++){
		printf("Thread %d is running\n",*(int*)a);
		double d=0;
		for(int j=0;j<50000000;j++) d+=0.1f;
	}
	pthread_exit(a);
}

int main(int argc, char* argv[]){
	cpu_set_t msk;
	CPU_ZERO(&msk);
	CPU_SET(0, &msk);
	sched_setaffinity(0, sizeof(msk),&msk);
	
	struct sched_param param;	
	param.sched_priority = 99;
	if( argc>1 && strcmp(argv[1],"SCHED_FIFO")==0) sched_setscheduler(getpid(),SCHED_FIFO,&param);
	pthread_t tid[2];
	pthread_attr_t attr;
	int cnt[2]={1,2};

	for(int i=0;i<2;i++){
		if(!pthread_create(&tid[i],NULL,thread_func,&cnt[i]))
			printf("Thread %d was created\n",i+1);
	}
	for(int i=0;i<2;i++)
		pthread_join(tid[i],NULL);
	exit(0);
}
