#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
int sem_id;

void init()
{	
	key_t key;
	int ret;
	unsigned short sem_array[2];
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	}arg;

	key=ftok(".",'s');

	sem_id=semget(key,2,IPC_CREAT|0644);

	sem_array[0]=1;				// product
	sem_array[1]=1;			// space
	arg.array = sem_array;
	ret = semctl(sem_id, 0, SETALL, arg);	
	if (ret == -1) 
		printf("SETALL failed (%d)\n", errno); 
	printf("车已停好,等待售票员开门\n");
}
void semaphore_p(int m);
void semaphore_v(int n);
void del();
int main()
{
	init();
	pid_t pid1,pid2;
	//if(!semaphore_v())
			//exit(EXIT_FAILURE);
	int i;
	if((pid1=fork())==0)
	{
		//usleep(10);
		for(i=0;i<3;i++)
		{
		semaphore_p(0);
		printf("车已到站，已停好\n");
		semaphore_v(0);
		usleep(10);
		semaphore_p(0);
		printf("行驶中\n");
		semaphore_v(0);
		usleep(10);
		} 
		exit(0);
	}else if((pid2=fork())==0)
		{
			usleep(10);
			for(i=0;i<3;i++)
			{
			semaphore_p(1);
			printf("乘客上下车\n");
			semaphore_v(1);
			
			} 
			exit(0);
		}else{
			
			for(i=0;i<3;i++)
			{
			semaphore_p(0);
			semaphore_p(1);
			printf("开门\n");
			semaphore_v(1);
			
			printf("关门\n");
			semaphore_v(0);
			
			}
			del();
			wait(0);
	}
}
void del()
{
	semctl(sem_id,IPC_RMID,0);
}
void semaphore_p(int m)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = m;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
    }
    
}

void semaphore_v(int n)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = n;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        
    }
    
}
