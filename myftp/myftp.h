// myftp.h
// myFTP header

#define DATASIZE 1024
#define COMMANDSIZE 128
#define AVSIZE 4
#define PATHSIZE 64
#define FILE_NAME_SIZE 64
#define PORT_NUM "50021"

// myFTP packet types
#define QUIT 0x01
#define PWD 0x02
#define CWD 0x03
#define LIST 0x04
#define RETR 0x05
#define STOR 0x06
#define OK 0x10
#define CMD_ERR 0x11
#define FILE_ERR 0x12
#define UNKWN_ERR 0x13
#define DATA 0x20

#define HELP_TEXT "\
quit: myftpcの終了\n\
pwd:  サーバでのカレントディレクトリの表示\n\
cd:   サーバでのカレントディレクトリの移動\n\
dir:  サーバに存在するファイル情報の取得\n\
lpwd: クライアントでのカレントディレクトリの表示\n\
lcd:  クライアントでのカレントディレクトリの移動\n\
ldir: クライアントに存在するファイル情報の取得\n\
get:  サーバからクライアントへのファイル転送\n\
put:  クライアントからサーバへのファイル転送\n\
help: ヘルプメッセージの表示\n\
\n\
Usage:\n\
quit\n\
pwd\n\
cd [path]\n\
dir [path (option)]\n\
lpwd\n\
lcd [path]\n\
ldir [path (option)]\n\
get [path 1] [path 2 (option)]\n\
put [path 1] [path 2 (option)]\n\
help"

struct myftph_data {
    uint8_t  type;
    uint8_t  code;
    uint16_t length;
    char     data[DATASIZE];
};

struct myftph {
    uint8_t  type;
    uint8_t  code;
    uint16_t length;
};
