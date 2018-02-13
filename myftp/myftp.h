#define BUFSIZE 512
#define PORT_NUM "50021"
#define DATASIZE 1024

#define AVSIZE 4
#define PATHSIZE 64
#define COMMANDSIZE 128

#define QUIT 0x01
#define PWD 0x02
#define CWD 0x03
#define LIST 0x04
#define RETR 0x05
#define STOR 0x06
#define OK 0x10
#define CMDERR 0x11
#define FILEERR 0x12
#define UNKWNERR 0x13
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
