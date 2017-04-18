/*
 * platform_max310x.h: max310x platform data header file
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#ifndef _PLATFORM_MAX310X_H_
#define _PLATFORM_MAX310X_H_

#define FORCE_SPI_BUS_NUM	5
#define FORCE_CHIP_SELECT	1

extern void *max310x_platform_data(void *info) __attribute__((weak));
#endif
