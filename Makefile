all: main.c create_task.c FIFO.c SJF.c PSJF.c RR.c task.c
	gcc -o main main.c create_task.c FIFO.c SJF.c PSJF.c RR.c -lrt
	gcc -o task task.c -lrt

clear:
	rm -f main task
