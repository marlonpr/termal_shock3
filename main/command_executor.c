#include "command_executor.h"

#include "protocol.h"
#include "uart_link.h"
#include "relay.h"

#include "esp_log.h"

static const char *TAG = "CMD_EXEC";

/* ================= ACK HELPER ================= */

static void send_ack(uint32_t seq, uint8_t cmd_id, uint8_t status)
{
    uint8_t tx[64];

    size_t len = protocol_build_ack_packet(
        tx,
        sizeof(tx),
        seq,
        cmd_id,
        status
    );

    if (len > 0) {
        uart_link_send_bytes(tx, len);
    }
}

/* ================= COMMAND DISPATCH ================= */

void executor_handle_packet(const packet_header_t *hdr,
                            const uint8_t *payload)
{
    if (!hdr || !payload) {
        return;
    }

    if (hdr->length != sizeof(payload_command_t)) {
        ESP_LOGW(TAG, "Invalid command payload size");
        send_ack(hdr->sequence, 0xFF, ACK_ERR_INVALID);
        return;
    }

    const payload_command_t *cmd =
        (const payload_command_t *)payload;

    ESP_LOGI(TAG,
             "CMD RX id=0x%02X p16=0x%04X p32=%lu seq=%lu",
             cmd->cmd_id,
             cmd->param16,
             cmd->param32,
             hdr->sequence);

    uint8_t status = ACK_OK;

    switch (cmd->cmd_id) {

    /* ================= RELAY CONTROL ================= */

    case CMD_FORCE_RELAY:
        /*
         * param16 bitmask:
         *  bit0 = HOT
         *  bit1 = COLD
         */
        relay_force(cmd->param16);
        break;

    /* ================= SYSTEM ================= */

    case CMD_STOP_TEST:
        /* Safety: STOP always forces relays OFF */
        relay_all_off();
        break;

    case CMD_REQUEST_STATE:
        /* Actuator has no state machine; ACK only */
        break;

    /* ================= UNSUPPORTED ================= */

    case CMD_START_TEST:
    case CMD_SET_MODE:
    case CMD_SYNC_TIME:
        ESP_LOGW(TAG, "Command not supported on actuator");
        status = ACK_ERR_INVALID;
        break;

    default:
        ESP_LOGW(TAG, "Unknown command 0x%02X", cmd->cmd_id);
        status = ACK_ERR_UNKNOWN;
        break;
    }

    send_ack(hdr->sequence, cmd->cmd_id, status);
}
