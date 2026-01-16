#include "relay.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "RELAY";

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

static uint8_t hot_mask  = 0;   /* bit0..bit3 → HOT relays */
static uint8_t cold_mask = 0;   /* bit0..bit3 → COLD relays */

/* ============================================================
 * INTERNAL HELPERS
 * ============================================================ */

void apply_outputs(void)
{
    for (int i = 0; i < RELAY_HOT_COUNT; i++) {
        gpio_set_level(
            RELAY_HOT_PINS[i],
            (hot_mask & (1 << i)) ? 1 : 0
        );
    }

    for (int i = 0; i < RELAY_COLD_COUNT; i++) {
        gpio_set_level(
            RELAY_COLD_PINS[i],
            (cold_mask & (1 << i)) ? 1 : 0
        );
    }
}

static inline bool floats_safe(void)
{
    /*
     * Float switches are enforced by MASTER.
     * Actuator trusts MASTER commands.
     * Hook local GPIO safety here if needed.
     */
    return true;
}

/* ============================================================
 * PUBLIC API
 * ============================================================ */

void relay_init(void)
{
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    uint64_t mask = 0;

    for (int i = 0; i < RELAY_HOT_COUNT; i++) {
        mask |= (1ULL << RELAY_HOT_PINS[i]);
    }

    for (int i = 0; i < RELAY_COLD_COUNT; i++) {
        mask |= (1ULL << RELAY_COLD_PINS[i]);
    }

    io.pin_bit_mask = mask;
    gpio_config(&io);

    relay_all_off();

    ESP_LOGI(TAG, "Relay driver initialized (HOT=%d COLD=%d)",
             RELAY_HOT_COUNT, RELAY_COLD_COUNT);
}

void relay_all_off(void)
{
    hot_mask  = 0;
    cold_mask = 0;
    apply_outputs();

    ESP_LOGW(TAG, "All relays OFF");
}
void relay_force(uint16_t mask)
{
    /*
     * mask bits:
     *   [7:0]   HOT relays
     *   [15:8]  COLD relays
     */

    uint8_t req_hot  =  mask        & 0xFF;
    uint8_t req_cold = (mask >> 8)  & 0xFF;

    if (!floats_safe()) {
        ESP_LOGE(TAG, "Float unsafe → RELAYS OFF");
        relay_all_off();
        return;
    }

    /* HARD INTERLOCK */
    if (req_hot && req_cold) {
        ESP_LOGE(TAG, "Interlock violation (HOT & COLD) → RELAYS OFF");
        relay_all_off();
        return;
    }

    hot_mask  = req_hot;
    cold_mask = req_cold;

    apply_outputs();

    ESP_LOGI(TAG,
             "Relay state: HOT=0x%X COLD=0x%X",
             hot_mask, cold_mask);
}


uint8_t relay_hot_mask(void)
{
    return hot_mask;
}

uint8_t relay_cold_mask(void)
{
    return cold_mask;
}

