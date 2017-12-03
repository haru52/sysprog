#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT_NUM 8888

int main(void)
{
    int s, count;
    in_port_t myport;         // 自ポート
    struct sockaddr_in myskt; // 自ソケットアドレス構造体
    struct sockaddr_in skt;   // 送信側ソケットアドレス構造体
    char buf[512];            // 受信用バッファ
    socklen_t sktlen;
    char *client_ip_addr;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    myport = PORT_NUM;
    memset(&myskt, 0, sizeof myskt);
    myskt.sin_family = AF_INET;
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

        client_ip_addr = inet_ntoa(skt.sin_addr);
        printf("Message from %s\n", client_ip_addr);
        printf("%s\n", buf);

        if ((count = sendto(s, buf, sizeof buf, 0, (struct sockaddr *) &skt, sktlen)) < 0) {
            perror("sendto");
            exit(1);
        }
    }

    return 0;
}
