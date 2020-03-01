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
    int number_to_send, converted_number;



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

    printf("Enter a number: \n");

    while(1){

        scanf("%d", &number_to_send);
        if (number_to_send < 0 && number_to_send > 9)
        {
            printf("Enter a valid number(0-9): \n");
            continue;
        }

        if (number_to_send == 0)
        {
            break;
        }
        
        converted_number = htonl(number_to_send);

        // Write the number to the opened socket
        write(sock, &converted_number, sizeof(converted_number));

        printf("Enter a word: \n");
        scanf("%s", word);

        /* Write to socket */
        write(sock, word, strlen(word) + 1);
        fprintf(stdout, " sent \n");
    }

    

    printf("See you\n");
    /* Write to socket 
    write(sock, argv[2], strlen(argv[2]) + 1);
    fprintf(stdout, " sent \n");
    
    /* Read from socket
    read(sock, buffer, BUFFSIZE);
    fprintf(stdout, " %s ...done \n", buffer);*/
    
    /* Close socket */
    close(sock);
    
    exit(0);
}