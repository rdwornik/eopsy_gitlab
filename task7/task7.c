#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#define N	5
#define LEFT	( i + N - 1 ) % N
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
	down( &m );
		state[i] = HUNGRY;
        hungry(i);
		test( i );
	up( &m );
	down( &s[i] );
}

void put_away_forks( int i )
{
	down( &m );
		state[i] = THINKING;
		test( LEFT );
		test( RIGHT );
	up( &m );
}

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