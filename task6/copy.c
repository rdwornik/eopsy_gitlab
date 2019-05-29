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
#include<string.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MAX_BUFFER_SIZE 100

void help();
void using_mmap(int fd_from, int fd_to);
void using_read_write(int fd_from, int fd_to);

int main(int argc, char** argv)
{
    int opt;
    int index;
    char* arg1;
    char* arg2;
    short flag = 0;
    int fd;
    int fo;

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

                if((fd = open(arg1, O_RDONLY)) == -1){
                    perror("no such a input file\n");
                    exit(EXIT_FAILURE);
                }

                if((fo = open(arg2, O_WRONLY | O_TRUNC)) == -1){
                    perror("no such a output file\n");
                    exit(EXIT_FAILURE);
                }
            
                using_read_write(fd,fo);
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
       optind++;
       arg2 = argv[optind];
       printf("arg1: %s arg2: %s \n", arg1,arg2);
      
        if((fd = open(arg1,O_RDWR)) == -1)
        {
            perror("no such a input file\n");
            exit(EXIT_FAILURE);
        }

        if((fo= open(arg2,O_RDWR)) == -1)
        {
            perror("no such a input file\n");
            exit(EXIT_FAILURE);
        }
       using_mmap(fd,fo);
    }  
}

void using_mmap(int fd_from, int fd_to)
{
    struct stat sb;
    char *input;
    char *out;

    if(fstat(fd_from,&sb) == -1)
    {
        perror("can't get file size \n");
        exit(EXIT_FAILURE);
    }

    if((input = mmap(NULL,sb.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fd_from,0)) == MAP_FAILED)
    {
        perror("Mapping input file failed \n");
        exit(EXIT_FAILURE);
    }

    ftruncate(fd_to,sb.st_size);

    if((out = mmap(NULL,sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to,0)) == MAP_FAILED)
    {
        perror("Mapping output file failed \n");
        exit(EXIT_FAILURE);
    }

    memcpy(out,input,sb.st_size);

    munmap(input,sb.st_size);
    munmap(out,sb.st_size);

    if((close(fd_from)) == -1 || close(fd_to) == -1)
    {
        perror("error on closing occured\n");
        exit(EXIT_FAILURE);
    }
}

void using_read_write(int fd_from, int fd_to)
{
    
    char buffer[MAX_BUFFER_SIZE];
    ssize_t size;

    if((size = read(fd_from,&buffer,sizeof(buffer))) == -1)
    {
        perror("error occured durig reading buffer\n");
        exit(EXIT_FAILURE);
    }

    if(!(size < sizeof(buffer)))
        printf("size of file is to big \n");
    else
    {
        if(write(fd_to,&buffer,size) == -1)
        {
            perror("error occured durig writing to file from buffer\n");
            exit(EXIT_FAILURE);
        }
        
        if((close(fd_from)) == -1 || close(fd_to) == -1)
        {
            perror("error on closing occured\n");
            exit(EXIT_FAILURE);
        }
    }
}

void help()
{
    int fd;
    struct stat sb;
    char *input;

    if((fd = open("help.txt", O_RDONLY)) == -1)
    {
        perror("no such a input file\n");
        exit(EXIT_FAILURE);
    }

    if(fstat(fd,&sb) == -1)
    {
        perror("can't get file size \n");
        exit(EXIT_FAILURE);
    }

    if((input = mmap(NULL,sb.st_size,PROT_READ, MAP_PRIVATE, fd,0)) == MAP_FAILED)
    {
        perror("Mapping input file failed \n");
        exit(EXIT_FAILURE);
    }
    
    if((close(fd)) == -1)
    {
        perror("error on closing help file occured\n");
        exit(EXIT_FAILURE);
    }
}

