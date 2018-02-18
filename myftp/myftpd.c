// myftpd.c
// Simple FTP server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "myftp.h"

void quit(int sd1) {
    struct myftph pkt;

    pkt.type = OK;
    pkt.code = 0x00;
    if (send(sd1, &pkt, sizeof(pkt), 0) < 0) {
        perror("send");
        return;
    }
    close(sd1);
    exit(0);
}

void cwd(int sd1, struct myftph pkt)
{
    
}

void list(int sd1, struct myftph pkt)
{
    
}

void retr(int sd1, struct myftph pkt)
{
    char path[DATASIZE];
    FILE *fp;
    struct myftph_data pkt_data;

    if (recv(sd1, path, DATASIZE, 0) < 0) {
        perror("recv");
        return;
    }
    path[pkt.length] = '\0';

    if ((fp = fopen(path, "r")) == NULL) {
        perror("fopen");
        switch (errno) {
            case ENOENT:
                pkt.type = FILE_ERR;
                pkt.code = 0x00;
                break;
            case EACCES:
                pkt.type = FILE_ERR;
                pkt.code = 0x01;
                break;
            default:
                pkt.type = UNKWN_ERR;
                pkt.code = 0x05;
        }
        if (send(sd1, &pkt, sizeof(pkt), 0) < 0)
            perror("send");
        return;
    }

    pkt.type = OK;
    pkt.code = 0x01;
    if (send(sd1, &pkt, sizeof(pkt), 0) < 0) {
        perror("send");
        return;
    }

    pkt_data.type = DATA;
    while (1) {
        pkt_data.length = fread(pkt_data.data, sizeof(char), DATASIZE, fp);
        if (pkt_data.length == DATASIZE) {
            pkt_data.code = 0x01;
            if (send(sd1, &pkt_data, sizeof(pkt_data), 0) < 0) {
                perror("send");
                fclose(fp);
                return;
            }
        } else {
            pkt_data.code = 0x00;
            if (send(sd1, &pkt_data, sizeof(pkt_data), 0) < 0)
                perror("send");
            break;
        }
    }
    fclose(fp);
}

void stor(int sd1, struct myftph pkt)
{
    char path[DATASIZE];
    FILE *fp;
    struct myftph_data pkt_data;

    if (recv(sd1, path, DATASIZE, 0) < 0) {
        perror("recv");
        exit(1);
    }
    path[pkt.length] = '\0';

    if ((fp = fopen(path, "w")) == NULL) {
        perror("fopen");
        return;
    }

    pkt.type = OK;
    pkt.code = 0x02;
    if (send(sd1, &pkt, sizeof(pkt), 0) < 0) {
        perror("send");
        return;
    }

    while (1) {
        if (recv(sd1, &pkt_data, sizeof(pkt_data), 0) < 0) {
            perror("recv");
            exit(1);
        }
        if (fwrite(pkt_data.data, sizeof(char), pkt_data.length, fp) < pkt_data.length) {
            perror("fwrite");
            exit(1);
        }
        if (pkt_data.code == 0x00)
            break;
        else if (pkt_data.code == 0x01)
            continue;
        else {
            fprintf(stderr, "Error: stor: Invalid code\n");
            break;
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    struct sockaddr_storage sin;
    char *serv;
    int sd, sd1, err;
    socklen_t sktlen;
    pid_t pid;
    struct myftph pkt;

    if (argc != 1 && argc != 2) {
        fprintf(stderr, "Usage: ./myftpd [current directory (option)]\n");
        exit(1);
    }

    serv = PORT_NUM;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
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
    printf("myFTP server is running...\n");

    while (1) {
        if ((sd1 = accept(sd, (struct sockaddr *) &sin, &sktlen)) < 0) {
            perror("accept");
            exit(1);
        }
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            while (1) {
                while (1) {
                    if (recv(sd1, &pkt, sizeof(pkt), 0) < 0)
                        perror("recv");
                    else
                        break;
                }
                switch (pkt.type) {
                    case QUIT:
                        quit(sd1);
                        break;
                    case CWD:
                        cwd(sd1, pkt);
                        break;
                    case LIST:
                        list(sd1, pkt);
                        break;
                    case RETR:
                        retr(sd1, pkt);
                        break;
                    case STOR:
                        stor(sd1, pkt);
                        break;
                    default:
                        pkt.type = CMD_ERR;
                        pkt.code = 0x02;
                        if (send(sd1, &pkt, sizeof(pkt), 0) < 0)
                            perror("send");
                }
            }
        } else
            close(sd1);
    }
    return 0;
}
