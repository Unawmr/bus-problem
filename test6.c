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
	unsigned short sem_array[3];
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	}arg;

	key=ftok(".",'s');
	sem_id=semget(key,3,IPC_CREAT|0644);

	sem_array[0]=1;				
	sem_array[1]=0;
	sem_array[2]=0;			
	arg.array = sem_array;
	ret = semctl(sem_id, 0, SETALL, arg);	
	if (ret == -1) 
		printf("SETALL failed (%d)\n", errno); 
	printf("车已停稳,等待售票员开门\n");
}
void semaphore_p(int m);
void semaphore_v(int n);
void del();
int main()
{
	init();
	int i;
	pid_t pid1,pid2;
	if((pid1=fork())==-1)
		printf("fork error");
	else if(pid1==0)
		{
		for(i=0;i<3;i++)
		{
			semaphore_p(2);
			printf("汽车发车\n");
			sleep(1);
			printf("行驶中...\n");
			sleep(3);
			printf("车已到站\n");
			semaphore_v(0);
 		}
		exit(0);
		}else {	
			if((pid2=fork())==-1)
				printf("fork error");
			else if(pid2==0){
					for(i=0;i<3;i++)
					{
						semaphore_p(1);
						printf("乘客上下车...\n");
						sleep(2);
						semaphore_v(0);
					}
					exit(0);
					
				
				}else{
					for(i=0;i<3;i++)
					{
						semaphore_p(0);
						printf("售票员开门\n");
						sleep(1);
						semaphore_v(1);
						semaphore_p(0);
						printf("售票员关门\n");
						sleep(1);
						semaphore_v(2);
					}
					wait(0);
					wait(0);
					del();
					exit(0);
					}	
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
    sem_b.sem_op = -1; 
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
    }
    
}

void semaphore_v(int n)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = n;
    sem_b.sem_op = 1; 
    sem_b.sem_flg = SEM_UNDO;       
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        
    }
    
}
