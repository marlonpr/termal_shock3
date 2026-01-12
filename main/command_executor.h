#pragma once

#include <stdint.h>
#include "protocol.h"

/* Entry point from UART RX */
void executor_handle_packet(const packet_header_t *hdr,
                            const uint8_t *payload);
