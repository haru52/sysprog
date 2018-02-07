// chat.c
// UDP chat program with select()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 512
#define PORT_NUM "49155"

int main(int argc, char *argv[])
{
    struct addrinfo myhints, hints, *myres, *res;
    struct sockaddr_storage sin;
    char *host, *serv, buf[BUFSIZE], sbuf[BUFSIZE];
    int sd, err, count, datalen;
    socklen_t sktlen;
    fd_set rdfds;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./chat [Destination hostname]\n");
        exit(1);
    }
    printf("Chat start!\nInput \"FIN\" if you want to close this chat.\n\n");

    serv = PORT_NUM;
    memset(&myhints, 0, sizeof(myhints));
    myhints.ai_flags = AI_PASSIVE;
    myhints.ai_socktype = SOCK_DGRAM;
    if ((err = getaddrinfo(NULL, serv, &myhints, &myres)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }
    if ((sd = socket(myres->ai_family, myres->ai_socktype, myres->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    if (bind(sd, myres->ai_addr, myres->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }
    freeaddrinfo(myres);
    sktlen = sizeof(sin);

    host = argv[1];
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    if ((err = getaddrinfo(host, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    while(1) {
        FD_ZERO(&rdfds);
        FD_SET(0, &rdfds);
        FD_SET(sd, &rdfds);

        if (select(sd + 1, &rdfds, NULL, NULL, NULL) < 0) {
            fprintf(stderr, "Error!\n");
        }

        if (FD_ISSET(0, &rdfds)) {
            fgets(sbuf, sizeof(sbuf), stdin);
            sbuf[strlen(sbuf) - 1] = '\0';
            datalen = sizeof(char) * (strlen(sbuf) + 1);
            if ((count = sendto(sd, sbuf, datalen, 0, res->ai_addr, res->ai_addrlen)) < 0) {
                perror("sendto");
                exit(1);
            }
            if (strcmp(sbuf, "FIN") == 0) {
                break;
            }
        }

        if (FD_ISSET(sd, &rdfds)) {
            if ((count = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &sktlen)) < 0) {
                perror("recvfrom");
                exit(1);
            }
            printf("%s: %s\n", host, buf);
            if (strcmp(buf, "FIN") == 0) {
                break;
            }
        }
    }
    freeaddrinfo(res);
    close(sd);
    printf("\nChat close!\n");
    return 0;
}
