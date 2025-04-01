

#ifndef _MW1268_UART_H
#define _MW1268_UART_H

#include "sys.h"


uint8_t *lora_check_cmd(uint8_t *str);
uint8_t lora_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime);


#endif
