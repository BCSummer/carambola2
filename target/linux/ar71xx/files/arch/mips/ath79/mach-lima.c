/*
 *  8devices Lima board support
 *
 *  Copyright (C) 2016 Mantas Pucka <mantas@8devices.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define LIMA_GPIO_BTN_RST_DEFAULT	16
#define LIMA_KEYS_POLL_INTERVAL	20	/* msecs */
#define LIMA_KEYS_DEBOUNCE_INTERVAL	(3 * LIMA_KEYS_POLL_INTERVAL)

#define LIMA_ETH_PHYS	(BIT(0) | BIT(1))

static struct gpio_keys_button lima_gpio_keys[] __initdata = {
	{
		.desc		= "button0",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LIMA_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= LIMA_GPIO_BTN_RST_DEFAULT,
		.active_low	= 1,
	}
};

static struct ath79_spi_controller_data lima_spi0_cdata =
{
	.cs_type = ATH79_SPI_CS_TYPE_INTERNAL,
	.cs_line = 0,
};

static struct ath79_spi_controller_data lima_spi1_cdata =
{
	.cs_type = ATH79_SPI_CS_TYPE_INTERNAL,
	.cs_line = 1,
};

static struct ath79_spi_controller_data lima_spi2_cdata =
{
	.cs_type = ATH79_SPI_CS_TYPE_INTERNAL,
	.cs_line = 2,
};

static struct spi_board_info lima_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.controller_data = &lima_spi0_cdata,
	},
	{
		.bus_num	= 0,
		.chip_select	= 1,
		.max_speed_hz   = 25000000,
		.modalias	= "spidev",
		.controller_data = &lima_spi1_cdata,
	},
	{
		.bus_num	= 0,
		.chip_select	= 2,
		.max_speed_hz   = 25000000,
		.modalias	= "spidev",
		.controller_data = &lima_spi2_cdata,
	}
};

static struct ath79_spi_platform_data lima_spi_data =
{
	.bus_num		= 0,
	.num_chipselect 	= 3,
	.use_hw_shiftreg 	= true,
};

static void __init lima_setup(void)
{
	ath79_register_spi(&lima_spi_data, lima_spi_info, 3);

	ath79_register_gpio_keys_polled(-1, LIMA_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(lima_gpio_keys),
					lima_gpio_keys);

	ath79_setup_ar933x_phy4_switch(true, true);
	ath79_register_mdio(0, ~LIMA_ETH_PHYS);
	ath79_switch_data.phy4_mii_en = 1;

	/* LAN */
	ath79_switch_data.phy_poll_mask |= BIT(0);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.phy_mask = BIT(1);
	ath79_register_eth(1);

	/* WAN */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_register_eth(0);

	ath79_register_wmac_simple();
	ath79_register_usb();
	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_LIMA, "LIMA", "8devices Lima board", lima_setup);
