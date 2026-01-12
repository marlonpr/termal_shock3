#include "uart_link.h"
#include "protocol.h"
#include "command_executor.h"
#include "board_pins.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "UART_LINK";

#define RX_BUF_SIZE 256

static uint8_t rx_buf[RX_BUF_SIZE];
static size_t  rx_len = 0;

#include "crc16.h"

static void process_rx_buffer(void)
{
    while (rx_len >= sizeof(packet_header_t) + 2) {

        packet_header_t hdr;
        memcpy(&hdr, rx_buf, sizeof(hdr));

        /* 1. Magic check */
        if (hdr.magic != PACKET_MAGIC) {
            memmove(rx_buf, rx_buf + 1, --rx_len);
            continue;
        }

        /* 2. Version check */
        if (hdr.version != PROTOCOL_VERSION) {
            ESP_LOGW(TAG, "Protocol version mismatch");
            memmove(rx_buf, rx_buf + 1, --rx_len);
            continue;
        }

        size_t total_len =
            sizeof(packet_header_t) + hdr.length + 2; // + CRC16

        /* 3. Wait for full packet */
        if (rx_len < total_len) {
            return;
        }

        /* 4. CRC check */
        uint16_t crc_rx =
            (rx_buf[total_len - 2] << 8) |
             rx_buf[total_len - 1];

        uint16_t crc_calc =
            crc16_ccitt(rx_buf,
                        total_len - 2,
                        0);

        if (crc_calc != crc_rx) {
            ESP_LOGW(TAG,
                     "CRC error seq=%lu type=0x%02X calc=0x%04X rx=0x%04X",
                     hdr.sequence, hdr.type, crc_calc, crc_rx);

            /* Drop one byte and resync */
            memmove(rx_buf, rx_buf + 1, --rx_len);
            continue;
        }

        /* 5. Dispatch packet */
        const uint8_t *payload =
            rx_buf + sizeof(packet_header_t);

        switch (hdr.type) {

        case PKT_COMMAND:
            executor_handle_packet(&hdr, payload);
            break;

        default:
            ESP_LOGW(TAG,
                     "Unhandled packet type: 0x%02X",
                     hdr.type);
            break;
        }

        /* 6. Remove processed packet */
        memmove(rx_buf,
                rx_buf + total_len,
                rx_len - total_len);

        rx_len -= total_len;
    }
}


void uart_link_init(void)
{
    uart_config_t cfg = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(
        uart_driver_install(
            UART_RELAY,
            2048,
            0,
            0,
            NULL,
            0
        )
    );

    ESP_ERROR_CHECK(uart_param_config(UART_RELAY, &cfg));

    ESP_ERROR_CHECK(
        uart_set_pin(
            UART_RELAY,
            PIN_UART_RELAY_TX,
            PIN_UART_RELAY_RX,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE
        )
    );

    ESP_LOGI(TAG, "UART RX initialized");
}

void uart_link_send_bytes(const uint8_t *data, size_t len)
{
    if (!data || !len) return;
    uart_write_bytes(UART_RELAY, (const char *)data, len);
}

void uart_link_task(void *arg)
{
    uint8_t tmp[64];

    while (1) {
        int r = uart_read_bytes(
            UART_RELAY,
            tmp,
            sizeof(tmp),
            pdMS_TO_TICKS(100)
        );

        if (r > 0) {
            if (rx_len + r > RX_BUF_SIZE) {
                ESP_LOGW(TAG, "RX overflow, clearing buffer");
                rx_len = 0;
            }

            memcpy(rx_buf + rx_len, tmp, r);
            rx_len += r;

            process_rx_buffer();   // <-- THIS is correct
        }
    }
}

