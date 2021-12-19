//------------------------------------------Includes--------------------------------------------//
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
//----------------------------------------------------------------------------------------------//
#define true 1
#define false 0
//------------------------------------------Global Variables------------------------------------//

int shmid; // shared memory ID
char* shmaddr;// char to know attachement location
char *ptr;//to divide the mem
int sem1;  // semaphore 1
int sem2;  // semaphore 2
key_t shm_key = 12613;
const int shm_size = 1024;
const int buffer_size=10;
//----------------------------------------------------------------------------------------------//

//-------------------------------------- Semaphores ----------------------------------------------//
/* arg for semctl system calls. */
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

int create_sem(int key, int initial_value)
{
    union Semun semun;

    int sem = semget(key, 1, 0666 | IPC_CREAT);

    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = initial_value; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    return sem;
}

void destroy_sem(int sem)
{
    if (semctl(sem, 0, IPC_RMID) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
}

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}
//------------------------------------------------------------------------------------------------//

//------------------------------------------Clean Shared Memory-----------------------------------//

void cleanup(int signum)
{
    shmctl(shmid, IPC_RMID, NULL);
}
//------------------------------------------------------------------------------------------------//

//------------------------------------------Producing Function------------------------------------//
void Produce(int shmid)
{
    int next[buffer_size];
    shmaddr = (char*) shmat (shmid, 0, 0); 
    if(atoi(shmaddr) == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        printf("\nShared memory attached at address %x\n", shmaddr);
        //strcpy((char*) shmaddr, "Initial string!");
    }
    while(true)
    {
        printf("\nProducing...\n\n");
    }
}

//------------------------------------------------------------------------------------------------//

//------------------------------------------Main------------------------------------------------//
void main()
{
    sem1 = create_sem(12614, 0);
    sem2 = create_sem(12615, 0);
    shmid = shmget(shm_key, shm_size, IPC_CREAT | 0644); // creating the shared memory
}