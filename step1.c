/*
 * Filename: step1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <sys/ioctl.h>
#include <termios.h>

#include <sys/shm.h>
#define SHM_KEY 0x1234

#define STDINFD  0

void err_sys(const char* text)
{
	perror(text);
	exit(1);
} 

struct shmseg {
	int number;
	int end;
    char fileName[100];
	char word[250];
};


int main(int argc, char *argv[])
{
	int 	input;
	char 	buffer[10+2];
	char 	key;
	int count;
	int shmid;
	struct shmseg *shmp;
    int number;
    char word[255];
	
	if (argc != 2) {
	fprintf(stderr, "USAGE: %s <file_name>\n", argv[0]);
    exit(1);
	}	

    //Create sharedMemory
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666|IPC_CREAT);
    printf("holaaaaa");
	if (shmid == -1) err_sys("Shared Memory Error");
    printf("Adioooos");

	//attach to the shmp pointer
	shmp = shmat(shmid, NULL, 0);
	if (shmp == (void*)(-1)) err_sys("Shared Memory attachment error");


    //Write in sharedMemory the name of the file
    strcpy((char*)(shmp->fileName), argv[1]);


    //Ask user for a number (0-9)
    while (1)
    {
        printf("Enter a number: \n");
        scanf("%d", &number);
        if (number < 0 || number > 9)
        {
            printf("Please, just valid numbers(0-9): \n");
            continue;
        }

        //Finalize process
        if(number == 0)
        {
            shmp->end=1;
            printf("Bye!!");
            exit(0);
        }

        //Write in sharedMemory the number
        shmp->number=number;
        shmp->end=0;


        //Ask user for a word
        printf("Enter a word: \n");
        scanf("%s", word);
        strcat(word, "\n");

        //Write in sharedmemory the word
        strcpy((char*)(shmp->word), word);  
        
    }
}