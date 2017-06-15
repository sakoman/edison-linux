/*
 * platform_ds1307.h: ds1307 rtc platform data header file
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#ifndef _PLATFORM_DS1307_H_
#define _PLATFORM_DS1307_H_

extern void __init *ds1307_platform_data(void *info) __attribute__((weak));
#endif
