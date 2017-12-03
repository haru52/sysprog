#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT_NUM 8888

int main(int argc, char *argv[])
{
    int s, count, datalen;
    struct sockaddr_in skt; // 受信側ソケット
    char buf[512];          // 送信用バッファ
    in_port_t port;         // 受信側のポート番号
    struct in_addr ipaddr;  // 送信側のIPアドレス
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
        fgets(buf, sizeof buf, stdin);
        buf[strlen(buf) - 1] = '\0';
        datalen = sizeof(char) * (strlen(buf) + 1);
        port = PORT_NUM;
        inet_aton(argv[1], &ipaddr);

        memset(&skt, 0, sizeof skt);
        skt.sin_family = AF_INET;
        skt.sin_port = htons(port);
        skt.sin_addr.s_addr = ipaddr.s_addr;
        sktlen = sizeof skt;
        if ((count = sendto(s, buf, datalen, 0, (struct sockaddr *)&skt, sktlen)) < 0) {
            perror("sendto");
            exit(1);
        }

        if ((count = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }

        printf("%s\n", buf);
    } while(strcmp(buf, "FIN") != 0);

    close(s);

    return 0;
}
