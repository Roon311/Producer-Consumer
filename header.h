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
#include<stdbool.h>
//----------------------------------------------------------------------------------------------//


//------------------------------------------Global Variables------------------------------------//
key_t items_sem_key = 13165;
key_t buff_key = 12613;
key_t index_key = 13164;
//key_t sem_test = 13166;
int items_max =10;  // the number of max items in the buffer
short sleep_interval = 10000;  //10 ms
//------------------------------------------Global Variables------------------------------------//
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
typedef struct Item
{
    //any dummy data
    //I added a serial for debugging
    int data1,serial;
    char x;
    float u;
    
}item;