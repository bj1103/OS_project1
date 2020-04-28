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

int _fd = 0, *_T = 0;
int N;
Task task[1000];

void create_task(Task *task){
    int pid = fork();
    if(pid < 0){
        fprintf(stderr ,"%s\n","fork failed");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        char arrive[20], remain[20], pid[20];
        sprintf(arrive, "%d", task->arrive);
        sprintf(remain, "%d", task->remain);
        sprintf(pid, "%d", getpid());
        execlp("./task", "./task", pid, task->name, arrive, remain, NULL);
    }
    else{
        task->pid = pid;
    }
}

void run_task(int time, Task *task){
    *_T = time;
    task->remain -= time;
    struct sched_param param;
    param.sched_priority = 99;
    if(sched_setscheduler(task->pid, SCHED_FIFO, &param) == -1){
        fprintf(stderr ,"%s\n","task sched_setscheduler failed");
        exit(EXIT_FAILURE);
    }
}