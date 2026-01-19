#pragma once
#include "driver/gpio.h"

/* ================= UART ================= */
#define UART_RELAY UART_NUM_2
#define PIN_UART_RELAY_TX GPIO_NUM_17
#define PIN_UART_RELAY_RX GPIO_NUM_16

#define PIN_FLOAT_1         GPIO_NUM_35
#define PIN_FLOAT_2         GPIO_NUM_34

/* ================= RELAYS =================
 *
 * Logical relay order:
 * bit 0..3 : heaters
 * bit 4    : pump
 * bit 5    : fan
 */


#define RELAY_MASK_COUNT     8
#define RELAY_SENSOR_COUNT  4
#define RELAY_TOTAL_COUNT   12

#define RELAY_FLOAT1_A   1   // Relay 9
#define RELAY_FLOAT1_B   2   // Relay 10
#define RELAY_FLOAT2     3   // Relay 11




typedef enum 
{
	//M1
    RELAY_M1_1 = 0, //EV4
    RELAY_M1_2,		//EV7		
    RELAY_M1_3,		//EV2
    RELAY_M1_4,		//EV1*

	//M2
    RELAY_M2_1,		//B1*
    RELAY_M2_2,		//EV3
    RELAY_M2_3,		//
	RELAY_M2_4,		//

	//M3
    RELAY_M3_1,		//
    RELAY_M3_2,		//B2
    RELAY_M3_3,		//EV6
	RELAY_M3_4,		//EV5*

	RELAY_COUNT


} relay_id_t;

_Static_assert(RELAY_COUNT <= 16, "Relay count exceeds mask width");


static const gpio_num_t RELAY_PINS[RELAY_COUNT] = 
{
    [RELAY_M1_1] = GPIO_NUM_18,
    [RELAY_M1_2] = GPIO_NUM_19,
    [RELAY_M1_3] = GPIO_NUM_4,
    [RELAY_M1_4] = GPIO_NUM_27,

    [RELAY_M2_1] = GPIO_NUM_26,
    [RELAY_M2_2] = GPIO_NUM_23, //OK
    [RELAY_M2_3] = GPIO_NUM_5,
    [RELAY_M2_4] = GPIO_NUM_14,

    [RELAY_M3_1] = GPIO_NUM_12,
    [RELAY_M3_2] = GPIO_NUM_21, //OK
    [RELAY_M3_3] = GPIO_NUM_25, //OK
    [RELAY_M3_4] = GPIO_NUM_15,
};








