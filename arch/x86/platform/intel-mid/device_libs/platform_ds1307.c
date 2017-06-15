/*
 * platform_ds1307.c: ds1307 rtc platform data initilization file
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <asm/intel-mid.h>
#include "platform_ds1307.h"
#include <linux/rtc/ds1307.h>

static struct ds1307_platform_data ds1307_pdata = {
	.trickle_charger_setup	= DS1307_TRICKLE_CHARGER_250_OHM,
};

void __init *ds1307_platform_data(void *info)
{
	struct i2c_board_info *i2c_info = info;

	pr_err("%s: using GPIO %d for IRQ\n", __func__, i2c_info->irq);

	return &ds1307_pdata;
}
