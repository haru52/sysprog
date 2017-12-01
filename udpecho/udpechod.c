#include <stdio.h>
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
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define IP_ADDR "127.0.0.1"
#define PORT_NUM 8888

int main(void)
{
    int s, count;
    in_port_t myport;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    struct in_addr ipaddr;
    char buf[512];
    socklen_t sktlen;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    inet_aton(IP_ADDR, &ipaddr);
    myport = PORT_NUM;
    memset(&myskt, 0, sizeof myskt);
    myskt.sin_port = htons(myport);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr *)&myskt, sizeof myskt) < 0) {
        perror("bind");
        exit(1);
    }

    while(1) {
        sktlen = sizeof skt;
        if ((count = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }

        printf("%s\n", buf);

        if ((count = sendto(s, buf, sizeof buf, 0, (struct sockaddr *) &skt, sktlen)) < 0) {
            perror("send");
            exit(1);
        }
    }

    return 0;
}
