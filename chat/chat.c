// chat.c
// UDP chat program with select()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 512
#define PORT_NUM 49155

int main(int argc, char *argv[])
{
    int s;
    int count, datalen;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    char buf[BUF_SIZE];
    char sbuf[BUF_SIZE];
    in_port_t port;
    struct in_addr ipaddr;
    fd_set rdfds;

    if (argc != 2) {
      fprintf(stderr, "Usage: ./chat [Destination IP address]\n");
      exit(1);
    }
    printf("Chat start!\n");

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }
    port = PORT_NUM;
    memset(&myskt, 0, sizeof myskt);
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(port);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
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
            memset(sbuf, 0, sizeof(sbuf));
            fgets(sbuf, sizeof(sbuf), stdin);
            sbuf[strlen(sbuf) - 1] = '\0';
            datalen = sizeof(char) * (strlen(sbuf) + 1);
            inet_aton(argv[1], &ipaddr);
            skt.sin_family = AF_INET;
            skt.sin_port = htons(port);
            skt.sin_addr.s_addr = ipaddr.s_addr;
            if ((count = sendto(s, sbuf, datalen, 0, (struct sockaddr *)&skt, sizeof(skt))) < 0) {
                perror("sendto");
                exit(1);
            }
            if (strcmp(sbuf, "FIN") == 0) {
                break;
            }
        }

        if (FD_ISSET(s, &rdfds)) {
            memset(buf, 0, sizeof(buf));
            socklen_t sktlen = sizeof(skt);
            if ((count = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
                perror("recvfrom");
                exit(1);
            }
            printf("%s: %s\n", inet_ntoa(skt.sin_addr), buf);
            if (strcmp(buf, "FIN") == 0) {
                break;
            }
        }
    }
    close(s);
    printf("Chat close!\n");
    return 0;
}
