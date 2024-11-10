#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include <ctype.h>
#include <string.h>

typedef enum {
  ProbeRequest,
  ProbeResponse
} FrameType;

typedef struct {
  FrameType type;
  unsigned int len;

  char buf[1024];
} DataPacket;

// pretend this receives a raw packet from the network
void wifi_recv(DataPacket* pkt) {
  pkt->type = getchar();
  scanf("%d", &pkt->len);
  fgets(pkt->buf, pkt->len, stdin);
}

void wifi_process_response(DataPacket* pkt) {
  char buf[1024];
  memset(buf, 0, sizeof(buf));

  if (pkt->type == ProbeRequest) {
    // not implemented yet
    printf("%s(): probe request handler not implemented yet!\n", __func__);
    return;
  } else if (pkt->type == ProbeResponse) {
    for (int i = 0; i < pkt->len; i++) {
      buf[i] = pkt->buf[i];
    }
    printf("%s(): copied into buf!\n", __func__);

    // this is just so the compiler doesn't optimize out `buf`
    printf("%s(): buf[0]=%d", buf[0]);

    return;
  }
}

int main() {
  bi_decl(bi_program_description("Fuzzing Example"));
  stdio_init_all();

  DataPacket pkt;
  wifi_recv(&pkt);
  wifi_process_response(&pkt);

  return 0;
}