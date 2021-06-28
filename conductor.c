#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
int sem_id;
void semaphore_v();
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

	sem_array[0]=0;				// product
	sem_array[1]=0;			// space
	arg.array = sem_array;
	ret = semctl(sem_id, 0, SETALL, arg);	
	if (ret == -1) 
		printf("SETALL failed (%d)\n", errno); 
	printf("%d\n",ret);
	printf("product init is %d\n",semctl(sem_id,0,GETVAL));
	printf("space init is %d\n\n",semctl(sem_id,1,GETVAL));
}
int main()
{
	init();
	/*while(1){
if(){

		printf("开门");
		sleep(1);
		printf("关门");
		}*/

	
}
void del()
{
	semctl(sem_id,IPC_RMID,0);
}
static int semaphore_p(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}

void semaphore_v()
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        //return(0);
    }
    //return(1);
}
