#define PORT_NUM "50021"
#define DATASIZE 1024
#define COMMANDSIZE 128
#define AVSIZE 4

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
