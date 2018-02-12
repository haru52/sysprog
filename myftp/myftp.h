#define BUFSIZE 512
#define PORT_NUM "50021"
#define DATASIZE 1024

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
