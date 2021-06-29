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

	sem_array[0]=1;				
	sem_array[1]=1;			
	arg.array = sem_array;
	ret = semctl(sem_id, 0, SETALL, arg);	
	if (ret == -1) 
		printf("SETALL failed (%d)\n", errno); 
	printf("车已停好,等待售票员开门\n");
}
void semaphore_p();
void semaphore_v();
int main()
{
	
	init();
	pid_t pid1,pid2;
	//if(!semaphore_v())
			//exit(EXIT_FAILURE);
	int i;
	if((pid1=fork())==0)
	{
		for(i=0;i<5;i++)
		{
		semaphore_p(0);
		printf("车已到站，已停好\n");
		semaphore_v(0);
		sleep(1);
		semaphore_p(0);
		printf("行驶中\n");
		sleep(3);
		semaphore_v(0);
		} 
		exit(0);
	}else if((pid2=fork())==0)
		{
			for(i=0;i<5;i++)
			{
			semaphore_p(1);
			printf("乘客上下车\n");
			sleep(2);
			semaphore_v(1);
			sleep(1);
			} 
			exit(0);
		}else{
			for(i=0;i<5;i++)
			{
			semaphore_p(0);
			semaphore_p(1);
			printf("开门\n");
			semaphore_v(1);
			sleep(1);
			printf("关门\n");
			semaphore_v(0);
			}
			wait(0);
	}
	
	//if (!semaphore_p()) 
			//exit(EXIT_FAILURE);
	//printf("product init is %d\n",semctl(sem_id,0,GETVAL));
	/*int a=0;
	a++;
	if(a>0)
	{
		printf("%d\n",a);
		a--;
		printf("%d\n",a);
	}*/
}
void semaphore_p(int m)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = m;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        //return(0);
    }
    //return(1);
}

void semaphore_v(int n)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = n;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        //return(0);
    }
    //return(1);
}
