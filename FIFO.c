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

int event[10000][3];

int FIFO_cmp(const void *a, const void *b){
    Task *c = (Task *)a;
    Task *d = (Task *)b;
    if(c->arrive != d->arrive){
        return c->arrive > d->arrive;
    }
    else{
        return c->pid > d->pid;
    }
}

void FIFO(){
    qsort(task, N, sizeof(Task), FIFO_cmp);
    //event[][] [0] : wait, create, run; [1] : time; [2] : index
    int task_end = 0;
    int task_begin = 0;
    int now_job = -1;
    int created_job = -1;
    int finish = -1;
    int event_ptr = 0;
    while(finish < N){
        int created = 0;
        for(int i = created_job + 1; i < N; i++){
            if(task[i].arrive >= task_begin && task[i].arrive < task_end){
                created += 1;
            }
            else{
                break;
            }
        }
        // fprintf(stderr, "%d\n", created);
        int check_start = created_job;
        for(int i = check_start + 1; i <= check_start + created; i++){
            int runtime = task[i].arrive - task_begin;
            if(runtime == 0){
                event[event_ptr][0] = 1;
                event[event_ptr][1] = 0;
                event[event_ptr][2] = i;
                event_ptr += 1;
                created_job += 1;
            }
            else{
                event[event_ptr][0] = 2;
                event[event_ptr][1] = runtime;
                event[event_ptr][2] = now_job;
                event_ptr += 1;
                event[event_ptr][0] = 1;
                event[event_ptr][1] = 0;
                event[event_ptr][2] = i;
                event_ptr += 1;
                created_job += 1;
                task_begin += runtime;
            }
        }
        if(task_end > task_begin){
            event[event_ptr][0] = 2;
            event[event_ptr][1] = task_end - task_begin;
            event[event_ptr][2] = now_job;
            event_ptr += 1;
            task_begin = task_end;
        }
        now_job += 1;
        finish += 1;
        if(finish < N){
            if(task[now_job].arrive > task_end){
                event[event_ptr][0] = 0;
                event[event_ptr][1] = task[now_job].arrive - task_end;
                event[event_ptr][2] = -1;
                event_ptr += 1;
                task_begin = task[now_job].arrive;

                event[event_ptr][0] = 1;
                event[event_ptr][1] = 0;
                event[event_ptr][2] = now_job;
                event_ptr += 1;
                created_job += 1;
            }
            else if(task[now_job].arrive == task_end){
                task_begin = task[now_job].arrive;
                event[event_ptr][0] = 1;
                event[event_ptr][1] = 0;
                event[event_ptr][2] = now_job;
                event_ptr += 1;
                created_job += 1;
            }
            task_end = task_begin + task[now_job].remain;
        }
    }
    for(int i = 0; i < event_ptr; i++){
        // fprintf(stderr, "event = %d\n", event[i][0]);
        if(event[i][0] == 0){
            for(int t = 0; t < event[i][1]; t++){
                volatile unsigned long i; 
                for(i=0;i<1000000UL;i++);
            }
        }
        else if(event[i][0] == 1){
            // fprintf(stderr,"create %s\n", task[event[i][2]].name);
            create_task(&task[event[i][2]]);
        }
        else{
            // fprintf(stderr,"run %s %d\n", task[event[i][2]].name, task[event[i][2]].pid);
            run_task(event[i][1], &task[event[i][2]]);
        }
    }
}