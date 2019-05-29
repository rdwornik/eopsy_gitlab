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

    //zczytujemy flagi za pomocą funkcji getopt
    while ((opt = getopt(argc,argv,":m:h")) != -1)
    {
        switch (opt)
        {
        case 'h':
            help();
            exit(EXIT_SUCCESS);
        case 'm':
            flag = 1; //ustawiamy flage na jeden jeśli ta opcja została wybrana
            index = strlen(optarg) + 1; //index of the second argument
            arg1 = optarg;          //optarg zwraca tablice charów
            arg2 = optarg + index; //przesuwamy się o długość pierwszego stringa by zczytać drugi argument
            
            //sprawdzamy czy użytkownik podał drugi argument
            if(strncmp(arg2,"CLUTTER_IM_MODULE=xim",strlen(arg2)) != 0)
            {
            //otwieram plik na inputcie flaga read only więcej uprawnień nam nie potrzeba    
                if((fd = open(arg1, O_RDONLY)) == -1)
                {
                    perror("no such a input file\n");
                    exit(EXIT_FAILURE);
                }
            //otwieram plik do którego będziemy tylko nadpisywać, flaga O_TRUNC czyści plik z zawartości aby nie doszło do dopisania
                if((fo = open(arg2, O_WRONLY | O_TRUNC)) == -1)
                {
                    perror("no such a output file\n");
                    exit(EXIT_FAILURE);
                }
            //wywołanie funkcij    
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
        //w optind znajduje się index kolejnych argumentów
        arg1 = argv[optind];  
        optind++;
        arg2 = argv[optind];

        //znowu otwieramy plik tylko zczytywanie będzie nam potrzebne      
        if((fd = open(arg1,O_RDONLY)) == -1)
        {
            perror("no such a input file\n");
            exit(EXIT_FAILURE);
        }
        //otwieramy plik tym razem uprawnienia do zczytywania i nadpisywania będą nam potrzbne ponieważ używają mmap
        //inaczej dostaniemy komunikat o braku uprawnień do odczytu pliku.
        if((fo = open(arg2,O_RDWR| O_TRUNC)) == -1)
        {
            perror("no such a input file\n");
            exit(EXIT_FAILURE);
        }
       using_mmap(fd,fo);
    }  
}

void using_mmap(int fd_from, int fd_to)
{
    //stuct stat jest to zmienna systemowa w której przechowujemy statystki pliku z funkcji fstat
    struct stat sb;
    char *input;
    char *out;

    //funkcja stat zbiera statyski takie jak wielkość czy też poziom uprawnień nam potrzeba tylko wielkość pliku
    if(fstat(fd_from,&sb) == -1)
    {
        perror("can't get file size \n");
        exit(EXIT_FAILURE);
    }

    //mmap zwraca nam wskaźnik do pierwszego indexu bloku pamięci w którym znajduje się nasz input plik
    //nadajemy mu uprawnienia do sczytywania ponieważ więcej nam nie będzie potrzebne a mapowanie jest prywatne ponieważ nie ma potrzeby
    //dzielenie się z innymi procesami tym fragmentem pamięci
    //sb.st_size przechowuje wielkość pliku
    if((input = mmap(NULL,sb.st_size,PROT_READ, MAP_PRIVATE, fd_from,0)) == MAP_FAILED)
    {
        perror("Mapping input file failed \n");
        exit(EXIT_FAILURE);
    }

    //jako że nasz wyczyszczony output plik ma wielkość zero nie możemy przypisać bloku pamięci o większej wielkość do bloku o wielkości większej
    //dlatego rozszerzamy wielkość output pliku do wielkości naszego input pliku
    ftruncate(fd_to,sb.st_size);

    //tak jak poprzednio mmap zwracam nam wskaźnik do miejsce w pamięci w którym znajduje się nasz output plik 
    //tym razem nadajemy inne uprawnienia jako że będzięmy zapisywać kopie input do jakiegoś miejsca w pamięci 
    //musi on mieć uprawnienia do zapisu, jednocześnie musi mieć ustawiony flage MAP_SHARED aby pamięć mogła być jawnie przypisana 
    //inaczej dostaniemy hash 
    if((out = mmap(NULL,sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to,0)) == MAP_FAILED)
    {
        perror("Mapping output file failed \n");
        exit(EXIT_FAILURE);
    }

    //kopiujemy miejsce w pamięci w którym znajduje się nasz input plik do miejsca w pamięci w którym znajduje się nasz output plik
    memcpy(out,input,sb.st_size);

    //munmap usuwanie mapowanie dla określonego przedziału jest to dobra praktyka gdy zakończymy mapowanie
    munmap(input,sb.st_size);
    munmap(out,sb.st_size);

    //zamykam bezpiecznie pliki aby użytkownik mógł mieć donich dostęp po zakończeniu działania programu
    if((close(fd_from)) == -1 || close(fd_to) == -1)
    {
        perror("error on closing occured\n");
        exit(EXIT_FAILURE);
    }
}

void using_read_write(int fd_from, int fd_to)
{
    
    char buffer[MAX_BUFFER_SIZE]; //buffor przechowuje chary z czytanego pliku
    ssize_t size;

    //czytamy z pliku chary i wpisujemy do buffora
    //funkcja read zwraca wielkość pliku
    if((size = read(fd_from,&buffer,sizeof(buffer))) == -1)
    {
        perror("error occured durig reading buffer\n");
        exit(EXIT_FAILURE);
    }
    //sprawdzamy czy bufor nie został przepełniony
    if(!(size < sizeof(buffer)))
        printf("size of file is to big \n");
    else
    {
        //zapisujemy chary z buffora do pliku
        if(write(fd_to,&buffer,size) == -1)
        {
            perror("error occured durig writing to file from buffer\n");
            exit(EXIT_FAILURE);
        }

        //zamykam bezpiecznie pliki aby użytkownik mógł mieć donich dostęp po zakończeniu działania programu
        if((close(fd_from)) == -1 || close(fd_to) == -1)
        {
            perror("error on closing occured\n");
            exit(EXIT_FAILURE);
        }
    }
}

void help()
{
    //to samo co dla funkcji using_mmap
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

    //wykorzystujemy fakt że map zwracam na wskaźnik do początku miejsca w pamięci w którym znajdują się nasze zasoby 
    for(int i = 0; i < sb.st_size; i++)
    {
        printf("%c", input[i]);
    }

    printf("\n");
    
    if((close(fd)) == -1)
    {
        perror("error on closing help file occured\n");
        exit(EXIT_FAILURE);
    }
}

