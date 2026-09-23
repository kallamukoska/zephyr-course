/*
 * Minimal board init for our_board (MCXN947, CPU0).
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/init.h>
#include <zephyr/devicetree.h>
#include <fsl_clock.h>
#include <fsl_spc.h>
#include <soc.h>

/* Core clock frequency: 150MHz (must match SYS_CLOCK_HW_CYCLES_PER_SEC) */
#define CLOCK_INIT_CORE_CLOCK 150000000U

extern uint32_t SystemCoreClock;

/* Raise core voltage to 1.2 V (OverDrive), required to run at 150MHz. */
static void power_mode_od(void)
{
	spc_active_mode_dcdc_option_t opt = {
		.DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
		.DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
	};
	SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &opt);

	spc_active_mode_core_ldo_option_t ldo_opt = {
		.CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
		.CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
	};
	SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldo_opt);

	spc_sram_voltage_config_t cfg = {
		.operateVoltage       = kSPC_sramOperateAt1P2V,
		.requestVoltageUpdate = true,
	};
	SPC_SetSRAMOperateVoltage(SPC0, &cfg);
}

void board_early_init_hook(void)
{
	power_mode_od();

	CLOCK_EnableClock(kCLOCK_Scg);

	/* Run from FRO 12M while reconfiguring the main clock */
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

	/* Flash wait-states for 1.2 V / 150MHz */
	FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | FMU_FCTRL_RWSC(0x3U);

	/* FRO HF 48MHz is the PLL0 source */
	CLOCK_SetupFROHFClocking(48000000U);

	/* PLL0: 48MHz / 8 * 50 / 2 = 150MHz */
	const pll_setup_t pll0Setup = {
		.pllctrl = SCG_APLLCTRL_SOURCE(1U) | SCG_APLLCTRL_SELI(27U) |
			   SCG_APLLCTRL_SELP(13U),
		.pllndiv = SCG_APLLNDIV_NDIV(8U),
		.pllpdiv = SCG_APLLPDIV_PDIV(1U),
		.pllmdiv = SCG_APLLMDIV_MDIV(50U),
		.pllRate = 150000000U
	};
	CLOCK_SetPLL0Freq(&pll0Setup);
	CLOCK_SetPll0MonitorMode(kSCG_Pll0MonitorDisable);

	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);

	/* GPIO ports are clock-gated after reset; the GPIO driver does not enable them. */
#if DT_NODE_HAS_STATUS_OKAY(DT_NODELABEL(gpio0))
	CLOCK_EnableClock(kCLOCK_Gpio0);
#endif

	SystemCoreClock = CLOCK_INIT_CORE_CLOCK;
}
