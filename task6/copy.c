#include<stdio.h>
#include<sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void help();
void using_mmap(const char* inputFile, const char* outputFile);
void using_read_write(const char* inputFile, const char* outputFile);



int main(int argc, char** argv){
    int opt;
    int index;
    char* arg1;
    char* arg2;
    short flag = 0;

    while ((opt = getopt(argc,argv,":m:h")) != -1)
    {
        switch (opt)
        {
        case 'h':
            help();
            exit(EXIT_SUCCESS);
        case 'm':
            flag = 1;
            index = strlen(optarg) + 1; //index of the second argument
            arg1 = optarg;
            arg2 = optarg + index;
            if(strncmp(arg2,"CLUTTER_IM_MODULE=xim",strlen(arg2)) != 0)
            {
                printf("i am following -m: first arg %s secodn %s \n", arg1,arg2);
                using_read_write(arg1,arg2);
            }
            else
            {
                printf("No second arguemnt");
                exit(EXIT_FAILURE);
            }
            break;
        case '?':
            printf("uknown option %c \n", optopt);
            exit(EXIT_FAILURE);
        case ':':
            printf("option needs a value \n");
            exit(EXIT_FAILURE);
        }
    }
    //no option choses
    if(flag != 1)
    {
       arg1 = argv[optind];
       arg2 = argv[++optind];
       printf("arg1: %s arg2: %s \n", arg1,arg2);
       // using_mmap(argv[(int)arg1],argv[(int)arg2]);
    }  
}

void help(){
    
}

void using_mmap(const char* inputFile, const char* outputFile){

}
void using_read_write(const char* inputFile, const char* outputFile){
    int fd;
    int fo;
    char buffer[100];

    if((fd = open(inputFile, O_RDONLY)) == -1){
        perror("no such a input file\n");
        exit(EXIT_FAILURE);
    }

    if((fo = open(outputFile, O_WRONLY)) == -1){
        perror("no such a output file\n");
        exit(EXIT_FAILURE);
    }

    if((read(fd,&buffer,sizeof(buffer))) == -1){
        perror("error occured durig reading buffer\n");
        exit(EXIT_FAILURE);
    }

    if(write(fo,&buffer,malloc_usable_size(buffer)) == -1){
        perror("error occured durig writing to file from buffer\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < strlen(buffer); i++){
        printf("%c",buffer[i]);
    }

     if((close(fd)) == -1 || close(fo) == -1){
        perror("error on closing occured\n");
        exit(EXIT_FAILURE);
    }


}

