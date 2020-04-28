extern int _fd, *_T, N;
typedef struct task
{
    char name[100];
    int arrive;
    int remain;
    int pid;
}Task;

extern Task task[1000];

void create_task(Task *task);
void run_task(int time, Task *task);
