/*
 * Rad Pro
 * System
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#if !defined(SYSTEM_H)
#define SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

#define FIRMWARE_AUTHOR "Gissio"
#define FIRMWARE_NAME "GC-01 Pro RU"
#define FIRMWARE_VERSION "0.3.0-exp"
#define SETTINGS_VERSION {'G','C','P','R','U','0','0','1'}

void initGPIO(void);

void initSystem(void);

void setFastSystemClock(bool value);

void getDeviceId(char *);

void startBootloader(void);

#endif
