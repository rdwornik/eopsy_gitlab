#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

// semaphore sv = 1;
// loop forever {
// P(sv);
// critical code section;
// V(sv);
// noncritical code section;
// }


#define EXIT_FAILURE 1
#define N 5

#define LEFT ( left_fork_id + N - 1) % N
#define RIGHT ( left_fork_id + 1) % N

void eat(int philosopher_id);
void think(int philosopher_id);
void hungry(int philosopher_id);
static int semaphore_v(int sem_id,int left);
static int semaphore_p(int sem_id,int left);
static void del_semvalue(int sem_id,int sem_num);
static int set_semvalue(int sem_id, int sem_num);

void philosopher(int num);
void put_away_forks( int left_fork_id );
void grab_forks(int left_fork_id);
void test(int left_fork_id);

union semun {
int val;
struct semid_ds *buf;
unsigned short *array;
};

//we need shared memory t share process Id beetwen processes
struct shared_data{
  int sem_id;
};

//setting flag to shared memory 
void* create_shared_memory(size_t size){
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;

return mmap(NULL,size,protection,visibility, -1,0);
}

struct shared_data* shmem;

int main(){
int i;
struct shared_data shared;
pid_t pids[N];      
printf("fork program\n");

//create five semaphores each represents one fork
shared.sem_id = semget((key_t)123467, 5, 0666 | IPC_CREAT);
printf("sem id is: %d \n", shared.sem_id);

//set semaphores value to one
for(i = 0; i < N; i++)
if (!set_semvalue(shared.sem_id, i)) {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
}

//create shared memory
shmem = create_shared_memory(128);
memcpy(shmem, &shared,sizeof(shared));


// create 5 processes each represents one semaphore
for (i = 0; i < N; i++){
    pids[i] = fork();     
    switch(pids[i])        
        {
            case -1:            
                perror("fork failed\n");    
                kill(-2, SIGTERM);   
                exit(EXIT_FAILURE);
            case 0:
                philosopher(i);      
                exit(0);
        }
}

int stat_val;
pid_t child_pid;   
int n = N;
while(n > 0 ){      
        child_pid = wait(&stat_val);                                    
        printf("Child[%d] After waiting, child has finishied\n", child_pid);

        if(WIFEXITED(stat_val))   //stat_val is nonzero if the child is terminated normally
            printf("child exited with code %d\n", WEXITSTATUS(stat_val));
        else
        {
            printf("Child terminated abnormally\n"); //inform user child terminated ABNORMALLY
        }
        n--;
}

/*The del_semvalue function has almost the same form as set_semvalue, except that the call to semctl uses the
command IPC_RMID to remove the semaphore’s ID */
for (i = 0; i < N; i++)
    del_semvalue(shmem->sem_id, i);

//end of main
}



void put_away_forks( int left_fork_id ) {
    if (!semaphore_v(shmem->sem_id,left_fork_id)) exit(EXIT_FAILURE);
}

void grab_forks(int left_fork_id) {
    if (!semaphore_p(shmem->sem_id,left_fork_id)) exit(EXIT_FAILURE);
    sleep(1);
}

//function reperenst philospher life cycle excecuted by thread when it is created
void philosopher(int num) {
    while (1) {
        think(num);
        hungry(num);    
        grab_forks(num);
        eat(num);
        put_away_forks(num);
    }
}

void hungry(int philosopher_id){
    printf("Phlisopher: %d is hungry \n", philosopher_id );
}
void eat(int philosopher_id){
    printf("Phlisopher: %d is eatining \n", philosopher_id);
    sleep(3);
}
void think(int philosopher_id){
    printf("Phlisopher: %d is thinking \n", philosopher_id);
    sleep(2);
}

/*semaphore_p changes the semaphore by –1. This is the “wait” operation*/
static int semaphore_p(int semid, int left_fork_id)
{
    int right_fork_id = (left_fork_id + 1)% N;
    
    //by creating a buffer wee lock two semaphores at once  
        struct sembuf semaphor_as_a_fork[2] = {
            {right_fork_id,-1,0},
            {left_fork_id,-1,0}
        };
        if (semop(semid, semaphor_as_a_fork, 2)  ) {
            fprintf(stderr, "semaphore_p failed lock forks sem id: %d \n", semid);
            return(0);
            }
            printf("Phlisopher: %d is takes fork %d and %d \n", left_fork_id , (LEFT 
            + 1)%5, RIGHT);    
            return(1);         
}

/*emaphore_v is similar to semaphore_p except for setting the sem_op part of the sembuf structure to 1. This is
the “release” operation, so that the semaphore becomes available*/
static int semaphore_v(int semid, int left_fork_id)
{
    int right_fork_id = (left_fork_id + 1)% N;
    //by creating a buffer wee unlock two semaphores at once  
        struct sembuf semaphor_as_a_fork[2] = {
            {right_fork_id,1,0},
            {left_fork_id,1,0}
        };
        if (semop(semid, semaphor_as_a_fork, 2)  ) {
                fprintf(stderr, "semaphore_v failed lock forks sem id: %d \n", semid);
                return(0);
            }
        return(1);   
}


/*
The semctl function allows direct control of semaphore information:
int semctl(int sem_id, int sem_num, int command, ...);
The first parameter, sem_id , is a semaphore identifier, obtained from semget . The sem_num parameter
is the semaphore number. You use this when you’re working with arrays of semaphores. Usually, this is
0, the first and only semaphore. The command parameter is the action to take, and a fourth parameter, if
present, is a union semun

union semun {
int val;
struct semid_ds *buf;
unsigned short *array;
}

The two common values of command are:

SETVAL : Used for initializing a semaphore to a known value. The value required is passed as the
val member of the union semun . This is required to set the semaphore up before it’s used for
the first time.

IPC_RMID : Used for deleting a semaphore identifier when it’s no longer required.


*/
static int set_semvalue(int semid,int sem_num)
{
    union semun sem_union;
    sem_union.val = 1;
    if (semctl(semid, sem_num, SETVAL, sem_union) == -1) return(0);
    return(1);
}

static void del_semvalue(int semid,int sem_num)
{
    union semun sem_union;
    if (semctl(semid, sem_num, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore number: %d sem_num: %d\n", semid, sem_num);
}
