
#include <stdint.h>
#include <stdbool.h>


#define UART_BAUD 115200
#define MAGIC_BYTE '%'
#define MAX_PACKET_LENGTH 4096
typedef enum {
    DECODE_MSG = 'D',
    SUBSCRIBE_MSG = 'S',
    LIST_MSG = 'L',
    ACK_MSG = 'A',
    ERROR_MSG = 'E',
    DEBUG_MSG = 'G'
} opcode_t;

typedef struct {
    uint8_t magic;
    uint8_t opcode;
    uint16_t length;
} pkt_header;

typedef struct {
    pkt_header header;
    uint8_t body[];
} pkt;

int uart_readbyte(void);
void uart_flush(void);
void uart_writebyte(uint8_t data);
int uart_init(void);
int read_bytes(uint8_t *buff, uint16_t len);
void read_header(pkt_header* header);
int read_ack(void);
int write_bytes(uint8_t* bytes, uint16_t len, bool need_ack);
int write_packet(opcode_t cmd, uint8_t* body, uint16_t len);
int read_packet(opcode_t* cmd, uint16_t* len, uint8_t* body);

int list_subscriptions(void); // returns SUCCESS on success, ERROR on error
int update_subscription(pkt* pkt); // returns SUCCESS on success, ERROR on error
int decode_frame(pkt* pkt); // returns SUCCESS on success, ERROR on error