//------------------------------------------Includes--------------------------------------------//
#include "header.h"
//----------------------------------------------------------------------------------------------//

int buffid; // shared memory ID of the buffer
int index_id ;  //ID of current items index in shm
void* buffaddr; // pointer to know the items location
void *index_addr;//to divide the mem
int items_sem;  // semaphore 1
int sem2;
int total_consumed = 0;

//----------------------------------------------------------------------------------------------//


void cleanup(int signum)
{
    printf("total consumed: %d\n",total_consumed);
    shmdt(buffaddr);
    shmdt(index_addr);
    exit(1);
}
//------------------------------------------------------------------------------------------------//


//------------------------------------------Consuming Function------------------------------------//
void Consume()
{
    printf("trying to consume\n");
    down(items_sem);
    int* index = (int*)index_addr;
    item* items = (item*) buffaddr;
    int new_index =*index-1;
    //
    
    //delete the item ... idk how !!!
    //
    (*index)--;
    up(items_sem);
    printf("consumed\n");
    printf("items are now %d\n",new_index+1);
    printf("total consumed: %d\n",total_consumed);
    total_consumed ++;

}

bool is_empty() {
    printf("checking if empty\n");
    down(items_sem);
    int* index = (int*)index_addr;
    up(items_sem);
    if (*index == -1)
    {
        printf("empty\n");
    }
    else
    {
        printf("not empty\n");
    }
    
    
    return (*index == -1);
}


//------------------------------------------------------------------------------------------------//


//------------------------------------------Main------------------------------------------------//
void main(int argc, char *argv[])
{
    signal(SIGINT,cleanup);
    if (argc ==1)
    {
        printf("provide the rate retard\n");
    }
    int cons_time;
    cons_time = atoi(argv[1]);
    
    items_sem = semget(items_sem_key,1,0666);
    

    //sem2 = semget(sem_test,1,0666);
    buffid = shmget(buff_key,  items_max *sizeof(item),0666);
    index_id = shmget(index_key,sizeof(int),0666);
    buffaddr = shmat(buffid,(void *)0, 0);
    index_addr = shmat(index_id,(void *)0, 0);
    printf("consumer started successfully\n");
    
    while (true)
    {
        sleep(cons_time);
        if(!is_empty())
            Consume();
    }
    

}