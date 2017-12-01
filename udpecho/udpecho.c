#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define PORT_NUM 8888

int main(int argc, char *argv[])
{
    int s, count, datalen;
    struct sockaddr_in skt;
    char sbuf[512];
    in_port_t port;
    struct in_addr ipaddr;
    socklen_t sktlen;

    if (argc != 2) {
        fprintf(stderr, "Usage: udpecho [server IP address]\n");
        exit(1);
    }

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    do {
        printf("Input message: ");
        fgets(sbuf, sizeof sbuf, stdin);
        sbuf[strlen(sbuf) - 1] = '\0';
        datalen = sizeof(char) * (strlen(sbuf) + 1);
        inet_aton(argv[1], &ipaddr);
        port = PORT_NUM;
        memset(&skt, 0, sizeof skt);
        skt.sin_port = htons(port);
        skt.sin_addr.s_addr = ipaddr.s_addr;

        sktlen = sizeof skt;
        if ((count = sendto(s, sbuf, datalen, 0, (struct sockaddr *)&skt, sizeof skt)) < 0) {
            perror("sendto");
            exit(1);
        }

        if ((count = recvfrom(s, sbuf, sizeof sbuf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }

        printf("%s\n", sbuf);
    } while(strcmp(sbuf, "FIN") != 0);

    close(s);

    return 0;
}
