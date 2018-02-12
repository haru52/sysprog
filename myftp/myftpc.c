// myftpc.c
// Simple FTP client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "myftp.h"

int get(int sd, char *filepath) {
    return 0;
}

int put(int sd, char *filepath) {
    return 0;
}


int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    char *host, *serv, buf[BUFSIZE];
    int sd, err, count;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./myftpc [server hostname]\n");
        exit(1);
    }

    // hostが指す領域にホスト名(e.g., "www.ics.keio.ac.jp" or
    // "131.113.126.136")を格納しておく.
    // servが指す領域にサービス名(e.g, "http" or "80")を格納しておく.
    host = argv[1];
    serv = PORT_NUM;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM; // TCPを使用

    if ((err = getaddrinfo(host, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }

    if (connect(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(1);
    }

    // buf[]に送信するデータを格納する.
    strcpy(buf, "Hello, world!");
    if ((count = send(sd, buf, sizeof(buf), 0)) < 0) {
        perror("send");
        exit(1);
    }

    return 0;
}
