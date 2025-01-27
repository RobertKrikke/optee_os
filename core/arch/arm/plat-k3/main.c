// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2016-2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 */

#include <platform_config.h>

#include <console.h>
#include <drivers/gic.h>
#include <drivers/serial8250_uart.h>
#include <kernel/boot.h>
#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <kernel/tee_common_otp.h>
#include <mm/core_memprot.h>
#include <mm/tee_pager.h>
#include <platform_config.h>
#include <stdint.h>
#include <string_ext.h>

#if defined(PLATFORM_FLAVOR_am65x) || defined(PLATFORM_FLAVOR_j721e)
#include "drivers/sec_proxy.h"
#include "drivers/ti_sci.h"
#endif

static struct gic_data gic_data;
static struct serial8250_uart_data console_data;

register_phys_mem_pgdir(MEM_AREA_IO_SEC, GICC_BASE, GICC_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, GICD_BASE, GICD_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE,
		  SERIAL8250_UART_REG_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, SEC_PROXY_DATA_BASE,
			SEC_PROXY_DATA_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, SEC_PROXY_SCFG_BASE,
			SEC_PROXY_SCFG_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, SEC_PROXY_RT_BASE, SEC_PROXY_RT_SIZE);
register_ddr(DRAM0_BASE, DRAM0_SIZE);

void main_init_gic(void)
{
	gic_init_base_addr(&gic_data, GICC_BASE, GICD_BASE);
	itr_init(&gic_data.chip);
}

void main_secondary_init_gic(void)
{
	gic_cpu_init(&gic_data);
}

void itr_core_handler(void)
{
	gic_it_handle(&gic_data);
}

void console_init(void)
{
	serial8250_uart_init(&console_data, CONSOLE_UART_BASE,
			     CONSOLE_UART_CLK_IN_HZ, CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
}

#if defined(PLATFORM_FLAVOR_am65x) || defined(PLATFORM_FLAVOR_j721e)
TEE_Result tee_otp_get_hw_unique_key(struct tee_hw_unique_key *hwkey)
{
	uint8_t dkek[SA2UL_DKEK_KEY_LEN] = { 0 };
	int ret = 0;

	assert(SA2UL_DKEK_KEY_LEN >= HW_UNIQUE_KEY_LENGTH);

	ret = k3_sec_proxy_init();
	if (ret)
		return ret;

	ret = ti_sci_init();
	if (ret)
		return ret;

	ret = ti_sci_get_dkek(0, "OP-TEE", "DKEK", dkek);
	if (ret)
		return ret;

	IMSG("HUK Initialized");
	memcpy(&hwkey->data[0], dkek, sizeof(hwkey->data));

	memzero_explicit(&dkek, sizeof(dkek));
	return TEE_SUCCESS;
}
#endif
