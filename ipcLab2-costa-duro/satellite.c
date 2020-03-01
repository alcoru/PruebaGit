/*
 * Filename: base_station.c
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdint.h>
#include <errno.h>

#define BUFFSIZE 255

void err_sys(char *mess) { perror(mess); exit(1); }

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int sock, result;
    int received = 0;
    char word[255];
    int number;



    /* Check input arguments */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip_server> <word> <port>\n", argv[0]);
        exit(1);
    }

    /* Try to create TCP socket */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        err_sys("Error socket");
    }
    
    /* Set information for sockaddr_in */
    memset(&echoserver, 0, sizeof(echoserver));       /* reset memory */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
    echoserver.sin_port = htons(atoi(argv[3]));       /* server port */
    
    /* Try to have a connection with the server */
    result = connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver));
    if (result < 0) {
        err_sys("Error connect");
    }

    while(1){

        printf("Enter a number: \n");
        scanf("%d", &number);
        if (number < 0 && number > 9)
        {
            printf("Enter a valid number(0-9): \n");
        }

        if (number == 0)
        {
            word[0] = '.';
            write(sock, word, strlen(word) + 1);
            break;
        }

        printf("Enter a word: \n");
        scanf("%s", word);

        for (int i = 0; i < number; i++)
        {
            /* Write to socket */
            write(sock, word, strlen(word) + 1);
            fprintf(stdout, " sent \n");
            /* Read from socket */
            read(sock, buffer, BUFFSIZE);
            fprintf(stdout, " %s ...done \n", buffer);
        }
    }

    printf("See you\n");
    
    /* Close socket */
    close(sock);
    
    exit(0);
}