#pragma once
#include "driver/gpio.h"

/* ================= UART ================= */
#define UART_RELAY UART_NUM_2
#define PIN_UART_RELAY_TX GPIO_NUM_17
#define PIN_UART_RELAY_RX GPIO_NUM_16

/* ================= RELAYS ================= */

/* HOT (now 6) */
static const gpio_num_t RELAY_HOT_PINS[6] = {
    GPIO_NUM_21,//
    GPIO_NUM_22,//
    GPIO_NUM_25,//                             
    GPIO_NUM_23,//
    GPIO_NUM_19,   // 19
    GPIO_NUM_4    // 
};

/* COLD (now 6) */
static const gpio_num_t RELAY_COLD_PINS[6] = {
    GPIO_NUM_18, //
    GPIO_NUM_15,//
    GPIO_NUM_12,//
    GPIO_NUM_14,//
    GPIO_NUM_27,   // 14
    GPIO_NUM_26    // 
};

#define RELAY_HOT_COUNT   6
#define RELAY_COLD_COUNT  6


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
