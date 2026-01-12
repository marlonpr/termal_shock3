#pragma once
#include <stdint.h>
#include <stddef.h>

void uart_link_init(void);
void uart_link_task(void *arg);
void uart_link_send_bytes(const uint8_t *data, size_t len);
