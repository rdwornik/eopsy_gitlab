#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#define N	5
#define LEFT	( i + N ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define EXIT_FAILURE 1 
#define EXIT_SUCCESS 0
#define down pthread_mutex_lock
#define up pthread_mutex_unlock


void* philosphers(void *num);
void grab_forks( int i );
void put_away_forks( int i );
void think(int philosopher_id);
void eat(int philosopher_id);
void hungry(int philosopher_id);
void test( int i );


pthread_mutex_t	m; 		//initialized to 1
int	state[N];	    //initiated to THINKING's
pthread_mutex_t s[N];	//initialized to 0's
int phil[N] = {0,1,2,3,4};

//Odpowiedź na pytanie zawarte w poleceniu znajduje się w pliku opis_do_lab7.txt

int main(){
    int res;
    pthread_t a_thread[N];
    void* thread_result;
    int i;
    //create threads
    for(i = 0; i < N; i++){
        res = pthread_create(&a_thread[i],NULL,philosphers,&phil[i]);
        if(res != 0){
            perror("Thread creation failed");
            return(EXIT_FAILURE);
        }
    }
    //setting mutexes to 0 
    for(i = 0; i < N; i++){
        state[i] = THINKING;
        down(&s[i]);
    }
    
    //joing threads
    for(i = 0; i < N; i++){
        res = pthread_join(a_thread[i],&thread_result);
        if(res != 0){
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("Thread joined");
    exit(EXIT_SUCCESS);
    
}

//philoshpers function represents the action made by philosophers
void* philosphers(void *num)
{
    while(1){
        int* i = num;
        think(*i);
        grab_forks(*i);
        eat(*i);
        put_away_forks(*i);
    }
}


void grab_forks( int i )
{
    //we lock down mutex to eneter creatical section where we access resource shared by all threads
    //in this case this state[i] array
    //enter critical section
	down( &m );
		state[i] = HUNGRY;
        hungry(i);
    //when we grab_forks the test function is checking if we can start to eat 
		test( i );
    //leaving critical section
	up( &m );
    //if we could not eat since forks were taken we are freezed here since we put all mutexes to zero we wait until somebody
    //will not wake us up if we were able to eat we just continue 
	down( &s[i] );
}

void put_away_forks( int i )
{
    //enter crtitcal section since we are changing states
	down( &m );
		state[i] = THINKING;
    //here we inform philosphers on our left and right that we stopped eating and we let them eat by puttinh the semaphor up
    //they are not freezed anymore if they were freezed in function grab forks
    //we do is in critical section since
    //in test function we are checking and changing the state
		test( LEFT );
		test( RIGHT );
	up( &m );
}
/*this function has two contexts:
1) if we check if ourselves if we can eat if yes we put semaphore up to not be blocked in end of the grab_forks function
and change our state to eating
2) the other contex is when we want to inform philosphers beside us that they can eat by change there state to eating and
puting mutex up so they are not freezed in grab forks anymore
*/
void test( int i )
{
	if( state[i] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING )
	{
		state[i] = EATING;
        printf("Philospher: %d takes forks: %d and %d \n", i, LEFT,RIGHT);
		up( &s[i] );
	}
}

void hungry(int philosopher_id)
{
    printf("Phlisopher: %d is hungry \n", philosopher_id );
}

void eat(int philosopher_id)
{
    printf("Phlisopher: %d is eatining \n", philosopher_id);
    sleep(3);
}

void think(int philosopher_id)
{
    printf("Phlisopher: %d is thinking \n", philosopher_id);
    sleep(2);
}