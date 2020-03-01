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
#include <time.h>

#define MAXPENDING 5    /* Maximum number of simultaneous connections */
#define BUFFSIZE 255    /* Size of message to be reeived */
#define CLOSECONNECTION "SHv+Hf<MdR/dpx5w" /* word to close connection */

void err_sys(char *mess) { perror(mess); exit(1); }

void closeConnection(int sock1, int sock2){
    printf("Bye\n");

    /* Close socket */
    close(sock1);
    close(sock2);
}

void handle_client(int sock, char ip[]) {
    struct sockaddr_in echose;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int sock2, result2;
    int received = -1;
    FILE * fp;

    /* Intorduce TimeStamp */
    char buff[20];
    struct tm *sTm;
    struct tm *pTm;

    /* Try to create TCP socket */
    sock2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        err_sys("Error socket");
    }

    /* Set information for sockaddr_in */
    memset(&echose, 0, sizeof(echose));       /* reset memory */
    echose.sin_family = AF_INET;                  /* Internet/IP */
    echose.sin_addr.s_addr = inet_addr(ip);  /* IP address */
    echose.sin_port = htons(8080);       /* server port */

    /* Try to have a connection with the server */
    result2 = connect(sock2, (struct sockaddr *) &echose, sizeof(echose));
    if (result2 < 0) {
        err_sys("Error connect");
    }

    fp = fopen("data_link.txt", "w+");

    while(1)
    {
        /* Read from socket */
        read(sock, &buffer[0], BUFFSIZE);
        printf("Message from client: %s\n", buffer);

        if(strcmp(buffer, CLOSECONNECTION) != 0)
        {
            time_t now = time (0);
            sTm = gmtime (&now);

            strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);

            char r[255] = "";
            strcat(r, "[INPUT]  ");
            strcat(r, buff);
            strcat(r, "- ");
            strcat(r, buffer);

            /* Write in file */
            fputs(r, fp);

            /* Write to socket 2 */
            write(sock2, buffer, strlen(buffer) + 1);

            /* Read from socket 2 */
            read(sock2, &buffer[0], BUFFSIZE);
            printf("Message from client: %s\n", buffer);

            time_t post = time (0);
            pTm = gmtime (&post);

            /* Generate random delay */
            srand(time(0));

            pTm->tm_sec += rand() % (65 +1 - 0) + 0;

            strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", pTm);

            char s[255] = "";
            strcat(s, "[OUTPUT]  ");
            strcat(s, buff);
            strcat(s, "- ");
            strcat(s, buffer);

            fputs(s, fp);

            /* Write to socket */
            write(sock, buffer, strlen(buffer) + 1);
        }

        else
        {
            /* Write to socket 2 */
            write(sock2, buffer, strlen(buffer) + 1);

            /* Read from socket 2 */
            read(sock2, &buffer[0], BUFFSIZE);
            printf("Message from client: %s\n", buffer);

            /* Write to socket */
            write(sock, buffer, strlen(buffer) + 1);
            
            closeConnection(sock, sock2);
            fclose(fp);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver, echoclient;
    int serversock, clientsock;
    int result;    

    /* Check input arguments */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip_server> <port> <file>\n", argv[0]);
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
    echoserver.sin_port = htons(atoi(argv[2]));       /* server port */

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
        handle_client(clientsock, argv[1]);
    }
}