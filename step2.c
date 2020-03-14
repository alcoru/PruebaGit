/*
 * Filename: step2.c
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
#include <termio.h>

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
	sem_t*	psem1;
	sem_t*	psem2;	
	int 	sem_value;
	char 	key;
	int count;
	int shmid;
	struct shmseg *shmp;
    int num;
    char name_file[100];
    char word[250];
    FILE * file;
	
	if (argc != 1) {
	fprintf(stderr, "USAGE: %s <iterations> <ID>\n", argv[0]);
    exit(1);
	}	
	
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666);
	if (shmid == -1) err_sys("Shared Memory Error");

	//attach to the shmp pointer
	shmp = shmat(shmid, NULL, 0);
	if (shmp == (void*)(-1)) err_sys("Shared Memory attachment error");

    //read shared memory name for the name of the file
    strcpy(name_file, shmp->fileName);
    while (1)
    {
        //read shared memory for number
        num = shmp->number;

        if (num == 0)
        {
            printf("See you !!");
            exit(0);
        }
        //read shared memory for word
        strcpy(word, shmp->word);

        //write in the file the word
        file = fopen(name_file, "a+");
        for (int i = 0; i < num; i++)
        {
            fputs(word, file);
        }
        fclose(file);
    }
    
    
}