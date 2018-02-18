// myftpc.c
// Simple FTP client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "myftp.h"

void print_error_message(uint8_t type, uint8_t code) {
    if (type == CMD_ERR && code == 0x01)
        fprintf(stderr, "エラー: 構文エラー\n");
    else if (type == CMD_ERR && code == 0x02)
        fprintf(stderr, "エラー: 未定義コマンド\n");
    else if (type == CMD_ERR && code == 0x03)
        fprintf(stderr, "エラー: プロトコルエラー\n");
    else if (type == FILE_ERR && code == 0x00)
        fprintf(stderr, "エラー: ファイル/ディレクトリが存在しない\n");
    else if (type == FILE_ERR && code == 0x01)
        fprintf(stderr, "エラー: ファイル/ディレクトリのアクセス権限がない\n");
    else if (type == UNKWN_ERR && code == 0x05)
        fprintf(stderr, "エラー: 未定義のエラー\n");
    else
        fprintf(stderr, "Unexpected error\n");
}

void quit(int sd) {
    struct myftph pkt;

    pkt.type = QUIT;
    if (send(sd, &pkt, sizeof(pkt), 0) < 0) {
        perror("send");
        exit(1);
    }
    if (recv(sd, &pkt, sizeof(pkt), 0) < 0) {
        perror("recv");
        exit(1);
    }
    if (pkt.type == OK && pkt.code == 0x00) {
        close(sd);
        exit(0);
    } else {
        fprintf(stderr, "Error: quit\n");
        exit(1);
    }
}

void pwd(int sd) {}

void cd(int sd, char *path) {}

void dir(int sd, char *path) {}

void lpwd(void)
{
    char path[PATHSIZE];

    memset(path, 0, PATHSIZE);
    if (getcwd(path, PATHSIZE) == NULL) {
        perror("lpwd");
        return;
    }
    printf("%s\n", path);
}

void lcd(char *path)
{
    if (chdir(path) == -1)
        perror("lcd");
}

void ldir(char *path)
{
    struct stat st;
    DIR *dir;
    struct dirent *dp;

    if (stat(path, &st) == -1) {
        perror("ldir");
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        dir = opendir(path);
        for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
            if (dp->d_name[0] != '.')
                printf("%s\n", dp->d_name);
        }
        closedir(dir);
    } else
        printf("%s\n", path);
}

void get(int sd, char *path1, char *path2)
{
    FILE *fp;
    struct myftph pkt;
    struct myftph_data pkt_data;

    pkt_data.type = RETR;
    pkt_data.length = strlen(path1);
    strcpy(pkt_data.data, path1);
    if (send(sd, &pkt_data, sizeof(pkt_data), 0) < 0) {
        perror("send");
        exit(1);
    }

    if (recv(sd, &pkt, sizeof(pkt), 0) < 0) {
        perror("recv");
        exit(1);
    }
    if (pkt.type != OK || pkt.code != 0x01) {
        print_error_message(pkt.type, pkt.code);
        return;
    }

    if ((fp = fopen(path2, "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    while (1) {
        if (recv(sd, &pkt_data, sizeof(pkt_data), 0) < 0) {
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
            fprintf(stderr, "Error: put: Invalid code\n");
            break;
        }
    }
    fclose(fp);
}

void put(int sd, char *path1, char *path2)
{
    FILE *fp;
    struct myftph pkt;
    struct myftph_data pkt_data;

    if ((fp = fopen(path2, "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    pkt_data.type = STOR;
    pkt_data.length = strlen(path1);
    strcpy(pkt_data.data, path1);
    if (send(sd, &pkt_data, sizeof(pkt_data), 0) < 0) {
        perror("send");
        exit(1);
    }

    if (recv(sd, &pkt, sizeof(pkt), 0) < 0) {
        perror("recv");
        exit(1);
    }
    if (pkt.type != OK || pkt.code != 0x01) {
        print_error_message(pkt.type, pkt.code);
        return;
    }

    while (1) {
        if (recv(sd, &pkt_data, sizeof(pkt_data), 0) < 0) {
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
    return;
}

void help(void)
{
    printf("%s\n", HELP_TEXT);
}

void getargv(char *command, char *av[], int *nargs)
{
    char *p = command;
    *nargs = 0;
    while (*nargs <= AVSIZE) {
        if (*p == '\n') {
            return;
        } else if (*p == ' ' || *p == '\t') {
            p++;
        } else {
            av[*nargs] = p;
            *nargs += 1;
            p++;
            while (1) {
                if (*p == '\n') {
                    *p = '\0';
                    return;
                } else if (*p == ' ' || *p == '\t') {
                    *p = '\0';
                    p++;
                    break;
                } else {
                    p++;
                }
            }
        }
    }
}

void ftp_proc(int sd)
{
    char command[COMMANDSIZE];
    char *av[AVSIZE];
    int nargs;
    char *path2;

    fgets(command, sizeof(command), stdin);
    getargv(command, av, &nargs);

    if (strcmp(av[0], "quit") == 0) {
        quit(sd);
    } else if (strcmp(av[0], "pwd") == 0) {
        // pwd(sd);
    } else if (strcmp(av[0], "cd") == 0) {
        // cd(sd, av[1]);
    } else if (strcmp(av[0], "dir") == 0) {
        // dir(sd, av[1]);
    } else if (strcmp(av[0], "lpwd") == 0) {
        if (nargs != 1) {
            fprintf (stderr, "Error: Invalid argument\nUsage: lpwd\n");
            return;
        }
        lpwd();
    } else if (strcmp(av[0], "lcd") == 0) {
        if (nargs != 2) {
            fprintf (stderr, "Error: Invalid argument\nUsage: lcd [path]\n");
            return;
        }
        lcd(av[1]);
    } else if (strcmp(av[0], "ldir") == 0) {
        if (nargs != 1 && nargs != 2) {
            fprintf (stderr, "Error: Invalid argument\nUsage: ldir [path (option)]\n");
            return;
        }
        if (nargs == 1) {
            char path[PATHSIZE];
            memset(path, 0, PATHSIZE);
            if (getcwd(path, PATHSIZE) == NULL)
                perror("ldir");
            ldir(path);
        } else
            ldir(av[1]);
    } else if (strcmp(av[0], "get") == 0) {
        if (nargs != 2 && nargs != 3) {
            fprintf (stderr, "Error: Invalid argument\nUsage: get [path 1] [path 2 (option)]\n");
            return;
        }
        if (nargs == 2) {
            path2 = (char *) malloc(strlen(av[1]) + 1);
            strcpy(path2, av[1]);
            get(sd, av[1], path2);
        } else
            get(sd, av[1], av[2]);
    } else if (strcmp(av[0], "put") == 0) {
        if (nargs != 2 && nargs != 3) {
            fprintf (stderr, "Error: Invalid argument\nUsage: put [path 1] [path 2 (option)]\n");
            return;
        }
        if (nargs == 2) {
            path2 = (char *) malloc(strlen(av[1]) + 1);
            strcpy(path2, av[1]);
            put(sd, av[1], path2);
        } else
            put(sd, av[1], av[2]);
    } else if (strcmp(av[0], "help") == 0) {
        help();
    } else {
        fprintf(stderr, "Error: Invalid command\n");
    }
}

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    char *host, *serv;
    int sd, err;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./myftpc [server hostname]\n");
        exit(1);
    }

    host = argv[1];
    serv = PORT_NUM;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

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

    while (1) {
        printf("myFTP%% ");
        ftp_proc(sd);
    }

    return 0;
}
