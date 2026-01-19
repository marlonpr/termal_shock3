// main.c  (ACTUATOR NODE – NO FSM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "uart_link.h"
#include "relay.h"
#include "sensor.h"

static const char *TAG = "ACTUATOR_MAIN";

static TaskHandle_t uart_task_handle = NULL;

static TaskHandle_t sensor_task_handle = NULL;


void app_main(void)
{
    ESP_LOGI(TAG, "Actuator node boot");

    /* ---- Initialize relays (safe state) ---- */
    relay_init();

	float_sensors_init();

    /* ---- Initialize UART link ---- */
    uart_link_init();

    /* ---- Start UART RX task ---- */
    xTaskCreate(
        uart_link_task,
        "uart_link_task",
        4096,
        NULL,
        1,
        &uart_task_handle
    );

    /* ---- Start Sensor task ---- */
    xTaskCreate(
        sensor_task,
        "sensor_task",
        4096,
        NULL,
        2,
        &sensor_task_handle
    );

    ESP_LOGI(TAG, "Actuator ready (command-driven)");

	vTaskDelay(pdMS_TO_TICKS(3000));



	


}


/*
	//====================== TEST ALL ==========================	
	
	relay_self_test_start();

	while (relay_self_test_active()) 
	{
	    relay_self_test_tick();
	    vTaskDelay(pdMS_TO_TICKS(100));
	}
*/

//====================== TEST CMD ==========================
/*
	//relay_force(0x01);

	//vTaskDelay(pdMS_TO_TICKS(30000));

	//relay_force(0x00);
*/
