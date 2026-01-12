// main.c  (ACTUATOR NODE – NO FSM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "uart_link.h"
#include "relay.h"

static const char *TAG = "ACTUATOR_MAIN";

static TaskHandle_t uart_task_handle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "Actuator node boot");

    /* ---- Initialize relays (safe state) ---- */
    relay_init();

    /* ---- Initialize UART link ---- */
    uart_link_init();

    /* ---- Start UART RX task ---- */
    xTaskCreate(
        uart_link_task,
        "uart_link_task",
        4096,
        NULL,
        10,
        &uart_task_handle
    );

    ESP_LOGI(TAG, "Actuator ready (command-driven)");
}
