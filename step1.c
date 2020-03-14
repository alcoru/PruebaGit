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
    sem_t*  psem1;
    sem_t*  psem2;
    int result;
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

    /* Create psem1 */
    psem1 = (sem_t*)sem_open("/semaphore1", O_CREAT, 0600, 0);
    if (psem1 == SEM_FAILED) {
        err_sys("Open psem1");
    }

    /* Read and print psem1 value */
    result = sem_getvalue(psem1, &sem_value);
    if (result < 0) {
        err_sys("Read psem1");
    }
    printf("PROCESS 1(SEM1): %d\n", sem_value);

    /* Wait for sem_value to be 0 */
    while (sem_value > 0) {
        sem_wait(psem1);
        sem_value--;
    }

    /* Repeat */
    count = 0;
    while (count < 1) {
        /* Increment the value of semaphore to initialize it to set argument */
        count++;

        /* Post to psem1 */
        result = sem_post(psem1);
        if (result < 0) {
            err_sys("Post psem1");
        }
    }

    /* Read and print psem1 value */
    result = sem_getvalue(psem1, &sem_value);
    if (result < 0) {
        err_sys("Read psem1");
    }
    printf("PROCESS 1(SEM1): %d\n", sem_value);

    /* Create psem2 */
    psem2 = (sem_t*)sem_open("/semaphore2", O_CREAT, 0600, 0);
    if (psem2 == SEM_FAILED) {
        err_sys("Open psem2");
    }

    /* Read and print psem2 value */
    result = sem_getvalue(psem2, &sem_value);
    if (result < 0) {
        err_sys("Read psem2");
    }
    printf("PROCESS 1(SEM2): %d\n", sem_value);

    /* Wait for sem_value to be 0 */
    while (sem_value > 0) {
        sem_wait(psem2);
        sem_value--;
    }

    /* Repeat */
    count = 0;
    while (count < 0) {
        /* Increment the value of semaphore to initialize it to set argument */
        count++;

        /* Post to psem1 */
        result = sem_post(psem2);
        if (result < 0) {
            err_sys("Post psem2");
        }
    }

    /* Read and print psem2 value */
    result = sem_getvalue(psem2, &sem_value);
    if (result < 0) {
        err_sys("Read psem2");
    }
    printf("PROCESS 1(SEM2): %d\n", sem_value);


    //Create sharedMemory
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666|IPC_CREAT);
    printf("%d", shmid);
	if (shmid == -1) err_sys("Shared Memory Error");

	//attach to the shmp pointer
	shmp = shmat(shmid, NULL, 0);
	if (shmp == (void*)(-1)) err_sys("Shared Memory attachment error");


    //Write in sharedMemory the name of the file
    strcpy((char*)(shmp->fileName), argv[1]);


    //Ask user for a number (0-9)
    while (1)
    {
        result = sem_wait(psem1);
        if (result < 0) {
            err_sys("Wait psem1");
        }

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
            shmp->number=0;
            printf("Bye!!");
            break;
        }

        //Write in sharedMemory the number
        shmp->number=number;


        //Ask user for a word
        printf("Enter a word: \n");
        scanf("%s", word);
        strcat(word, "\n");

        //Write in sharedmemory the word
        strcpy((char*)(shmp->word), word);

        result = sem_post(psem2);
        if (result < 0) {
            err_sys("Signal psem2");
        }
        
    }

    /* Close psem1 */
    result = sem_close(psem1);
    if (result != 0) {
        err_sys("Close psem1");
    }

    /* Close psem2 */
    result = sem_close(psem2);
    if (result != 0) {
        err_sys("Close psem2");
    }
}