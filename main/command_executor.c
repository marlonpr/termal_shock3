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
	
		case CMD_FORCE_RELAY:
		    send_ack(hdr->sequence, cmd->cmd_id, ACK_OK);
		    relay_force(cmd->param16);
		    return;
		
		case CMD_STOP_TEST:
		    send_ack(hdr->sequence, cmd->cmd_id, ACK_OK);
		    relay_all_off();
		    return;
		
		case CMD_UART_TEST:
		    ESP_LOGI(TAG, "UART TEST RX seq=%lu", hdr->sequence);
		    send_ack(hdr->sequence, cmd->cmd_id, ACK_OK);
		    return;
		
		default:
		    status = ACK_ERR_INVALID;
		    break;
	}
	
	/* fallback ACK */
	send_ack(hdr->sequence, cmd->cmd_id, status);

}
