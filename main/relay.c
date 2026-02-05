#include "relay.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "RELAY";

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */
 
 
 static const uint32_t relay_on_delay_ms[RELAY_SENSOR_COUNT] = {
    [RELAY_FLOAT1_A] = 50,   // 0.5 s
    [RELAY_FLOAT1_B] = 50,  // 1.0 s
    [RELAY_FLOAT2]   = 50,   // 2.0 s
    [RELAY_FLOAT3]   = 1000   // 2.0 s
    
};
 
 
 
#include "esp_timer.h"

typedef struct {
    bool     current;      // applied output state
    bool     pending;      // requested state
    int64_t  change_ts;    // request timestamp (µs)
    bool     armed;        // delay active
} relay_delay_t;

static relay_delay_t relay_state[RELAY_SENSOR_COUNT];


 

 

#define RELAY_SELF_TEST_DELAY_MS  1000

static bool     self_test_active = false;
static uint8_t  self_test_index  = 0;
static uint32_t self_test_ts_ms  = 0;



static uint8_t mask_relays   = 0;   // relays 0..7
static uint8_t sensor_relays = 0;   // relays 8..11


/* ============================================================
 * INTERNAL HELPERS
 * ============================================================ */

static void apply_outputs(void)
{
    /* Mask-controlled relays */
    for (int i = 0; i < RELAY_MASK_COUNT; i++) {
        gpio_set_level(
            RELAY_PINS[i],
            (mask_relays & (1 << i)) ? 1 : 0
        );
    }

    /* Sensor-controlled relays */
    for (int i = 0; i < RELAY_SENSOR_COUNT; i++) {
        gpio_set_level(
            RELAY_PINS[RELAY_MASK_COUNT + i],
            (sensor_relays & (1 << i)) ? 1 : 0
        );
    }
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

    uint64_t pin_mask = 0;

    for (int i = 0; i < RELAY_COUNT; i++) {
        pin_mask |= (1ULL << RELAY_PINS[i]);
    }

    io.pin_bit_mask = pin_mask;
    gpio_config(&io);

    relay_all_off();

    ESP_LOGI(TAG, "Relay driver initialized (%d relays)", RELAY_COUNT);
}

void relay_all_off(void)
{
    //mask_relays = 0; //0x02
    mask_relays = 0x02;
    apply_outputs();
    ESP_LOGI(TAG, "Mask relays = 0x%02X", mask_relays);

    ESP_LOGW(TAG, "All relays OFF");
}

void relay_force(uint16_t mask)
{
    /* Only lower 8 bits are valid */
    mask_relays = mask & 0xFF;
    
    if (mask_relays == 0)
    {
		mask_relays = 0x02;
	}

    apply_outputs();

    ESP_LOGI(TAG, "Mask relays FORCE = 0x%02X", mask_relays);
}


uint8_t relay_get_mask(void)
{
    return mask_relays;
}


static uint32_t now_ms(void)
{
    return esp_log_timestamp();   // milliseconds since boot
}

void relay_self_test_start(void)
{
    ESP_LOGW(TAG, "Relay self-test START");

    relay_all_off();

    self_test_active = true;
    self_test_index  = 0;
    self_test_ts_ms  = now_ms();

    /* Turn ON first relay immediately */
    relay_force(1U << self_test_index);

    ESP_LOGI(TAG, "Self-test relay %u ON (mask=0x%04X)",
             self_test_index, (1U << self_test_index));
}


void relay_self_test_tick(void)
{
    if (!self_test_active) {
        return;
    }

    if ((now_ms() - self_test_ts_ms) < RELAY_SELF_TEST_DELAY_MS) {
        return;
    }

    /* Turn current relay OFF */
    relay_all_off();

    /* Move to next relay */
    self_test_index++;

    if (self_test_index >= RELAY_COUNT) {
        ESP_LOGW(TAG, "Relay self-test COMPLETE");
        self_test_active = false;
        return;
    }

    /* Turn next relay ON */
    uint16_t mask = (1U << self_test_index);
    relay_force(mask);

    ESP_LOGI(TAG, "Self-test relay %u ON (mask=0x%04X)",
             self_test_index, mask);

    self_test_ts_ms = now_ms();
}


bool relay_self_test_active(void)
{
    return self_test_active;
}















void relay_sensor_request(uint8_t index, bool on)
{
    if (index >= RELAY_SENSOR_COUNT) {
        ESP_LOGE(TAG, "Invalid relay index");
        return;
    }

    relay_delay_t *r = &relay_state[index];

    /* OFF → immediate */
    if (!on) {
        r->pending = false;
        r->armed   = false;

        if (r->current) {
            r->current = false;
            sensor_relays &= ~(1 << index);
            apply_outputs();
        }
        return;
    }

    /* ON → delayed */
    if (!r->current && !r->armed) {
        r->pending   = true;
        r->change_ts = esp_timer_get_time();
        r->armed     = true;
    }
}







void relay_process(void)
{
    int64_t now = esp_timer_get_time();

    for (uint8_t i = 0; i < RELAY_SENSOR_COUNT; i++) {

        relay_delay_t *r = &relay_state[i];

        if (!r->armed)
            continue;

        if ((now - r->change_ts) >=
            ((int64_t)relay_on_delay_ms[i] * 1000)) {

            r->armed   = false;
            r->current = true;

            sensor_relays |= (1 << i);
            apply_outputs();
        }
    }
}




void sensors_update(bool float1_ok, bool float2_ok)
{
    relay_sensor_request(RELAY_FLOAT1_A, !float1_ok);
    relay_sensor_request(RELAY_FLOAT1_B, !float1_ok);
    relay_sensor_request(RELAY_FLOAT2,    float2_ok);
}







/*
void relay_sensor_set(uint8_t index, bool on)
{
    if (index >= RELAY_SENSOR_COUNT) {
        ESP_LOGE(TAG, "Invalid sensor relay index");
        return;
    }

    if (on) {
        sensor_relays |=  (1 << index);
    } else {
        sensor_relays &= ~(1 << index);
    }

    apply_outputs();
}



void sensors_update(bool float1_ok, bool float2_ok)
{
    relay_sensor_set(RELAY_FLOAT1_A, !float1_ok);
    relay_sensor_set(RELAY_FLOAT1_B, !float1_ok);
    relay_sensor_set(RELAY_FLOAT2,   float2_ok);
}




*/

