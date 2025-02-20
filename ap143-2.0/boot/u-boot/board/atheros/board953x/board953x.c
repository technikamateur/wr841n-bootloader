/*
 * Copyright (c) 2013 Qualcomm Atheros, Inc.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include <atheros.h>

extern int ath_ddr_initial_config(uint32_t refresh);
extern int ath_ddr_find_size(void);

/* By WANG Wenhu, May 20th, 2015. 
 * For MR3420(UN) 3.0 GPIO Configuration. 
 */
#if (CONFIG_PRODUCT_ID == 0x34200003)
#define AP143_2_GPIO_SHIFT_OE     11
#define AP143_2_GPIO_SHIFT_SER    16
#define AP143_2_GPIO_SHIFT_SRCLK  15
#define AP143_2_GPIO_SHIFT_SRCLR  14
#define AP143_2_GPIO_SHIFT_RCLK   13


enum SHIFT_REGISTER_OUTPUT {
    QH_HUB_RESET = 0,
	QG_WIFI_LED,
	QF_WAN_OLED,
	QE_WAN_YLED,
	QD_LAN1_LED,
	QC_LAN2_LED,
	QB_LAN3_LED,
	QA_LAN4_LED,	
	SHIFT_REGISTER_OUTPUT_NUM
};

void shift_register_set(unsigned int val) 
{
    int index = 0;
	int value = 0;

	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_RCLK));  
	
	for(index = 0; index < SHIFT_REGISTER_OUTPUT_NUM; index++)
	{
		value = (val >> index) & 0x1;
		if(value)
		{
		   ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SER));
		}
		else
		{
		   ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SER));  
		}

        udelay(1);
		ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SRCLK));
		udelay(1);
		ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SRCLK));
	}

    ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_RCLK));
    udelay(1);
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_RCLK));
	udelay(1);
}
#endif

#ifdef COMPRESSED_UBOOT
#	define prmsg(...)
#	define args		char *s
#	define board_str(a)	do {						\
	char ver[] = "0";							\
	uint32_t        revid;							\
	if(((revid=ath_reg_rd(RST_REVISION_ID_ADDRESS))&0xff0)==0x140) 		\
		strcpy(s, a " - Honey Bee 1.");					\
	else			 						\
		strcpy(s, a " - Honey Bee 2.");					\
	ver[0] +=  (revid & 0xf) ;		 				\
	strcat(s, ver);								\
} while (0)
#else
#	define prmsg	printf
#	define args		void
#	define board_str(a)							\
	uint32_t        revid;							\
	if(((revid=ath_reg_rd(RST_REVISION_ID_ADDRESS))&0xff0)==0x140) 		\
	printf(a " - Honey Bee 1.%d", revid & 0xf);				\
	else									\ 
	printf(a " - Honey Bee 2.%d", revid & 0xf);
#endif

void
ath_usb_initial_config(void)
{
#define unset(a)	(~(a))

	if (ath_reg_rd(RST_BOOTSTRAP_ADDRESS) & RST_BOOTSTRAP_TESTROM_ENABLE_MASK) {

		ath_reg_rmw_set(RST_RESET_ADDRESS, RST_RESET_USB_HOST_RESET_SET(1));
		udelay(1000);
		ath_reg_rmw_set(RST_RESET_ADDRESS, RST_RESET_USB_PHY_RESET_SET(1));
		udelay(1000);

		ath_reg_wr(PHY_CTRL5_ADDRESS, PHY_CTRL5_RESET_1);
		udelay(1000);

		ath_reg_rmw_set(RST_RESET_ADDRESS, RST_RESET_USB_PHY_PLL_PWD_EXT_SET(1));
		udelay(1000);
		ath_reg_rmw_set(RST_RESET_ADDRESS, RST_RESET_USB_PHY_ARESET_SET(1));
		udelay(1000);

		ath_reg_rmw_clear(RST_CLKGAT_EN_ADDRESS, RST_CLKGAT_EN_USB1_SET(1));

		return;
	}

	ath_reg_wr_nf(SWITCH_CLOCK_SPARE_ADDRESS,
		ath_reg_rd(SWITCH_CLOCK_SPARE_ADDRESS) |
		SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(2));
	udelay(1000);

	ath_reg_rmw_set(RST_RESET_ADDRESS,
				RST_RESET_USB_PHY_SUSPEND_OVERRIDE_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_ARESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_RESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_HOST_RESET_SET(1));
	udelay(1000);

	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_PLL_PWD_EXT_SET(1));
	udelay(10);
}

#if 0
void ath_led_off(void)
{
#if (CONFIG_PRODUCT_ID == 0x08410011 | CONFIG_PRODUCT_ID == 0x08450001 | CONFIG_PRODUCT_ID == 0x07400006)
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, ((1 << 1) | (1 << 2) |(1 << 3) | (1 << 4) | (1 << 11) | (1 << 13) | (1 << 14) | (1 << 15) | (1 << 16)));
#endif
}
#endif

void ath_gpio_config(void)
{
	/* disable the CLK_OBS on GPIO_4 and set GPIO4 as input */
#if 0
	ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 4));
	ath_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_MASK);
	ath_reg_rmw_set(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_SET(0x80));
	ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 4));
#endif

#if 0
	/* Set GPIO 13 as input for LED functionality to be OFF during bootup */
	ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 13));
	/* Turn off JUMPST_LED and 5Gz LED during bootup */
	ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 15));
#endif
	//ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 12));
#if 0
#ifdef  CONFIG_PRODUCT_WR843NV2
/*
		input GPIO:		
						1 - WPS/RESET Button, low electrical level is effective
						2 - Router/WISP model swtich, low electrical level is WISP, high  electrical level is Router
		output GPIO:	
						12 - WLAN Led, low electrical level is effective
						13 - System Led, low electrical level is effective
						14 - WAN Led, low electrical level is effective
						15 - WPS Led, high electrical level is effective
						17 - LAN1 Led, low electrical level is effective
						11 - LAN2 Led, low electrical level is effective
						3  - LAN3 Led, low electrical level is effective
						0  - LAN4 Led, low electrical level is effective
						16 - OD, GPIO16 = 1 & GPIO14 = 0, GREEN; 
								 GPIO16 = 0 & GPIO14 = 1, YELLOW;					
	*/		

	//disable JTAG
    ath_reg_rmw_set(GPIO_FUNCTION_ADDRESS, (1 << 1));
    // set GPIO 0, 1 as input
    //ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 0) | (1 << 1));
    ath_reg_rmw_set(GPIO_OE_ADDRESS, 1 << 1);

    ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 4));
   
	// set GPIO 0,3,11,12,13,14,15,16,17 as output
    ath_reg_rmw_clear(GPIO_OE_ADDRESS, ((1 << 0) | (1 << 3) | (1 << 11)| (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17)));
	//ath_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_MASK);
	//ath_reg_rmw_set(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_SET(0x80));
    ath_reg_rmw_clear(GPIO_OUT_FUNCTION4_ADDRESS, GPIO_OUT_FUNCTION4_ENABLE_GPIO_17_MASK);
	//ath_reg_rmw_set(GPIO_OUT_FUNCTION4_ADDRESS, GPIO_OUT_FUNCTION4_ENABLE_GPIO_17_SET(0x00));

	/*turn all LED on*/
  	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 << 0) | (1 << 3) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) |(1 << 17)) ;
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 << 15));

	udelay(200 * 1000);

	/* turn all LED off */
    ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 << 0) | (1 << 3) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) |(1 << 17)) ;
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 << 15));
#endif
#endif

/* By WANG Wenhu, May 20th, 2015. 
 * For MR3420(UN) 3.0 GPIO Configuration. 
 */
#if (CONFIG_PRODUCT_ID == 0x34200003)
	/*
    ath_reg_rmw_set(GPIO_FUNCTION_ADDRESS, (1 << 1));

    ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1));
    ath_reg_rmw_set(GPIO_OUT_ADDRESS,(1));
	
    ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 1) | (1 << 3) | (1 << 11) | (1 << 13) | (1 << 14) | (1 << 15) | (1 << 16) );

	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 << 1) | (1 << 3));
	
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_OE));
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SER) | \
		(1 <<  AP143_2_GPIO_SHIFT_SRCLK) | (1 << AP143_2_GPIO_SHIFT_SRCLR) | (1 <<  AP143_2_GPIO_SHIFT_RCLK));

    udelay(1);
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SRCLR));
	udelay(1);
	
	shift_register_set(0x4);
    ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_OE));
    udelay(500 * 1000);
	
    shift_register_set(0x8);
    udelay(500 * 1000);

    ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 << 1) | (1 << 3) | (1 <<  AP143_2_GPIO_SHIFT_OE));
	shift_register_set(0xff);*/

	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 << 1) | (1 << 3));
	udelay(100);
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_SRCLR));
	shift_register_set(0x4);
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_OE));
	udelay(100);
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 << 1) | (1 << 3));
	udelay(200 * 1000);
	
    shift_register_set(0x8);
    udelay(200 * 1000);
	
	// for regester control
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  AP143_2_GPIO_SHIFT_OE) | (1 << 13) | (1 << 14) | (1 << 15) | (1 << 16));
	shift_register_set(0xff);
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 << 1) | (1 << 3));
    udelay(200 * 1000);

	//USB disable
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (0));
	
#elif (CONFIG_PRODUCT_ID == 0x08420003)
	//disable JTAG
	ath_reg_rmw_set(GPIO_FUNCTION_ADDRESS, (1 << 1));
	// set GPIO 0, 1 as input
	ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 0) | (1 << 1));

	ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 4));
	// set GPIO 2,3,4,11~17 as output
	ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 <<  2) | (1 <<  3) | (1 <<  4) | (1 << 11) | \
										(1 << 12) | (1 << 13) | (1 << 14) | (1 << 15) | \
										(1 << 16) | (1 << 17));
	ath_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_MASK);
	ath_reg_rmw_set(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_SET(0x80));
	ath_reg_rmw_clear(GPIO_OUT_FUNCTION4_ADDRESS, GPIO_OUT_FUNCTION4_ENABLE_GPIO_17_MASK);
	//ath_reg_rmw_set(GPIO_OUT_FUNCTION4_ADDRESS, GPIO_OUT_FUNCTION4_ENABLE_GPIO_17_SET(0x00));

	/* turn all LED off */
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  2) | (1 <<  3) | (1 << 16) | (1 << 17) | \
										(1 << 12) | (1 << 13) | (1 << 14) | (1 << 15));
	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  4));
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  11));

	udelay(200 * 1000);

	ath_reg_rmw_clear(GPIO_OUT_ADDRESS, (1 <<  11));
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  4));

	udelay(200 * 1000);

	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  3) | (1 << 16) | (1 << 17)| \
									(1 << 12) | (1 << 13) | (1 << 14) | (1 << 15));
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  4));
	ath_reg_rmw_set(GPIO_OUT_ADDRESS, (1 <<  11));
#endif

}

int
ath_mem_config(void)
{
	unsigned int type, reg32, *tap;
	extern uint32_t *ath_ddr_tap_cal(void);

#if !defined(CONFIG_ATH_EMULATION)
	type = ath_ddr_initial_config(CFG_DDR_REFRESH_VAL);

	tap = ath_ddr_tap_cal();
	prmsg("tap = 0x%p\n", tap);

	tap = (uint32_t *)0xbd001f10;
	prmsg("Tap (low, high) = (0x%x, 0x%x)\n", tap[0], tap[1]);

	tap = (uint32_t *)TAP_CONTROL_0_ADDRESS;
	prmsg("Tap values = (0x%x, 0x%x, 0x%x, 0x%x)\n",
		tap[0], tap[2], tap[2], tap[3]);

	/* Take WMAC out of reset */
	reg32 = ath_reg_rd(RST_RESET_ADDRESS);
	reg32 = reg32 & ~RST_RESET_RTC_RESET_SET(1);
	ath_reg_wr_nf(RST_RESET_ADDRESS, reg32);

	ath_usb_initial_config();

	ath_gpio_config();
#endif /* !defined(CONFIG_ATH_EMULATION) */

	return ath_ddr_find_size();
}

long int initdram(int board_type)
{
	return (ath_mem_config());
}

int	checkboard(args)
{
	board_str(CONFIG_BOARD_NAME);
	return 0;
}
