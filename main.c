#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include "create_task.h"
#include "FIFO.h"
#include "RR.h"
#include "SJF.h"
#include "PSJF.h"

int main(){
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0,&mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1){
        fprintf(stderr ,"%s\n", "set affinity failed");
        exit(EXIT_FAILURE);
    }

	_fd = shm_open("TIME", O_CREAT | O_RDWR, S_IRWXU);

	if(_fd < 0){
        fprintf(stderr ,"%s\n","main open share memory failed");
        exit(EXIT_FAILURE);
    }
	if (ftruncate(_fd, sizeof(int)) == -1){
        fprintf(stderr ,"%s\n","main share memory ftruncate failed");
        exit(EXIT_FAILURE);
    }
	_T = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (_T == NULL){
        fprintf(stderr ,"%s\n","main mmap failed");
        exit(EXIT_FAILURE);
    }
	*_T = 0;

	struct sched_param param;
	param.sched_priority = 50;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1){
        fprintf(stderr ,"%s\n","main sched_setscheduler failed");
        exit(EXIT_FAILURE);
    }
	// if(sched_setparam(0, &param) == -1)
    //     perror("sched_setparam failed");
    //     exit(EXIT_FAILURE);
	char sched_method[20];
    
    
	scanf("%s%d", sched_method, &N);
    for(int i = 0; i < N; i++){
        scanf("%s%d%d", task[i].name, &task[i].arrive, &task[i].remain);
    }
    if(sched_method[0] == 'F'){
        FIFO();
    }
    else if(sched_method[0] == 'R'){
        RR();
    }
    else if(sched_method[0] == 'S'){
        SJF();
    }
    else{
        PSJF();
    }
    for(int i = 0; i < N; i++){
        printf("%s %d\n", task[i].name, task[i].pid);
        waitpid(task[i].pid, NULL, 0);
    }
	shm_unlink("TIME");
}