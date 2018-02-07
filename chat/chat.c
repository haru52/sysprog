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

#define BUF_SIZE 512
#define PORT_NUM 49155
#define SERV_SIZE 6

int main(int argc, char *argv[])
{
    int s, count, datalen;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    char buf[BUF_SIZE];
    char sbuf[BUF_SIZE];
    socklen_t sktlen;
    in_port_t myport;
    struct in_addr ipaddr;
    struct addrinfo hints, *res;
    char *host;
    char serv[SERV_SIZE];
    int err;
    fd_set rdfds;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./chat [Destination hostname]\n");
        exit(1);
    }
    printf("Chat start!\nInput \"FIN\" if you want to close this chat.\n\n");

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    myport = PORT_NUM;
    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(myport);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }
    sktlen = sizeof(skt);

    host = argv[1];
    snprintf(serv, SERV_SIZE, "%d", PORT_NUM);
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    if ((err = getaddrinfo(host, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    while(1) {
        FD_ZERO(&rdfds);
        FD_SET(0, &rdfds);
        FD_SET(s, &rdfds);

        if (select(s + 1, &rdfds, NULL, NULL, NULL) < 0) {
            fprintf(stderr, "Error!\n");
        }

        if (FD_ISSET(0, &rdfds)) {
            fgets(sbuf, sizeof(sbuf), stdin);
            sbuf[strlen(sbuf) - 1] = '\0';
            datalen = sizeof(char) * (strlen(sbuf) + 1);
            if ((count = sendto(s, sbuf, datalen, 0, res->ai_addr, res->ai_addrlen)) < 0) {
                perror("sendto");
                exit(1);
            }
            if (strcmp(sbuf, "FIN") == 0) {
                break;
            }
        }

        if (FD_ISSET(s, &rdfds)) {
            if ((count = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
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
    close(s);
    printf("\nChat close!\n");
    return 0;
}
