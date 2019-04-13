#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>


#define WITH_SIGNAL //define with signal if commented task 2 will be procesed if not task 3 is procesed
#define NUM_CHILD 10 //set number of children processess to create by parent                    


#ifdef WITH_SIGNAL //if WITH_SIGNAL not commented code from ifdef to end ifdef will bre proccesed

static int occurance = 0; //flag to mark if keyboard interrupt appeard it's global and static so process will refer to same place in memory
                            //of this variable

void info(int sig){         //function handling occurence of SIGTERM in situation when there was error during child creation by fork
    printf("Child[%d]: caught SIGTERM \n",getpid()); //getpid return PID of the current procces
}

void keyboardInterrupt(int sig){ //function handling occurence of SIGINT 
    printf("Parent[%d]: keyboard interrupt\n",getpid());
    occurance = 1;              //set flag up if signal caught
}

void childTermination(int sig){
    printf("Child[%d]: child is terminated\n",getpid()); 
}

#endif


int main() {

pid_t pids[NUM_CHILD];      //table to store PIDs of children
printf("fork program\n");
int n = NUM_CHILD;       //assign number of child to n, need it in while loop to wait for all children 

struct sigaction act;       //initializing sigaction structure act
act.sa_flags = 0;           //sigaction flag set to zero it modifies signal behaviour  ex SA_RESETHAND Reset signal action to SIG_DFL on receipt.
                            //when set to 0 do no modifaction
sigemptyset(&act.sa_mask); //initialize signal set to be empty

int sucexit = 0; //we count how many children excited successfully


for (int i = 0; i < NUM_CHILD; i++){
    pids[i] = fork();     //create children processes using fork fork return -1 if could not create child
                            //return 0 if child created and we are in child process
                            //return postivie value which is PID of child but we are in parent process
    
    #ifdef WITH_SIGNAL
                                //a_handler is a pointer to a function called when signal is received.
    act.sa_handler = SIG_IGN; //assign handler(pointer to function) to ignore
    for(int j = 0; j < _NSIG; j++){ //NSIG number of all signals which is 65 
        sigaction(j,&act,0);   //used to define the actions to be taken on receipt of the signal specified by signal number
    }

    act.sa_handler = SIG_DFL; //SIGCHLD is signal generated when Child process has stopped or exited
                             //if SIGCHLD would be ignored it would prevent zombies since parent would be not interested in exit code of children
    sigaction(SIGCHLD,&act,0); //by defult action SIGCHLD is ignored but child expect than sombody will handle it exit code
                                //in our case we are interesting in children return code it is why we are waiting for each child to finish 
                                //otherwise we would ignore and don't care what happen to child
                

    act.sa_handler = keyboardInterrupt;
    sigaction(SIGINT,&act,0); //If oact is not null (last argument in sigaction), sigaction writes the previous signal action to the location it refers to
                                //we put 0 since we are not intereseted in previous signal action
    #endif


         switch(pids[i])         //fork return 0 if child was created or -1 if there was error
        {
            case -1:            //by checking PIDs of the process we now if we are we are in parent process or in child process
                perror("fork failed\n");    //informing about err
                kill(-2, SIGTERM);   //sending signal of flag SIGTERM to all processes from kill description
                                        //If pid is -1, sig shall be sent to all processes 
                                        /*If pid is negative, but not -1, sig shall be sent to all processes 
                                        (excluding an unspecified set of system processes)
                                         whose process group ID is equal to the absolute value of pid, 
                                         and for which the process has permission to send a signal.*/
                exit(1);                //exit with code 1 means there was some errorr
            case 0:
                #ifdef WITH_SIGNAL
                    act.sa_handler = SIG_IGN;   //ignoring key interrupt during  child processing
                    sigaction(SIGINT,&act,0);
                    act.sa_handler = childTermination;      //handling child termination signal
                    sigaction(SIGTERM,&act,0); /* SIGTERM Sent as a request for a process to finish. Used by UNIX when shut-
                                                ting down to request that system services stop. This is the default
                                                signal sent from the kill command.*/
                #endif
                (void) signal(SIGTERM,info);    //handling signal send
                printf("Child[%d]: created with parent PID: %d\n",getpid(), getppid()); //getppid returns parent PID
                ++sucexit; //count successul exits
                sleep(10); 
                printf("Child[%d]: completed execution\n",getpid());
                
                exit(0);
        }
    sleep(1);
 #ifdef WITH_SIGNAL
    if(occurance){ //if occurance flag is set to 1 it means that keyboard interrupt occured
        printf("Parent[%d]: Interrupt during creation process\n", getpid()); //we inform user about what happen
        kill(-2,SIGTERM); //we send signal SIGTERM to all processes in a group children which where not been created yet will not be successfully created in final result
        break;
    }
 #endif
}

int stat_val;
pid_t child_pid;    //initialize child_pid variable

while(n > 0){       //while loop runs until we wait for all children 
        child_pid = wait(&stat_val); //wait system call causes a parent process to pause until one of its child processes is stopped. The call
                                        //returns the PID of the child process.
                                        //the status information will bewritten to the memory location variable stat_val
        printf("Child[%d] After waiting, child has finishied\n", child_pid);
        if(WIFEXITED(stat_val))   //stat_val is nonzero if the child is terminated normally
            printf("child exited with code %d\n", WEXITSTATUS(stat_val));
        else
        {
            printf("Child terminated abnormally\n"); //inform user child terminated ABNORMALLY
        }
        --n;
    }

printf("No more processes to be synchronized\n");
printf("Number of successful exit codes: %d \n",sucexit);

/*
At the end of the main process, the old service handlers of all
signals should be restored.
*/
#ifdef WITH_SIGNAL
  act.sa_handler = SIG_DFL;
    for(int j = 0; j < _NSIG; j++){
        sigaction(j,&act,0);
    }
#endif



exit(0);

}
