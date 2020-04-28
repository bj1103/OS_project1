#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>

int main(int argc, char const *argv[]){
    int pid = atoi(argv[1]);
    int arrive_time = atoi(argv[3]);
    int run_time = atoi(argv[4]);

    int _fd = shm_open("TIME", O_RDWR, S_IRWXU);

	if(_fd < 0){
        fprintf(stderr, "%s\n","child open share memory failed");
        exit(EXIT_FAILURE);
    }
    int *_T = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (_T == NULL){
        fprintf(stderr, "%s\n","child mmap failed");
        exit(EXIT_FAILURE);
    }
    unsigned long start_s, start_n, end_s, end_n;
    if(syscall(334, &start_s, &start_n) == -1){
        fprintf(stderr, "%s\n","call sys_my_get_time failed");
    }
    while(run_time > 0){
        run_time -= *_T;
        for(int t = 0; t < *_T; t++){
            volatile unsigned long i;
            // fprintf(stderr, "%d\n", t);
            for(i=0;i<1000000UL;i++);
        }
        
        if(run_time == 0){
            if(syscall(334, &end_s, &end_n) == -1){
                fprintf(stderr, "%s\n","call sys_my_get_time failed");
            }
            char output[200];
            sprintf(output, "[Project1] %d %lu.%09lu, %lu.%09lu", pid, start_s, start_n, end_s, end_n);
            if(syscall(333, output) == -1){
                fprintf(stderr, "%s\n","call sys_my_print failed");
            }
            exit(0);
        }
        struct sched_param param;
        param.sched_priority = 1;
        if(sched_setscheduler(0, SCHED_FIFO, &param) == -1){
            fprintf(stderr, "%s\n","child sched_setscheduler failed");
            exit(EXIT_FAILURE);
        }
        sched_yield();
    }
}