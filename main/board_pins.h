#pragma once

#include "driver/gpio.h"

/* ============================================================
 * UART – MASTER LINK
 * ============================================================ */
#define UART_RELAY   UART_NUM_2
#define PIN_UART_RELAY_TX        GPIO_NUM_17
#define PIN_UART_RELAY_RX        GPIO_NUM_16

/* ============================================================
 * RELAY OUTPUTS
 * 4 HOT relays
 * 4 COLD relays
 * ============================================================ */

/* ---- HOT RELAYS ---- */
static const gpio_num_t RELAY_HOT_PINS[4] = {
    GPIO_NUM_12,
    GPIO_NUM_13,
    GPIO_NUM_14,
    GPIO_NUM_15
};

/* ---- COLD RELAYS ---- */
static const gpio_num_t RELAY_COLD_PINS[4] = {
    GPIO_NUM_16,
    GPIO_NUM_17,
    GPIO_NUM_18,
    GPIO_NUM_19
};

#define RELAY_HOT_COUNT     4
#define RELAY_COLD_COUNT    4

/* ============================================================
 * OPTIONAL: FLOAT SWITCH INPUTS (if locally wired later)
 * ============================================================ */
/*
#define PIN_FLOAT_1         GPIO_NUM_21
#define PIN_FLOAT_2         GPIO_NUM_22
*/

/* ============================================================
 * SAFETY / DEBUG (OPTIONAL)
 * ============================================================ */
/*
#define PIN_EMERGENCY_STOP  GPIO_NUM_23
#define PIN_STATUS_LED      GPIO_NUM_2
*/
