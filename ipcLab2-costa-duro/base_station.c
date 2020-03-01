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

void err_sys(char *mess) { perror(mess); exit(1); }

int receive_int(int *num, int fd)
{
    int32_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = read(fd, data, left);
        if (rc <= 0) { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    *num = ntohl(ret);
    return 0;
}

void handle_client(int sock) {
    int received_int = 0;

    int return_status = read(sock, &received_int, sizeof(received_int));
    if (return_status > 0) {
    fprintf(stdout, "Received int = %d\n", ntohl(received_int));
    }
    else {
    // Handling erros here
    }
    /*char buffer[BUFFSIZE];
    int received = -1;

    /* Read from socket 
    read(sock, &buffer[0], BUFFSIZE);
    printf("Message from client: %s\n", buffer);

    /* Write to socket 
    write(sock, buffer, strlen(buffer) + 1);

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
