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

void help();
void using_mmap(int fd_from, int fd_to);
void using_read_write(int fd_from, int fd_to);



int main(int argc, char** argv){
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

                if((fo = open(arg2, O_WRONLY)) == -1){
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
       arg2 = argv[++optind];
       printf("arg1: %s arg2: %s \n", arg1,arg2);

        if((fd = open(arg1, O_RDONLY,S_IRUSR | S_IWUSR)) == -1){
                    perror("no such a input file\n");
                    exit(EXIT_FAILURE);
                }

        if((fo = open("dest", O_RDWR | O_CREAT,0666)) == -1){
            perror("no such a output file\n");
            exit(EXIT_FAILURE);
        }

       using_mmap(fd,fo);
    }  
}

void help(){
    
}

void using_mmap(int fd_from, int fd_to){
    struct stat sb;
    if(fstat(fd_from,&sb) == -1){
        perror("can't get file size \n");
        exit(EXIT_FAILURE);
    }



    printf("file size is %ld\n", sb.st_size);

  char *input = mmap(NULL,sb.st_size,PROT_READ | PROT_WRITE, MAP_PRIVATE, fd_from,0);
 

 
  char *out = mmap(NULL,sb.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fd_to,0);
 
  for(int i = 0; i < sb.st_size; i++)
  printf("%c",input[i]);
//   printf("\n");
   memcpy(out,input,sb.st_size);

   if((close(fd_from)) == -1 || close(fd_to) == -1){
            perror("error on closing occured\n");
            exit(EXIT_FAILURE);
        }
}




void using_read_write(int fd_from, int fd_to){
    
    char buffer[100];
    ssize_t size;
    if((size = read(fd_from,&buffer,sizeof(buffer))) == -1){
        perror("error occured durig reading buffer\n");
        exit(EXIT_FAILURE);
    }

    if(!(size < sizeof(buffer)))
        printf("size of file is to big \n");
    else{
        if(write(fd_to,&buffer,size) == -1){
            perror("error occured durig writing to file from buffer\n");
            exit(EXIT_FAILURE);
        }
        
        if((close(fd_from)) == -1 || close(fd_to) == -1){
            perror("error on closing occured\n");
            exit(EXIT_FAILURE);
        }
    }
}

