#ifndef __app_H
#define __app_H

#include <stdbool.h>

#include "crc.h"
#include "usart.h"
#include "gpio.h"

#include "stm32l0538_discovery_epd.h"

#define PICTURE_PACKET_LEN  9
#define PICTURE_HEIGHT      72
#define PICTURE_WIDTH       172
#define PICTURE_SIZE        (PICTURE_HEIGHT*PICTURE_WIDTH/8)

#define RX_BUFFER   10
#define RX_TIMEOUT  5000UL

#define CMD_START   0x80
#define CMD_END     0x81

typedef enum
{
    IDLE,
    READ,
    CHECK,
    DRAW
} app_state;

void app_init(void);
void app_periodic(void);
#endif /*__ __app_H */
