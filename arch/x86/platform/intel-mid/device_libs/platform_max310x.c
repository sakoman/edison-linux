/*
 * platform_max310x.c: max310x platform data initilization file
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/spi/spi.h>
#include <linux/spi/intel_mid_ssp_spi.h>
#include <asm/intel-mid.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/lnw_gpio.h>
#include "platform_max310x.h"
#include <linux/platform_data/max310x.h>

static void tng_ssp_spi_cs_control(u32 command);
static void tng_ssp_spi_platform_pinmux(void);
static int max310x_platform_init(struct max310x_pdata *);

static int tng_ssp_spi2_FS_gpio = 111;
static int tng_gpio_47 = 47;

static struct intel_mid_ssp_spi_chip chip = {
	.burst_size = DFLT_FIFO_BURST_SIZE,
	.timeout = DFLT_TIMEOUT_VAL,
	/* SPI DMA is currently not usable on Tangier */
	.dma_enabled = false,
	.cs_control = tng_ssp_spi_cs_control,
	.platform_pinmux = tng_ssp_spi_platform_pinmux,
};

static struct max310x_pdata max310x_pdata = {
	.driver_flags	= MAX310X_EXT_CLK,
	.uart_flags[0]	= MAX310X_ECHO_SUPRESS | MAX310X_AUTO_DIR_CTRL,
	.frequency	= 3686000,
	.gpio_base	= -1,
	.init		= max310x_platform_init,
};

static void tng_ssp_spi_cs_control(u32 command)
{
	gpio_set_value(tng_ssp_spi2_FS_gpio, (command != 0) ? 1 : 0);
}

static void tng_ssp_spi_platform_pinmux(void)
{
	int err;
	int saved_muxing;
	/* Request Chip Select gpios */
	saved_muxing = gpio_get_alt(tng_ssp_spi2_FS_gpio);

	lnw_gpio_set_alt(tng_ssp_spi2_FS_gpio, LNW_GPIO);
	err = gpio_request_one(tng_ssp_spi2_FS_gpio,
			GPIOF_DIR_OUT|GPIOF_INIT_HIGH, "Arduino Shield SS");
	if (err) {
		pr_err("%s: unable to get Chip Select GPIO,\
				fallback to legacy CS mode \n", __func__);
		lnw_gpio_set_alt(tng_ssp_spi2_FS_gpio, saved_muxing);
		chip.cs_control = NULL;
		chip.platform_pinmux = NULL;
	}

	lnw_gpio_set_alt(tng_gpio_47, LNW_GPIO);
}

static int max310x_platform_init(struct max310x_pdata *platform_data)
{
	int err = 0;

	pr_err("%s: Entry\n", __func__);

	if (IS_ERR(platform_data)) {
		err = PTR_ERR(platform_data);
		pr_err("%s: missing max310x platform data: %d\n", __func__, err);
		goto out;
	}

	if (gpio_is_valid(46)) {
		/* Request gpio */
		err = gpio_request(46, "max310x reset1");
		if (err < 0) {
			pr_err("%s: Unable to request GPIO:%d, err:%d\n",
					__func__, 46, err);
			goto out;
		}

		gpio_export(46, 0);

		/* set gpio direction and pull low */
		err = gpio_direction_output(46, 0);
		if (err < 0) {
			pr_err("%s: Unable to set GPIO:%d direction, err:%d\n",
			 __func__, 46, err);
			goto out;
		}

		udelay(1000);
		gpio_set_value(46, 1);

	}

	if (gpio_is_valid(165)) {
		/* Request gpio */
		err = gpio_request(165, "max310x reset0");
		if (err < 0) {
			pr_err("%s: Unable to request GPIO:%d, err:%d\n",
					__func__, 165, err);
			goto out;
		}

		gpio_export(165, 0);

		/* set gpio direction and pull low */
		err = gpio_direction_output(165, 0);
		if (err < 0) {
			pr_err("%s: Unable to set GPIO:%d direction, err:%d\n",
			 __func__, 165, err);
			goto out;
		}

		// udelay(1000);
		// gpio_set_value(165, 1);

	}

out:
	pr_err("%s Exit\n", __func__);
	return err;
}

void __init *max310x_platform_data(void *info)
{
	struct spi_board_info *spi_info = info;

	if (!spi_info) {
		pr_err("%s: invalid info pointer\n", __func__);
		return NULL;
	}

	spi_info->mode = SPI_MODE_0;
	spi_info->controller_data = &chip;
	spi_info->platform_data = &max310x_pdata;
	spi_info->bus_num = FORCE_SPI_BUS_NUM;

	return &max310x_pdata;
}
