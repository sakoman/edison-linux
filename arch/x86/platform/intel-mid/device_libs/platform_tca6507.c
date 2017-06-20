/*
 * platform_tca6507.c: tca6507 led driver platform data initilization file
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
#include "platform_tca6507.h"
#include <linux/leds-tca6507.h>

static struct led_info tca6507_led_info[] = {
	{
		.name = "tca6507-d9-blue",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-d9-green",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-d9-red",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-d10-blue",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-d10-green",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-d10-red",
		.default_trigger = "none",
		.flags = 0,
	},
	{
		.name = "tca6507-unused",
		.default_trigger = "none",
		.flags = 0,
	},
};

static struct tca6507_platform_data tca6507_pdata = {
	.leds = {
		.num_leds = ARRAY_SIZE(tca6507_led_info),
		.leds = tca6507_led_info,
	},
};

void __init *tca6507_platform_data(void *info)
{
	return &tca6507_pdata;
}
