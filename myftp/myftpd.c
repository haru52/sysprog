// myftpd.c
// Simple FTP server

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

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    struct sockaddr_storage sin;
    char *serv, buf[BUFSIZE];
    int sd, sd1, err, count;
    socklen_t sktlen;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: ./myftpd [current directory (option)]\n");
        exit(1);
    }
    
    // servが指す領域にサービス名(e.g, "http” or "80”)を格納しておく.
    serv = PORT_NUM;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // accept()で使用することを示す 
    hints.ai_socktype = SOCK_STREAM; // TCPを使用することを示す
    if ((err = getaddrinfo(NULL, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }
    if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    if (bind(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }
    freeaddrinfo(res);
    if (listen(sd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    sktlen = sizeof (struct sockaddr_storage);
    if ((sd1 = accept(sd, (struct sockaddr *)&sin, &sktlen)) < 0) {
        perror("accept");
        exit(1);
    }
    if ((count = recv(sd1, buf, sizeof(buf), 0)) < 0) {
        perror("recv");
        exit(1);
    }
    printf("Received message: %s\n", buf);

    return 0;
}
