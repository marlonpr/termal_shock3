#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "board_pins.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sensor.h"
#include "relay.h"



/*
// ============================================================
 * FLOAT SENSORS
 * ============================================================ */

typedef struct {
    gpio_num_t gpio;
    const char *name;
    int last_state;   // raw GPIO level: 0 or 1
} float_sensor_t;

static float_sensor_t sensors[] = {
    { PIN_FLOAT_1, "Tank_High", -1 },
    { PIN_FLOAT_2, "Tank_Low",  -1 }
};

#define SENSOR_COUNT (sizeof(sensors) / sizeof(sensors[0]))

void float_sensors_init(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    for (int i = 0; i < SENSOR_COUNT; i++) {
        io_conf.pin_bit_mask = (1ULL << sensors[i].gpio);
        gpio_config(&io_conf);
    }
}

static int read_debounced(gpio_num_t gpio, int samples, int delay_ms)
{
    int state = gpio_get_level(gpio);

    for (int i = 0; i < samples; i++) {
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        if (gpio_get_level(gpio) != state) {
            return -1;  // unstable
        }
    }
    return state;       // stable 0 or 1
}

static bool float1_ok = false;
static bool float2_ok = false;

static void scan_float_sensors(void)
{
    for (int i = 0; i < SENSOR_COUNT; i++) {

        int state = read_debounced(sensors[i].gpio, 5, 10);
        if (state < 0) {
            continue;  // ignore unstable
        }

        if (state != sensors[i].last_state) {
            sensors[i].last_state = state;

            ESP_LOGI("FLOAT", "%s: %s",
                     sensors[i].name,
                     state == 0 ? "CLOSED" : "OPEN");
        }
    }

    /* Normalize logic (active LOW → OK = 1 when CLOSED) */
    float1_ok = (sensors[0].last_state == 0);
    float2_ok = (sensors[1].last_state == 0);
}


static bool prev_float_ok = false;

static void process_float_logic(void)
{
    bool float_ok = float1_ok;  // or combine both if needed

    if (float_ok == prev_float_ok) {
        return;  // no change
    }

    if (float_ok) {
        ESP_LOGI("FLOAT", "Float OK → system armed");
        // ts_data.init_done = true;
    } else {
        ESP_LOGE("FLOAT", "Float LOST → emergency reset");
        // emergency actions here
    }

    prev_float_ok = float_ok;
}


void sensor_task(void *arg)
{
    while (1) {

        scan_float_sensors();

        process_float_logic();

        /* Update SENSOR-CONTROLLED relays ONLY */
        sensors_update(float1_ok, float2_ok);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

