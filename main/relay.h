#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
 * Relay mask bits (param16 of CMD_FORCE_RELAY)
 *
 * bit0 = HOT relay
 * bit1 = COLD relay
 */
#define RELAY_HOT_BIT   (1 << 0)
#define RELAY_COLD_BIT  (1 << 1)

/* Init GPIOs */
void relay_init(void);

/* Force relays using bitmask */
void relay_force(uint16_t mask);

/* Immediate OFF (safety) */
void relay_all_off(void);

/* Status queries */
bool relay_is_hot_on(void);
bool relay_is_cold_on(void);

uint8_t relay_hot_mask(void);
uint8_t relay_cold_mask(void);
void apply_outputs(void);