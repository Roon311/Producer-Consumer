//------------------------------------------Includes--------------------------------------------//
#include "header.h"
//------------------------------------------------------------------------------------------------//


//------------------------------------------Variables--------------------------------------------//
int buffid; // shared memory ID of the buffer
int index_id ;  //ID of current items index in shm
void* buffaddr; // pointer to know the items location
void *index_addr;//to divide the mem
int items_sem;  // semaphore 1

int total_produced = 0; //counts the totl number of items produced

//------------------------------------------------------------------------------------------------//

//------------------------------------------Clean Shared Memory-----------------------------------//

void cleanup(int signum)
{
    printf("total produced :%d\n",total_produced);
    shmdt(buffaddr);
    shmdt(index_addr);
    shmctl(buffid, IPC_RMID, NULL);
    shmctl(index_id, IPC_RMID, NULL); //could be removed???
                                      //nah...

    semctl(items_sem, 1, IPC_RMID); 
    exit(1);
}
//------------------------------------------------------------------------------------------------//

//------------------------------------------Producing Function------------------------------------//
void Produce()
{

    printf("trying to produce\n");
    
    int* index = (int*)index_addr;
    item* items = (item*) buffaddr;
    int new_index =*index+1;
    //
    total_produced ++;
    items[new_index].data1 =1;
    items[new_index].serial =total_produced;
    //
    (*index) ++;
    
    
    printf("produced succefully\n");
    printf("items are now %d\n",new_index+1);
    printf("total produced :%d\n",total_produced);
    
    
}

bool is_full() {
    printf("checking if full...\n");
    down(items_sem);
    int* index = (int*)index_addr;
    
    if (*index == items_max -1)
    {
        printf("full\n");
    }
    else
    {
        printf("not full\n");
    }
    

    return (*index == items_max -1);
}



//------------------------------------------Main------------------------------------------------//
void main(int argc, char *argv[])
{
    signal(SIGINT,cleanup);
    if (argc ==1)
    {
        printf("provide the rate in the arguments \n");
        exit(-1);
    }
     
    int prod_rate = atoi(argv[1]);     // rate per second
    
    if ( prod_rate <1  || prod_rate > 100)
    {
        printf("Rate error..Range from 1 to 100 per second\n");
        exit(-1);
    }
    printf("Producer started successfully\n");
    items_sem = create_sem(items_sem_key, 0);   
    //sem2 = create_sem(sem_test, 0);
    buffid = shmget(buff_key, items_max *sizeof(item), IPC_CREAT | 0644); // creating the shared memory of buffer
    index_id = shmget(index_key, sizeof(int), IPC_CREAT | 0644); // creating the shared memory of index 

    buffaddr = shmat(buffid,(void *)0, 0);
    index_addr = shmat(index_id,(void *)0, 0);
    *(int*)index_addr=-1;
    up(items_sem);
    while (true)
    {
        usleep((1000000/prod_rate));
        while(is_full())            //is_full starts with a down
        {
            up(items_sem);
            usleep(sleep_interval);
        }
        Produce();
        up(items_sem);
    }
}