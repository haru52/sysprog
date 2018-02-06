// chat.c
// UDP，select()を用いたチャットプログラム

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUF_SIZE 512
#define PORT_NUM 49155

int main(void)
{
    int s;                    // ソケット記述子
    int count, datalen;
    struct sockaddr_in myskt; // 受信用ソケットアドレス構造体
    struct sockaddr_in skt;   // 送信用ソケットアドレス構造体
    char buf[BUF_SIZE];       // 受信用バッファ
    char sbuf[BUF_SIZE];      // 送信用バッファ
    in_port_t port;           // ポート番号
    struct in_addr ipaddr;    // 送信用IPアドレス
    fd_set rdfds;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0) < 0) {
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

    FD_ZERO(&rdfds);
    FD_SET(0, &rdfds);
    FD_SET(s, &rdfds);

    if (select(s + 1, &rdfds, NULL, NULL, NULL) < 0) {
        // エラー処理
        fprintf(stderr, "Error!\n");
    }

    if (FD_ISSET(0, &rdfds)) {
        // 標準入力から入力
        fgets(sbuf, sizeof(sbuf), stdin);
        buf[strlen(buf) - 1] = '\0';
        datalen = sizeof(char) * (strlen(buf) + 1);
        inet_aton(argv[1], &ipaddr);

        skt.sin_family = AF_INET;
        skt.sin_port = htons(port);
        skt.sin_addr.s_addr = htonl(ipaddr.s_addr);
        if ((count = sendto(s, sbuf, datalen, 0, (struct sockaddr *)&skt, sizeof skt)) < 0) {
            perror("sendto");
            exit(1);
        }
    }

    if (FD_ISSET(s, &rdfds)) {
        // パケット受信
        // recvfrom & printf
        sktlen = sizeof(skt);
        if ((count = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        printf("%s\n", buf);
    }
}
