#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
 * Relay mask bits (param16 of CMD_FORCE_RELAY)
 *
 */


/* Init GPIOs */
void relay_init(void);

/* Force relays using bitmask */
void relay_force(uint16_t mask);

/* Immediate OFF (safety) */
void relay_all_off(void);

/* Self-test */
void relay_self_test_start(void);
void relay_self_test_tick(void);
bool relay_self_test_active(void);

void sensors_update(bool float1, bool float2);
