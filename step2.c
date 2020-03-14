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

char inkey(void) {
    char c;
    struct termio param_ant, params;

    ioctl(STDINFD, TCGETA, &param_ant);

    params = param_ant;
    params.c_lflag &= ~(ICANON | ECHO);
    params.c_cc[4] = 1;

    ioctl(STDINFD, TCSETA, &params);

    fflush(stdin); fflush(stderr); fflush(stdout);
    read(STDINFD, &c, 1);

    ioctl(STDINFD, TCSETA, &param_ant);
    return c;
}

struct shmseg {
	int number;
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
	/*
	if (argc != 1) {
	fprintf(stderr, "USAGE: %s <iterations> <ID>\n", argv[0]);
    exit(1);
	}	*/

    if ((psem1 = (sem_t*)sem_open("/semaphore1", O_CREAT, 0600, 0)) == SEM_FAILED) {
        err_sys("OPEN psem1");
    }
    if ((psem2 = (sem_t*)sem_open("/semaphore2", O_CREAT, 0600, 0)) == SEM_FAILED) {
        err_sys("OPEN psem2");
    }
	
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666);
	if (shmid == -1) err_sys("Shared Memory Error");

	//attach to the shmp pointer
	shmp = shmat(shmid, NULL, 0);
	if (shmp == (void*)(-1)) err_sys("Shared Memory attachment error");

    //read shared memory name for the name of the file
    strcpy(name_file, shmp->fileName);

    /*Create and clean the file*/
    file = fopen(name_file, "w+");
    fputs("", file);
    fclose(file);
    
    while (1)
    {
        if (sem_wait(psem2) < 0) err_sys("WAIT psem2");

        //read shared memory for number
        num = shmp->number;

        printf("%d", num);

        if (num == 0)
        {
            printf("See you !!\n");
            break;
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

        if (sem_post(psem1) < 0) err_sys("SIGNAL psem1");
    }

    //we can close de semaphore ID
    if (sem_close(psem1) != 0) err_sys("CLOSE psem1");
    if (sem_close(psem2) != 0) err_sys("CLOSE psem2");
}