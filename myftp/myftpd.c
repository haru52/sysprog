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
#include <sys/wait.h>
#include <unistd.h>
#include "myftp.h"

void quit(int sd1) {
    struct myftph pkt;
    pkt.type = OK;
    pkt.code = 0x00;
    if (send(sd1, &pkt, sizeof(pkt), 0) < 0) {
        perror("send");
        exit(1);
    }
    close(sd1);
    exit(0);
}

void stor(int sd1, struct myftph pkt)
{
    int count;
    char path[DATASIZE];
    struct myftph_data pkt_data;
    memset(path, 0, sizeof(path));

    if (recv(sd1, path, DATASIZE, 0) < 0) {
        perror("recv");
        exit(1);
    }
    path[pkt.length] = '\0';
    FILE *fp;
    if ((fp = fopen(path, "w")) == NULL) {
        perror("fopen");
        exit(1);
    }
    if ((count = recv(sd1, &pkt_data, sizeof(pkt_data), 0)) < 0) {
        perror("recv");
        exit(1);
    }
    if (fwrite(pkt_data.data, sizeof(char), pkt_data.length, fp) < pkt_data.length) {
        perror("fwrite");
        exit(1);
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    struct sockaddr_storage sin;
    char *serv;
    int sd, sd1, err, count;
    socklen_t sktlen;
    int pid;
    int status;

    if (argc != 1 && argc != 2) {
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

    while (1) {
        sktlen = sizeof (struct sockaddr_storage);
        if ((sd1 = accept(sd, (struct sockaddr *) &sin, &sktlen)) < 0) {
            perror("accept");
            exit(1);
        }

        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            struct myftph pkt;
            struct myftph_data pkt_data;

            int quit_flag = 0;
            while(1) {
                if (quit_flag)
                    break;
                while(1) {
                    if ((count = recv(sd1, &pkt, sizeof(pkt), 0)) < 0) {
                        perror("recv");
                        exit(1);
                    } else
                        break;
                }

                switch (pkt.type) {
                    case QUIT:
                        quit(sd1);
                        break;
                    case CWD:
                        break;
                    case LIST:
                        break;
                    case RETR:
                        break;
                    case STOR:
                        stor(sd1, pkt);
                        break;
                    default:
                        // Invalid type
                        break;
                }
            }
        } else {
            close(sd1);
        }
    }
    return 0;
}
