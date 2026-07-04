/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Adapted from boards/kounolab/kounolab_dongle_uf2/board.c:
 * VDDH is fed 5 V from USB (high-voltage mode). A fresh chip
 * defaults REGOUT0 to 1.8 V, which is too low for the WS2812
 * data input. Program it to 3.3 V once, then reset.
 */

#include <zephyr/init.h>
#include <hal/nrf_power.h>

static int board_versipellis_ev2_init(void)
{
	if ((nrf_power_mainregstatus_get(NRF_POWER) ==
	     NRF_POWER_MAINREGSTATUS_HIGH) &&
	    ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
	     (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))) {

		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
			;
		}

		NRF_UICR->REGOUT0 =
		    (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
		    (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos);

		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
			;
		}

		/* a reset is required for changes to take effect */
		NVIC_SystemReset();
	}

	return 0;
}

SYS_INIT(board_versipellis_ev2_init, PRE_KERNEL_1,
	 CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
