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

struct queue{
    int top;
    int end;
    int element[1000];
};
struct queue Queue;


void push(int a){
    Queue.element[Queue.end] = a;
    Queue.end = (Queue.end + 1) % 1000;

}

int is_empty(){
    if(Queue.top == Queue.end)
        return 1;
    else
        return 0;
}

int pop(){
    int a = Queue.element[Queue.top];
    Queue.top = (Queue.top + 1) % 1000;
    return a;
}

int get_(){
    int a = Queue.element[Queue.top];
    return a;
}

int RR_cmp(const void *a, const void *b){
    Task *c = (Task *)a;
    Task *d = (Task *)b;
    if(c->arrive != d->arrive){
        return c->arrive > d->arrive;
    }
    else{
        return c->pid > d->pid;
    }
}

void RR(){
    Queue.top = 0;
    Queue.end = 0;
    qsort(task, N, sizeof(Task), RR_cmp);
    int remain_of_task[N];
    for(int i = 0; i < N; i++){
        remain_of_task[i] = task[i].remain;
    }
    //event[][] [0] : wait, create, run; [1] : time; [2] : index
    int task_end = 0; //begin time of now job
    int task_begin = 0; //end time of now job
    int now_job = 0; //runnung job 
    int created_job = -1; // index of the newwest created job
    int finish = 0; //number of the finished job
    int event_ptr = 0;
    
    while(finish < N){
        int created = 0;
        if(is_empty()){
            now_job = finish;
            while(remain_of_task[now_job] == -1){
                now_job = now_job + 1;
            }
        }
        if(task[now_job].arrive > task_end){
            event[event_ptr][0] = 0;
            event[event_ptr][1] = task[now_job].arrive - task_end;
            event[event_ptr][2] = -1;
            event_ptr += 1;
            task_begin = task[now_job].arrive;

            event[event_ptr][0] = 1;
            event[event_ptr][1] = 0;
            event[event_ptr][2] = now_job;
            push(now_job);
            event_ptr += 1;
            created_job += 1;
        }
        else if(task[now_job].arrive == task_end){
            task_begin = task[now_job].arrive;
            event[event_ptr][0] = 1;
            event[event_ptr][1] = 0;
            event[event_ptr][2] = now_job;
            push(now_job);
            event_ptr += 1;
            created_job += 1;
        }
        now_job = pop();
        int is_finish = 0;
        if(remain_of_task[now_job] <= 500){
            task_end = task_begin + remain_of_task[now_job];
            remain_of_task[now_job] = -1;
            finish += 1;
            is_finish = 1;
        }
        else{
            task_end = task_begin + 500;
            remain_of_task[now_job] -= 500;
        }
        for(int i = created_job + 1; i < N; i++){
            if(task[i].arrive >= task_begin && task[i].arrive < task_end){
                created += 1;
                push(i);
            }
            else{
                break;
            }
        }
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
        if(!is_finish){
            push(now_job);
        }
        // now_job = (now_job + 1) % (created_job + 1);
        // if(finish < N){
        //     while(remain_of_task[now_job] == -1){
        //         now_job = (now_job + 1) % (created_job + 1);
        //     }
        // }
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
            // fprintf(stderr,"run %s for %d\n", task[event[i][2]].name, event[i][1]);
            run_task(event[i][1], &task[event[i][2]]);
        }
    }
}