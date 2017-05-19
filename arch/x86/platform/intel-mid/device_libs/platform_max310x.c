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

static void spi_platform_pinmux(void);

static void spi_cs0_control(u32 command);
static void spi_cs1_control(u32 command);

static int max310x_cs0_platform_init(struct max310x_pdata *);
static int max310x_cs1_platform_init(struct max310x_pdata *);

static int spi_cs0_gpio = 110;
static int spi_cs0_irq = 45;
static int spi_cs0_nreset = 165;

static int spi_cs1_gpio = 111;
static int spi_cs1_irq = 47;
static int spi_cs1_nreset = 46;

static struct intel_mid_ssp_spi_chip chip0 = {
	.burst_size = DFLT_FIFO_BURST_SIZE,
	.timeout = DFLT_TIMEOUT_VAL,
	/* SPI DMA is currently not usable on Tangier */
	.dma_enabled = false,
	.cs_control = spi_cs0_control,
	.platform_pinmux = spi_platform_pinmux,
};

static struct intel_mid_ssp_spi_chip chip1 = {
	.burst_size = DFLT_FIFO_BURST_SIZE,
	.timeout = DFLT_TIMEOUT_VAL,
	/* SPI DMA is currently not usable on Tangier */
	.dma_enabled = false,
	.cs_control = spi_cs1_control,
	.platform_pinmux = spi_platform_pinmux,
};

static struct max310x_pdata max310x_cs0_pdata = {
	.driver_flags	= MAX310X_EXT_CLK,
	.frequency	= 19200000,
	.gpio_base	= 480,
	.init		= max310x_cs0_platform_init,
};

static struct max310x_pdata max310x_cs1_pdata = {
	.driver_flags	= MAX310X_EXT_CLK,
	.frequency	= 19200000,
	.gpio_base	= 496,
	.init		= max310x_cs1_platform_init,
};

static void spi_cs0_control(u32 command)
{
	gpio_set_value(spi_cs0_gpio, (command != 0) ? 1 : 0);
}

static void spi_cs1_control(u32 command)
{
	gpio_set_value(spi_cs1_gpio, (command != 0) ? 1 : 0);
}

static void spi_platform_pinmux(void)
{
	int err;

	/* called only one time, so set up pinmux for both CS0 and CS1 */
	lnw_gpio_set_alt(spi_cs0_gpio, LNW_GPIO);

	err = gpio_request_one(spi_cs0_gpio,
			GPIOF_DIR_OUT|GPIOF_INIT_HIGH, "SPI CS0");
	if (err)
		pr_err("%s: unable to get Chip Select 0 GPIO\n", __func__);

	lnw_gpio_set_alt(spi_cs1_gpio, LNW_GPIO);

	err = gpio_request_one(spi_cs1_gpio,
			GPIOF_DIR_OUT|GPIOF_INIT_HIGH, "SPI CS1");
	if (err)
		pr_err("%s: unable to get Chip Select 1 GPIO\n", __func__);

	/* set pinmux for irq gpios */
	lnw_gpio_set_alt(spi_cs0_irq, LNW_GPIO);
	lnw_gpio_set_alt(spi_cs1_irq, LNW_GPIO);
}

static int max310x_cs0_platform_init(struct max310x_pdata *platform_data)
{
	int err = 0;

	if (IS_ERR(platform_data)) {
		err = PTR_ERR(platform_data);
		pr_err("%s: missing max310x platform data: %d\n", __func__, err);
		goto out;
	}

	if (gpio_is_valid(spi_cs0_nreset)) {
		/* Request gpio */
		err = gpio_request(spi_cs0_nreset, "max310x reset0");
		if (err < 0) {
			pr_err("%s: Unable to request GPIO:%d, err:%d\n",
					__func__, spi_cs0_nreset, err);
			goto out;
		}

		gpio_export(spi_cs0_nreset, 0);

		/* set gpio direction and pull low */
		err = gpio_direction_output(spi_cs0_nreset, 0);
		if (err < 0) {
			pr_err("%s: Unable to set GPIO:%d direction, err:%d\n",
			 __func__, spi_cs0_nreset, err);
			goto out;
		}

		udelay(1000);
		gpio_set_value(spi_cs0_nreset, 1);

	}

out:
	return err;
}

static int max310x_cs1_platform_init(struct max310x_pdata *platform_data)
{
	int err = 0;

	if (IS_ERR(platform_data)) {
		err = PTR_ERR(platform_data);
		pr_err("%s: missing max310x platform data: %d\n", __func__, err);
		goto out;
	}

	if (gpio_is_valid(spi_cs1_nreset)) {
		/* Request gpio */
		err = gpio_request(spi_cs1_nreset, "max310x reset1");
		if (err < 0) {
			pr_err("%s: Unable to request GPIO:%d, err:%d\n",
					__func__, spi_cs1_nreset, err);
			goto out;
		}

		gpio_export(spi_cs1_nreset, 0);

		/* set gpio direction and pull low */
		err = gpio_direction_output(spi_cs1_nreset, 0);
		if (err < 0) {
			pr_err("%s: Unable to set GPIO:%d direction, err:%d\n",
			 __func__, spi_cs1_nreset, err);
			goto out;
		}

		udelay(1000);
		gpio_set_value(spi_cs1_nreset, 1);

	}

out:
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
		spi_info->bus_num = FORCE_SPI_BUS_NUM;

	if (spi_info->chip_select == 0) {
		spi_info->controller_data = &chip0;
		spi_info->platform_data = &max310x_cs0_pdata;
		return &max310x_cs0_pdata;
	} else {
		if (spi_info->chip_select == 1) {
			spi_info->controller_data = &chip1;
			spi_info->platform_data = &max310x_cs1_pdata;
			return &max310x_cs1_pdata;
		} else {
			pr_err("%s: invalid chip select %d\n", __func__, spi_info->chip_select);
			return NULL;
		}
	}
}
