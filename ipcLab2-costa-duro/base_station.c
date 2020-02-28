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

int send_int(int num, int fd)
{
    printf("%s", "Send_int");
    int32_t conv = htonl(num);
    char *data = (char*)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(fd, data, left);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable again
            }
            else if (errno != EINTR) {
                return -1;
            }
        }
        else {
            printf("%s", data);
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    return 0;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int sock, result;
    int received = 0;
    char word[255];
    int number;
    int32_t conv;

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

    send_int(5, sock);

/*
    printf("holaaaaa");
    while(1){

        printf("Enter a number: ");
        scanf("%d", &number);
        if (number < 0 && number > 9)
        {
            continue;
        }
        
        conv = htonl(number);
        char*data = (char*)&conv;
        int n = sizeof(conv);
        int rc;

        /*fgets(number, sizeof(char), stdin);

        /* Write to socket 
        rc = write(sock, data, n);
        fprintf(stdout, " sent \n");

        /* Read from socket 
        read(sock, buffer, BUFFSIZE);
        fprintf(stdout, " %s ...done \n", buffer);

        printf("Enter a word");
        fgets(word, 255, stdin);

        /* Write to socket 
        write(sock, word, strlen(word) + 1);
        fprintf(stdout, " sent \n");
    }
    printf("Adioooooss");
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