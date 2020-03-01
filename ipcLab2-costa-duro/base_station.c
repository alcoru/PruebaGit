/*
 * Filename: satellite.c
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

#define MAXPENDING 5    /* Maximum number of simultaneous connections */
#define BUFFSIZE 255    /* Size of message to be reeived */
#define CLOSECONNECTION "SHv+Hf<MdR/dpx5w" /* word to close connection */

void err_sys(char *mess) { perror(mess); exit(1); }

void handle_client(int sock) {
    char buffer[BUFFSIZE];
    int received = -1;
    while(1)
    {
        /* Read from socket */
        read(sock, &buffer[0], BUFFSIZE);
        printf("Message from client: %s\n", buffer);

        /* Write to socket */
        write(sock, buffer, strlen(buffer) + 1);

        printf("%s", buffer);
        printf("%s", CLOSECONNECTION);

        if(strcmp(buffer, CLOSECONNECTION) == 0)
        {
            printf("%s", buffer);
            printf("%s", CLOSECONNECTION);
            break;
        }
    }

    printf("Bye");

    /* Close socket */
    close(sock);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver, echoclient;
    int serversock, clientsock;
    int result;    

    /* Check input arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* Create TCP socket */
    serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serversock < 0) {
        err_sys("Error socket");
    }

    /* Set information for sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* we reset memory */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* ANY address */
    echoserver.sin_port = htons(atoi(argv[1]));       /* server port */

    /* Bind socket */
    result = bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver));
    if (result < 0) {
        err_sys("Error bind");
    }

    /* Listen socket */
    result = listen(serversock, MAXPENDING);
    if (result < 0) {
        err_sys("Error listen");
    }

    /* loop */
    while (1) {
        unsigned int clientlen = sizeof(echoclient);

        /* Wait for a connection from a client */
        clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen);
        if (clientsock < 0) {
            err_sys("Error accept");
        }
        fprintf(stdout, "Client: %s\n", inet_ntoa(echoclient.sin_addr));

        /* Call function to handle socket */
        handle_client(clientsock);
    }
}
