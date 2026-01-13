#pragma once
#include "driver/gpio.h"

/* ================= UART ================= */
#define UART_RELAY UART_NUM_2
#define PIN_UART_RELAY_TX GPIO_NUM_17
#define PIN_UART_RELAY_RX GPIO_NUM_16

/* ================= RELAYS ================= */

/* HOT */
static const gpio_num_t RELAY_HOT_PINS[4] = {
    GPIO_NUM_18,
    GPIO_NUM_19,
    GPIO_NUM_21,
    GPIO_NUM_22
};

/* COLD */
static const gpio_num_t RELAY_COLD_PINS[4] = {
    GPIO_NUM_23,
    GPIO_NUM_25,
    GPIO_NUM_26,
    GPIO_NUM_27
};

#define RELAY_HOT_COUNT  4
#define RELAY_COLD_COUNT 4

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
