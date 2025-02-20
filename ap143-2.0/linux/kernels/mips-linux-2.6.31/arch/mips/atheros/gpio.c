/*
 *  Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/config.h>/*  by huangwenzhong, 28May13 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <asm/types.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/system.h>
#include <linux/mtd/mtd.h>
#include <linux/cdev.h>
#include <linux/irqreturn.h>

#include <atheros.h>

/*
 * IOCTL Command Codes
 */

#define ATH_GPIO_IOCTL_BASE			(0x01)
#define ATH_GPIO_IOCTL_CMD1      	(ATH_GPIO_IOCTL_BASE)
#define ATH_GPIO_IOCTL_CMD2      	(ATH_GPIO_IOCTL_BASE + 0x01)
#define ATH_GPIO_IOCTL_CMD3      	(ATH_GPIO_IOCTL_BASE + 0x02)
#define ATH_GPIO_IOCTL_CMD4      	(ATH_GPIO_IOCTL_BASE + 0x03)
#define ATH_GPIO_IOCTL_CMD5      	(ATH_GPIO_IOCTL_BASE + 0x04)
#define ATH_GPIO_IOCTL_CMD6      	(ATH_GPIO_IOCTL_BASE + 0x05)
#define ATH_GPIO_IOCTL_CMD7     	(ATH_GPIO_IOCTL_BASE + 0x06)
#define ATH_GPIO_IOCTL_CMD8     	(ATH_GPIO_IOCTL_BASE + 0x07)
#define ATH_GPIO_IOCTL_CMD9     	(ATH_GPIO_IOCTL_BASE + 0x08)
#define ATH_GPIO_IOCTL_CMD10     	(ATH_GPIO_IOCTL_BASE + 0x09)
#define ATH_GPIO_IOCTL_CMD11		(ATH_GPIO_IOCTL_BASE + 0x0a)
#define ATH_GPIO_IOCTL_CMD12		(ATH_GPIO_IOCTL_BASE + 0x0B)
#define ATH_GPIO_IOCTL_CMD13		(ATH_GPIO_IOCTL_BASE + 0x0C)
#define ATH_GPIO_IOCTL_CMD14		(ATH_GPIO_IOCTL_BASE + 0x0D)
#define ATH_GPIO_IOCTL_CMD15        (ATH_GPIO_IOCTL_BASE + 0x0E)
#define ATH_GPIO_IOCTL_CMD16        (ATH_GPIO_IOCTL_BASE + 0x0F)
#define ATH_GPIO_IOCTL_MAX          ATH_GPIO_IOCTL_CMD16


#define ATH_GPIO_MAGIC 				(0xB2)
#define	ATH_GPIO_BTN_READ			(_IOR(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD1, int))
#define	ATH_GPIO_WIFI_SW_READ		(_IOR(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD2, int))
#define	ATH_GPIO_LED_WRITE			(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD3, int))
#define ATH_GPIO_USB_LED1_WRITE		(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD4, int))
#define ATH_GPIO_INET_LED_WRITE		(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD5, int))
#define ATH_GPIO_WIFI_BTN_READ		(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD6, int))
#define	ATH_GPIO_BTN_PUSH_CNT_READ	(_IOR(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD7, int))
#define ATH_GPIO_WAN_LED_WRITE      (_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD8, int))
#define ATH_GPIO_WIFI_LED_WRITE     (_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD9, int))
#define	ATH_GPIO_POWER_LED_WRITE	(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD10, int))
#define	ATH_GPIO_WPS_LED_WRITE		(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD11, int))
#define ATH_GPIO_LAN_LED_WRITE 	    (_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD12, int))
/* By WANG Wenhu, May 20th, 2015. 
 * For MR3420(UN) 3.0 GPIO Configuration. 
 */
#define ATH_GPIO_REG_CTL_WRITE 	    (_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD13, int))
#define ATH_GPIO_USB_POWER_WRITE 	(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD14, int))
#define ATH_GPIO_RE_BTN_READ     	(_IOW(ATH_GPIO_MAGIC, ATH_GPIO_IOCTL_CMD15, int))
#if (CONFIG_GPIO_REG_CTL_LED == 1)
#define GPIP_REG_CTL_LED
#endif


/* By WANG Wenhu, May 20th, 2015. 
 * For MR3420(UN) 3.0 GPIO Configuration. 
 * In detail, these are for the shift register, 
 * which translates the five input GPIOs to the
 * eight output signals for the leds' control.
 */
#ifdef GPIP_REG_CTL_LED
#define SHIFT_REG_TW_FACTOR	                120
#define SHIFT_REG_SER_SRCLK_DELAY_FACTOR	150
#define SHIFT_REG_SRCLR_RCLK_DELAY_FACTOR	80

#define AP143_2_GPIO_SHIFT_OE     11
#define AP143_2_GPIO_SHIFT_SER    16
#define AP143_2_GPIO_SHIFT_SRCLK  15
#define AP143_2_GPIO_SHIFT_SRCLR  14
#define AP143_2_GPIO_SHIFT_RCLK   13

#define USB_EN_POWER_CTL_GPIO     0

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

#endif

/* for GPIO device number */
#define gpio_major      			(238)
#define gpio_minor      			(0)


/*
 * GPIO assignment
 */

#define ATH_GPIO_MIN				(0)

/* reset default button, default is GPIO12 */
#define RST_DFT_GPIO				(CONFIG_GPIO_RESET_FAC_BIT)
/* How long the user must press the button before Router rst, default is 5 */
#define RST_HOLD_TIME				(CONFIG_GPIO_FAC_RST_HOLD_TIME)

#if (CONFIG_GPIO_READY_STATUS_BIT >= ATH_GPIO_MIN)
/* system LED, default is GPIO13 	*/
#define SYS_LED_GPIO				(CONFIG_GPIO_READY_STATUS_BIT)
/* system LED's value when off, default is 1 */
#define SYS_LED_OFF         		(!CONFIG_GPIO_READY_STATUS_ON)
#define SYS_LED_ON         			(CONFIG_GPIO_READY_STATUS_ON)
#else
#undef SYS_LED_GPIO
#endif

#if (CONFIG_GPIO_LAN_STATUS_BIT >= ATH_GPIO_MIN)
#define LAN_LED_GPIO 		(CONFIG_GPIO_LAN_STATUS_BIT)
#define LAN_LED_ON                 (CONFIG_GPIO_LAN_STATUS_ON)
#define LAN_LED_OFF                (!CONFIG_GPIO_LAN_STATUS_ON)
#else
#undef  LAN_LED_GPIO
#endif

#if (CONFIG_GPIO_WPS_BUTTON_BIT >= ATH_GPIO_MIN)
#define WPS_BUTTON_GPIO			(CONFIG_GPIO_WPS_BUTTON_BIT)
#endif

/* QSS LED, default is GPIO3 */
#define TRICOLOR_LED_GREEN_PIN  	(CONFIG_GPIO_JUMPSTART_LED_BIT)
/* jump start LED's value when off */
#define JUMPSTART_LED_OFF     		(!CONFIG_GPIO_JUMPSTART_LED_ON)
/* jump start LED'S value when on, default is 0 */
#define JUMPSTART_LED_ON			(CONFIG_GPIO_JUMPSTART_LED_ON)

/* CN model may need not wifi switch */
#if (CONFIG_GPIO_WIFI_SWITCH_BIT >= ATH_GPIO_MIN)
/* WiFi switch, default is GPIO17 */
#define WIFI_RADIO_SW_GPIO			(CONFIG_GPIO_WIFI_SWITCH_BIT)
#define WIFI_BUTTON_HOLD_TIME		(CONFIG_GPIO_WIFI_BUTTON_HOLD_TIME)
#else
#undef WIFI_RADIO_SW_GPIO
#endif

#if (CONFIG_GPIO_WORKMODE_SWITCH_1ST_BIT >= ATH_GPIO_MIN)
/* Work Mode switch for 803, GPIO 14 and 16 */
#define WORKMODE_SWITCH_1ST_GPIO	(CONFIG_GPIO_WORKMODE_SWITCH_1ST_BIT)
#define WORKMODE_SWITCH_2ND_GPIO	(CONFIG_GPIO_WORKMODE_SWITCH_2ND_BIT)
#else
#undef WORKMODE_SWITCH_1ST_GPIO
#endif
/*Work mode swtich for WR843NV2,gpio 2, low is WISP, heigh is Normal Router. modify by ZQQ, 2015.4.9*/
#if (CONFIG_GPIO_WORKMODE_SWITCH_2ND_BIT >= ATH_GPIO_MIN)
#define WORKMODE_SWITCH_2ND_GPIO	(CONFIG_GPIO_WORKMODE_SWITCH_2ND_BIT)
#else
#undef WORKMODE_SWITCH_2ND_GPIO
#endif

#if (CONFIG_GPIO_INTERNET_LED_BIT >= ATH_GPIO_MIN)
/* Internet LED for 803, default is GPIO11 */
#define INET_LED_GPIO				(CONFIG_GPIO_INTERNET_LED_BIT)
/* Internet LED's vaule when on, default is 0 */
#define INET_LED_ON					(CONFIG_GPIO_INTERNET_LED_ON)
/* Internet LED's vaule when off, default is 1 */
#define	INET_LED_OFF				(!CONFIG_GPIO_INTERNET_LED_ON)
#else
#undef INET_LED_GPIO
#endif

/*added by ZQQ*/
//#define CONFIG_GPIO_INTERNET_LED_COLOR 16,  define in kernel.conf
#if (CONFIG_GPIO_INTERNET_LED_COLOR >= ATH_GPIO_MIN)
#define INET_LED_COLOR_GPIO		(CONFIG_GPIO_INTERNET_LED_COLOR)
#define INET_LED_COLOR_ON		(CONFIG_GPIO_INTERNET_LED_COLOR_ON)
#define	INET_LED_COLOR_OFF		(!CONFIG_GPIO_INTERNET_LED_COLOR_ON)
#else
#undef INET_LED_COLOR_GPIO
#endif


#if (CONFIG_GPIO_WIFI_LED_BIT >= ATH_GPIO_MIN)
#define WIFI_LED_GPIO               (CONFIG_GPIO_WIFI_LED_BIT)
#define WIFI_LED_ON                 (CONFIG_GPIO_WIFI_LED_ON)
#define WIFI_LED_OFF                (!CONFIG_GPIO_WIFI_LED_ON)
#else
#undef  WIFI_LED_GPIO
#endif

#if (CONFIG_GPIO_USB_LED_BIT >= ATH_GPIO_MIN)
/* USB LED, default is GPIO11 */
#define AP_USB_LED_GPIO     		(CONFIG_GPIO_USB_LED_BIT)
/* USB LED's value when off */
#define USB_LED_OFF         		(!CONFIG_GPIO_USB_LED_ON)
/* USB LED's value when on, default is 0 */
#define USB_LED_ON          		(CONFIG_GPIO_USB_LED_ON)
/* usb power switch, default is 4 */
#define USB_POWER_SW_GPIO			(CONFIG_GPIO_USB_SWITCHFOR3G_BIT)
#define USB_POWER_ON				(1)
#define USB_POWER_OFF				(!USB_POWER_ON)
#else
#undef AP_USB_LED_GPIO
#endif

#if (CONFIG_GPIO_RE_BTN_BIT >= ATH_GPIO_MIN)
#define RE_BTN_GPIO               (CONFIG_GPIO_RE_BTN_BIT)
#define RE_BTN_HOLD_TIME       (CONFIG_GPIO_RE_BTN_HOLD_TIME)
#define RE_WPS_LED_GPIO      (CONFIG_GPIO_RE_LED_BIT)
#define RE_WPS_LED_OFF           (!CONFIG_GPIO_RE_LED_ON)
#define RE_WPS_LED_ON            (CONFIG_GPIO_RE_LED_ON)
#else
#undef  RE_BTN_GPIO
#endif

static int counter = 0;

static int bBlockWps = 1;
static int btnPushCnt = 0;
static struct timer_list rst_timer;

#ifdef WIFI_RADIO_SW_GPIO
/* local var for wifi-switch */
static struct timer_list wifi_button_timer;
#ifdef CONFIG_GPIO_740
static int ignore_wifibutton = 0;
#else
static int ignore_wifibutton = 1;
#endif

static int wifi_button_flag = 0;

/* some models use wifi button for wps */
#ifdef CONFIG_GPIO_740
static int l_bMultiUseWifiButton = 1;
#else
static int l_bMultiUseWifiButton = 0;
#endif // CONFIG_GPIO_740

#endif

/**
 * If we need to software control power bit
 * (when upgrade, blink it), we should define
 * this bit here
 * when wireless is not calc, should blink faster...
 */
#if defined(CONFIG_GPIO_POWER_LED_BIT)
#if (CONFIG_GPIO_POWER_LED_BIT >= 0)
#define POWER_LED_GPIO_BIT   (CONFIG_GPIO_POWER_LED_BIT)
#define POWER_LED_GPIO_ON    (CONFIG_GPIO_POWER_LED_ON)
#else
#undef  POWER_LED_GPIO_BIT
#endif 
#else
#undef  POWER_LED_GPIO_BIT
#endif

/* control params for reset button reuse, by zjg, 13Apr10 */
static int l_bMultiUseResetButton		=	0;

static int l_bWaitForQss				=   1;


#ifdef AP_USB_LED_GPIO
int g_usbLedBlinkCountDown = 1;
EXPORT_SYMBOL(g_usbLedBlinkCountDown);
#endif

#ifdef RE_BTN_GPIO
static struct timer_list re_button_timer;
static int ignore_rebutton = 1;
static int re_button_flag = 0;
#endif

#if (1 == CONFIG_WPS_810EU)
static int sta_wps_flag = 0;
#endif

/*
 * GPIO interrupt stuff
 */
typedef enum 
{
    INT_TYPE_EDGE,
    INT_TYPE_LEVEL,
} ath_gpio_int_type_t;

typedef enum 
{
    INT_POL_ACTIVE_LOW,
    INT_POL_ACTIVE_HIGH,
} ath_gpio_int_pol_t;

/*  by huangwenzhong, 11Oct13 */
typedef enum
{
	ATH_GPIO_LEVEL_LOW = 0,
	ATH_GPIO_LEVEL_HIGH,
}ath_gpio_level;
#ifdef CONFIG_GPIO_OPTION66
// addd by xiejiabai@tp-link.net
/* used to calc how many unit of RST_HOLD_TIME rst button is hold */
static int rstPressTimeCounter = 0;
#define OPTION66_RST_HOLD_TIME 30
#define LAN_PORT_1_GPIO_841V11 11
#define LAN_PORT_2_GPIO_841V11 14
#define LAN_PORT_3_GPIO_841V11 15
#define LAN_PORT_4_GPIO_841V11 16
static int GPIO_LIST_841V11[] = {POWER_LED_GPIO_BIT,INET_LED_COLOR_GPIO,TRICOLOR_LED_GREEN_PIN,INET_LED_GPIO,LAN_PORT_1_GPIO_841V11,WIFI_LED_GPIO,LAN_PORT_2_GPIO_841V11,LAN_PORT_3_GPIO_841V11,LAN_PORT_4_GPIO_841V11};
#endif

/*changed by hujia.*/
typedef irqreturn_t(*sc_callback_t)(int, void *, void *, void *);
static sc_callback_t registered_cb = NULL;
static char* cb_name = NULL;
static void *cb_arg = NULL;
/*add by hujia.*/
static void *cb_pushtime = NULL;
/*end add.*/

/* for dual band, add by chenyuqi Jul2015 */
static sc_callback_t registered_cb_5g = NULL;
static char* cb_name_5g = NULL;
static void *cb_arg_5g = NULL;
static void *cb_pushtime_5g = NULL;
/* end add. */

/* top dir entry */
static struct proc_dir_entry *simple_config_entry = NULL;
static struct proc_dir_entry *simulate_push_button_entry = NULL;
static struct proc_dir_entry *tricolor_led_entry = NULL;
static struct proc_dir_entry *bicolour_led_entry = NULL;
#ifdef RE_BTN_GPIO
static struct proc_dir_entry *re_led_entry = NULL;
#endif

/* added by zjg, 12Apr10 */
static struct proc_dir_entry *multi_use_reset_button_entry = NULL;

#ifdef WIFI_RADIO_SW_GPIO
static struct proc_dir_entry *multi_use_wifi_button_entry = NULL;
static struct proc_dir_entry *wifi_button_entry = NULL;
#endif

#ifdef AP_USB_LED_GPIO
/* ZJin 100317: for 3g usb led blink feature, use procfs simple config. */
static struct proc_dir_entry *usb_led_blink_entry = NULL;

/*added by  ZQQ<10.06.02 for usb power*/
static struct proc_dir_entry *usb_power_entry = NULL;
#endif

#ifdef WORKMODE_SWITCH_1ST_GPIO
static struct proc_dir_entry *workmode_entry = NULL;
#endif

#ifdef CONFIG_WPS_HOST_CLIENT_MULTIPLE
/*  wps_client_mode  
 *  1: works in the mode with wps acts a client, such as repeater and client mode
 *  0: works in the mode with wps acts a host 
 *  The val is assigned by web_server, read by kernel
 */
static u_int32_t wps_client_mode = 0;
static struct proc_dir_entry *wps_client_mode_entry = NULL; 

/*  wps_client_button
 *  1: wps botton is pressed and wps_client_mode==1, then wps starts to work as a client
 *  0: none
 *  when works in repeater,client, web_server keep detecting the value.
 */
static u_int32_t wps_client_button = 0;
static struct proc_dir_entry *wps_client_button_entry = NULL;
#endif

#ifndef SYS_LED_GPIO
static struct proc_dir_entry *blockWps_entry = NULL;
#endif

/*
 * General GPIO Write & Read Proc
 */
static struct proc_dir_entry *gpio_write_entry = NULL;

/*  by huangwenzhong, 06Mar13 */
/* 根据GPIO的编号获得对应的FUNCTION寄存器地址 */
static u32 ath_get_gpio_function(int gpio)
{
	int i = (gpio / 4);
	u32 functionx = -1;
	switch (i)
	{
		case 0:
			functionx = ATH_GPIO_OUT_FUNCTION0;
			break;
		case 1:
			functionx = ATH_GPIO_OUT_FUNCTION1;
			break;
		case 2:
			functionx = ATH_GPIO_OUT_FUNCTION2;
			break;
		case 3:
			functionx = ATH_GPIO_OUT_FUNCTION3;
			break;
		case 4:
			functionx = ATH_GPIO_OUT_FUNCTION4;
			break;
		case 5:
			functionx = ATH_GPIO_OUT_FUNCTION5;
			break;
		default:
			functionx = -1;
	}
	return functionx;
}
/* 根据GPIO编号获取对应的8位设置位在寄存器中的偏移量 */
#define ATH_GET_GPIO_SHIFT_BIT(x)			(((x) % 4) * 8)



void ath_gpio_config_int(int gpio,
			 ath_gpio_int_type_t type,
			 ath_gpio_int_pol_t polarity)
{
	u32 val;

    ath_reg_rmw_clear(ATH_GPIO_INT_ENABLE, (1 << gpio));
	/*
	 * allow edge sensitive/rising edge too
	 */
	if (type == INT_TYPE_LEVEL) {
		/* level sensitive */
		ath_reg_rmw_set(ATH_GPIO_INT_TYPE, (1 << gpio));
	} else {
		/* edge triggered */
		val = ath_reg_rd(ATH_GPIO_INT_TYPE);
		val &= ~(1 << gpio);
		ath_reg_wr(ATH_GPIO_INT_TYPE, val);
	}

	if (polarity == INT_POL_ACTIVE_HIGH) {
		ath_reg_rmw_set(ATH_GPIO_INT_POLARITY, (1 << gpio));
	} else {
		val = ath_reg_rd(ATH_GPIO_INT_POLARITY);
		val &= ~(1 << gpio);
		ath_reg_wr(ATH_GPIO_INT_POLARITY, val);
	}

	ath_reg_rmw_set(ATH_GPIO_INT_ENABLE, (1 << gpio));
}

void ath_gpio_config_output(int gpio)
{
#if defined(CONFIG_MACH_AR934x) || \
    defined(CONFIG_MACH_QCA955x) || \
    defined(CONFIG_MACH_QCA953x) || \
	defined(CONFIG_MACH_QCA956x)
	ath_reg_rmw_clear(ATH_GPIO_OE, (1 << gpio));
#else
	ath_reg_rmw_set(ATH_GPIO_OE, (1 << gpio));
#endif
}
EXPORT_SYMBOL(ath_gpio_config_output);

void ath_gpio_config_input(int gpio)
{
#if defined(CONFIG_MACH_AR934x) || \
    defined(CONFIG_MACH_QCA955x) || \
    defined(CONFIG_MACH_QCA953x) || \
	defined(CONFIG_MACH_QCA956x)
	ath_reg_rmw_set(ATH_GPIO_OE, (1 << gpio));
#else
	ath_reg_rmw_clear(ATH_GPIO_OE, (1 << gpio));
#endif
}

void ath_gpio_out_val(int gpio, int val)
{
	if (val & 0x1) {
		ath_reg_rmw_set(ATH_GPIO_OUT, (1 << gpio));
	} else {
		ath_reg_rmw_clear(ATH_GPIO_OUT, (1 << gpio));
	}
}
EXPORT_SYMBOL(ath_gpio_out_val);

int ath_gpio_in_val(int gpio)
{
	return ((1 << gpio) & (ath_reg_rd(ATH_GPIO_IN)));
}

static void
ath_gpio_intr_enable(unsigned int irq)
{
	ath_reg_rmw_set(ATH_GPIO_INT_MASK,
				(1 << (irq - ATH_GPIO_IRQ_BASE)));
}

static void
ath_gpio_intr_disable(unsigned int irq)
{
	ath_reg_rmw_clear(ATH_GPIO_INT_MASK,
				(1 << (irq - ATH_GPIO_IRQ_BASE)));
}

static unsigned int
ath_gpio_intr_startup(unsigned int irq)
{
	ath_gpio_intr_enable(irq);
	return 0;
}

static void
ath_gpio_intr_shutdown(unsigned int irq)
{
	ath_gpio_intr_disable(irq);
}

static void
ath_gpio_intr_ack(unsigned int irq)
{
	ath_gpio_intr_disable(irq);
}

static void
ath_gpio_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ath_gpio_intr_enable(irq);
}

static int
ath_gpio_intr_set_affinity(unsigned int irq, const struct cpumask *dest)
{
	/*
	 * Only 1 CPU; ignore affinity request
	 */
	return 0;
}

struct irq_chip /* hw_interrupt_type */ ath_gpio_intr_controller = {
	.name = "ATH GPIO",
	.startup = ath_gpio_intr_startup,
	.shutdown = ath_gpio_intr_shutdown,
	.enable = ath_gpio_intr_enable,
	.disable = ath_gpio_intr_disable,
	.ack = ath_gpio_intr_ack,
	.end = ath_gpio_intr_end,
	.eoi = ath_gpio_intr_end,
	.set_affinity = ath_gpio_intr_set_affinity,
};

void ath_gpio_irq_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + ATH_GPIO_IRQ_COUNT; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		//irq_desc[i].chip = &ath_gpio_intr_controller;
		set_irq_chip_and_handler(i, &ath_gpio_intr_controller,
					 handle_percpu_irq);
	}
}

void register_simple_config_callback(char *cbname, void *callback, void *arg1, void *arg2)
{
    if(NULL == registered_cb)
    {
    	cb_name = cbname;
    	registered_cb = (sc_callback_t) callback;
        cb_arg = arg1;
        /* add by hujia.*/
        cb_pushtime=arg2;
        /*end add.*/
    }
    else if(NULL == registered_cb_5g)
    {
    	cb_name_5g = cbname;
    	registered_cb_5g = (sc_callback_t) callback;
        cb_arg_5g = arg1;
        cb_pushtime_5g=arg2;
    }
}
EXPORT_SYMBOL(register_simple_config_callback);

void unregister_simple_config_callback (char *name)
{
	if (cb_name && strcmp(cb_name, name)==0) {
        registered_cb = NULL;
        cb_arg = NULL;
    	cb_pushtime = NULL;
    	cb_name = NULL;
    }
    else if (cb_name_5g && strcmp(cb_name_5g, name)==0) {
        registered_cb_5g = NULL;
        cb_arg_5g = NULL;
    	cb_pushtime_5g = NULL;
    	cb_name_5g = NULL;
	}
}
EXPORT_SYMBOL(unregister_simple_config_callback);

#if(1 == CONFIG_WPS_810EU)
void sta_wps_notify ()
{
    printk("sta_wps_notify\n");
    sta_wps_flag = 1;
}
EXPORT_SYMBOL(sta_wps_notify);
#endif


/******* begin ioctl stuff **********/
#undef CONFIG_GPIO_DEBUG
#ifdef CONFIG_GPIO_DEBUG
void print_gpio_regs(char* prefix)
{
	printk("\n-------------------------%s---------------------------\n", prefix);
	printk("ATH_GPIO_OE:%#X\n", ath_reg_rd(ATH_GPIO_OE));
	printk("ATH_GPIO_IN:%#X\n", ath_reg_rd(ATH_GPIO_IN));
	printk("ATH_GPIO_OUT:%#X\n", ath_reg_rd(ATH_GPIO_OUT));
	//printk("ATH_GPIO_SET:%#X\n", ath_reg_rd(ATH_GPIO_SET));
	//printk("ATH_GPIO_CLEAR:%#X\n", ath_reg_rd(ATH_GPIO_CLEAR));
	printk("ATH_GPIO_INT_ENABLE:%#X\n", ath_reg_rd(ATH_GPIO_INT_ENABLE));
	printk("ATH_GPIO_INT_TYPE:%#X\n", ath_reg_rd(ATH_GPIO_INT_TYPE));
	printk("ATH_GPIO_INT_POLARITY:%#X\n", ath_reg_rd(ATH_GPIO_INT_POLARITY));
	printk("ATH_GPIO_INT_PENDING:%#X\n", ath_reg_rd(ATH_GPIO_INT_PENDING));
	printk("ATH_GPIO_INT_MASK:%#X\n", ath_reg_rd(ATH_GPIO_INT_MASK));
	printk("\n-------------------------------------------------------\n");
}
#endif


/* By WANG Wenhu, May 20th, 2015. 
 * For MR3420(UN) 3.0 GPIO Configuration. 
 */
#ifdef GPIP_REG_CTL_LED
void shift_register_set(int val)
{
	int index = 0;
	int value = 0;
	ath_gpio_out_val(AP143_2_GPIO_SHIFT_RCLK, 0);
	
	for(index = 0; index < SHIFT_REGISTER_OUTPUT_NUM; index++)
	{
		value = (val >> index) & 0x1;
		ath_gpio_out_val(AP143_2_GPIO_SHIFT_SER, value);
		ndelay(SHIFT_REG_SER_SRCLK_DELAY_FACTOR);
		ath_gpio_out_val(AP143_2_GPIO_SHIFT_SRCLK, 1);
		ndelay(SHIFT_REG_TW_FACTOR);
		ath_gpio_out_val(AP143_2_GPIO_SHIFT_SRCLK, 0);
	}	

	ath_gpio_out_val(AP143_2_GPIO_SHIFT_RCLK, 1);
	ndelay(SHIFT_REG_TW_FACTOR);
	ath_gpio_out_val(AP143_2_GPIO_SHIFT_RCLK, 0);
	ndelay(SHIFT_REG_TW_FACTOR);
}
#endif


/*
 * Genenal GPIO write & read
 */
static int simple_gpio_write (struct file *file, const char *buf,
				unsigned long count, void *data)
{
    u_int32_t val = 0;
    u_int32_t port = 0;

    if (sscanf(buf, "%d %d", &port, &val) < 0)
    {
        return -EINVAL;
    }

    if (val < 0 || val > 1)
    {
        return -EINVAL;
    }
    
    printk("port is %d, val is %s\n", port, val == 1 ? "on" : "off");
	ath_gpio_out_val(port, val);
   
	return count; 
}

static int simple_gpio_read(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
    return 0;
}

/*
 *  USB GPIO control
 */

void ap_usb_led_on(void)
{
#ifdef AP_USB_LED_GPIO
	if (AP_USB_LED_GPIO >= ATH_GPIO_MIN)
	{
		ath_gpio_out_val(AP_USB_LED_GPIO, USB_LED_ON);
		}
#endif
	}

EXPORT_SYMBOL(ap_usb_led_on);

void ap_usb_led_off(void)
{
#ifdef AP_USB_LED_GPIO
	if (AP_USB_LED_GPIO >= ATH_GPIO_MIN)
	{
		ath_gpio_out_val(AP_USB_LED_GPIO, USB_LED_OFF);
	}
#endif
}

EXPORT_SYMBOL(ap_usb_led_off);

#ifdef WPS_BUTTON_GPIO
/* for indepentdent wps button */
irqreturn_t wps_irq(int cpl, void *dev_id)
{
	if ( (!bBlockWps) && (l_bWaitForQss) &&
     (ath_gpio_in_val(WPS_BUTTON_GPIO) >> WPS_BUTTON_GPIO)) 
	{
		/*return registered_cb (cpl, cb_arg, regs);*/
        ath_gpio_config_int (WPS_BUTTON_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
#ifdef CONFIG_WPS_HOST_CLIENT_MULTIPLE
        /* when wps acts as client(repeater,client mode) */
        if(wps_client_mode)
        {
            wps_client_button = 1;
            return IRQ_HANDLED;
        }
#endif
        if(registered_cb != NULL)
            registered_cb(cpl, cb_arg, NULL, cb_pushtime);
        if(registered_cb_5g != NULL)
            registered_cb_5g(cpl, cb_arg_5g, NULL, cb_pushtime_5g);
    }
    return IRQ_HANDLED;    
}
#endif

/* for reset button push by user, >=5s */
static int ignore_rstbutton = 1;

/* irq handler for reset button */
irqreturn_t rst_irq(int cpl, void *dev_id)
{
    //printk("ignore_rstbutton = %d\n", ignore_rstbutton);
	//print_gpio_regs(irq_count++);
#if (1==CONFIG_GPIO_801)
    if (ignore_rstbutton)
#else
    if (ignore_rstbutton && !(ath_gpio_in_val(RST_DFT_GPIO) >> RST_DFT_GPIO))
#endif
	{
#if (1==CONFIG_GPIO_801)
		ath_gpio_config_int(RST_DFT_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
#else
		ath_gpio_config_int(RST_DFT_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_HIGH);
#endif
		ignore_rstbutton = 0;

		mod_timer(&rst_timer, jiffies + RST_HOLD_TIME * HZ);

		return IRQ_HANDLED;
	}

#if (1 != CONFIG_GPIO_801)
    else if (!ignore_rstbutton && (ath_gpio_in_val(RST_DFT_GPIO) >> RST_DFT_GPIO))
    {
#endif
        ath_gpio_config_int (RST_DFT_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
	    ignore_rstbutton = 1;
	    btnPushCnt++;

	    printk("Reset button pressed.\n");
#ifdef CONFIG_GPIO_OPTION66
		/* button is released here.
		 * if rst button is hold for more than 5s but less than 30s and released,
		 * reset to ISP config */
		if ((rstPressTimeCounter >=1) && (rstPressTimeCounter <OPTION66_RST_HOLD_TIME / RST_HOLD_TIME))
		{
			counter = rstPressTimeCounter;
			rstPressTimeCounter = 0;
			printk("reset to ISP config.\n");
		}
#endif
		
#ifndef CONFIG_GPIO_740
	    /* mark reset status, by zjg, 12Apr10 */
	    if ((registered_cb != NULL) && (!bBlockWps) && (l_bMultiUseResetButton) && (l_bWaitForQss)) 
	    {
		    /*return registered_cb (cpl, cb_arg, regs);*/
		    return registered_cb(cpl, cb_arg, NULL, cb_pushtime);
        }
#endif
#if (1 != CONFIG_GPIO_801)
    }
#endif
    return IRQ_HANDLED;
}

void check_rst(unsigned long nothing)
{
	if (!ignore_rstbutton)
	{
		
		/* check the level first, make sure it is still low level */
		if ((ath_gpio_in_val(RST_DFT_GPIO) >> RST_DFT_GPIO) == ATH_GPIO_LEVEL_LOW)
		{
#ifdef CONFIG_GPIO_OPTION66	
			rstPressTimeCounter ++;
			/* 
			 * under CONFIG_GPIO_OPTION66, if pressed for more than 30s,
			 * execute factory reset; if pressed for only more than 5s,
			 * execute ISP reset;
			 */
			if ( rstPressTimeCounter == 1)
			{
				// if the pressed time is more than 5s, we light all LEDs on
				// only works for 841nv11!!!
				int gpioIndex = 0;
				for (gpioIndex=0;gpioIndex<sizeof(GPIO_LIST_841V11)/sizeof(int);gpioIndex++)
				{
					ath_reg_rmw_clear(ath_get_gpio_function(GPIO_LIST_841V11[gpioIndex]),
									  0xff << ATH_GET_GPIO_SHIFT_BIT(GPIO_LIST_841V11[gpioIndex]));
					ath_gpio_config_output(GPIO_LIST_841V11[gpioIndex]);
					ath_gpio_out_val(GPIO_LIST_841V11[gpioIndex], 0);
				}
				/*
				 * wan led is controlled by 2 gpio,
				 * green: inet_gpio : 0, inet_gpio_color : 1
				 */
				ath_gpio_out_val(INET_LED_COLOR_GPIO, 1);
			}
				
			
			if ( rstPressTimeCounter < OPTION66_RST_HOLD_TIME / RST_HOLD_TIME)
			{
				/* reset timer and start new 5s waiting again */
				printk("press rst button for %d seconds\n", ( rstPressTimeCounter ) * RST_HOLD_TIME);
				mod_timer(&rst_timer, jiffies + RST_HOLD_TIME * HZ);
			}
			else
			{
				/* 
				 * reach maxium 30s, no matter the reset button is released, 
				 * we should take action such as reboot 
				 */
				counter = rstPressTimeCounter;
				printk ("restoring tp-link factory default...\n");
			}
#else
			counter ++;
			printk ("restoring factory default...\n");
#endif
	    }
#ifdef CONFIG_GPIO_740
        else{
            ignore_rstbutton = 1;
        }
#else
		/* to mark reset status, forbid QSS, added by zjg, 12Apr10 */
		if (l_bMultiUseResetButton)
        {
			l_bWaitForQss	= 0;
		}
#endif
	}
}

static int multi_use_reset_button_read (char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", l_bMultiUseResetButton);
}

static int multi_use_reset_button_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	/* only admit "0" or "1" */
	if ((val < 0) || (val > 1))
		return -EINVAL;	

	l_bMultiUseResetButton = val;
	
	return count;

}

#ifdef WIFI_RADIO_SW_GPIO
/* irq handler for wifi switch */
static irqreturn_t wifi_sw_irq(int cpl, void *dev_id)
{
#if (1==CONFIG_GPIO_801)
    if (ignore_wifibutton)
#else
    if (ignore_wifibutton && !(ath_gpio_in_val(WIFI_RADIO_SW_GPIO) >> WIFI_RADIO_SW_GPIO))
#endif
{
#if (1==CONFIG_GPIO_801)
		ath_gpio_config_int (WIFI_RADIO_SW_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
#else
		ath_gpio_config_int (WIFI_RADIO_SW_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_HIGH);
#endif
		ignore_wifibutton = 0;

		mod_timer(&wifi_button_timer, jiffies + WIFI_BUTTON_HOLD_TIME * HZ);	/* 2sec */

		if (l_bMultiUseWifiButton)
		{
			l_bWaitForQss = 1;
		}

		return IRQ_HANDLED;
	}
#if (1 != CONFIG_GPIO_801)
	else if (!ignore_wifibutton && (ath_gpio_in_val(WIFI_RADIO_SW_GPIO) >> WIFI_RADIO_SW_GPIO))
	{
#endif
		ath_gpio_config_int (WIFI_RADIO_SW_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
		ignore_wifibutton = 1;

		printk("WIFI button pressed.\n");

		if ((registered_cb != NULL) && (!bBlockWps) && (l_bMultiUseWifiButton) && (l_bWaitForQss)) 
		{
			return registered_cb(cpl, cb_arg, NULL, cb_pushtime);
		}
#if (1 != CONFIG_GPIO_801)
	}
#endif
            return IRQ_HANDLED;
}

static void wifi_sw_check(unsigned long nothing)
{
	/* if user keep push button more than 2s, 
	ignore_wifibutton will keep 0, 
	or ignore_wifibutton will be 1 */
	if (!ignore_wifibutton)
	{
		printk ("Switch Wi-Fi...\n");

		/* check the level first, make sure it is still low level */
		if ((ath_gpio_in_val(WIFI_RADIO_SW_GPIO) >> WIFI_RADIO_SW_GPIO) == ATH_GPIO_LEVEL_LOW)
		{
    		wifi_button_flag++;
	    }

		if (l_bMultiUseWifiButton)
	    {
			l_bWaitForQss	= 0;
		}
	}
}

static int multi_use_wifi_button_read (char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", l_bMultiUseWifiButton);
	}

static int multi_use_wifi_button_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	/* only admit "0" or "1" */
	if ((val < 0) || (val > 1))
		return -EINVAL;	

	l_bMultiUseWifiButton = val;
	
	return count;

}

static int wifi_button_proc_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
	u_int32_t ret = 0;

	u_int32_t reg_value1 = 0;

	reg_value1 = ath_gpio_in_val(WIFI_RADIO_SW_GPIO);

	reg_value1 = reg_value1 >> WIFI_RADIO_SW_GPIO;

	ret = reg_value1;

    return sprintf (page, "%d\n", ret);
}

static int wifi_button_proc_write(struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	if ((val < 0) || (val > 1))
		return -EINVAL;

	printk("%s %d: write value = %d\r\n",__FUNCTION__,__LINE__,val);

	return count;
}
#endif

#ifdef RE_BTN_GPIO
/* irq handler for re button */
irqreturn_t re_btn_irq(int cpl, void *dev_id) 
{
    //printk("ignore_rstbutton = %d\n", ignore_rstbutton);
    //print_gpio_regs(irq_count++);
    if (ignore_rebutton &&
        (ath_gpio_in_val(RE_BTN_GPIO) >> RE_BTN_GPIO) == ATH_GPIO_LEVEL_LOW)
    {
        printk("Re button down.\n");
        ath_gpio_config_int(RE_BTN_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_HIGH);
        ignore_rebutton = 0;
        
        mod_timer(&re_button_timer, jiffies + RE_BTN_HOLD_TIME * HZ);

        return IRQ_HANDLED;
    }

    ignore_rebutton = 1;
    ath_gpio_config_int (RE_BTN_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);

    printk("Re button up.\n");

    return IRQ_HANDLED;
}
static void re_check(unsigned long nothing)
{
    /* if user keep push button more than 2s, 
    ignore_wifibutton will keep 0, 
    or ignore_wifibutton will be 1 */
    if (!ignore_rebutton)
    {
        printk ("RE processing...\n");

        /* check the level first, make sure it is still low level */
        if ((ath_gpio_in_val(RE_BTN_GPIO) >> RE_BTN_GPIO) == ATH_GPIO_LEVEL_LOW)
        {
            re_button_flag ++;
             printk("re check low\n");
        }
    }
}
#endif

static int push_button_read(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	return 0;
}

static int push_button_write(struct file *file, const char *buf,
				unsigned long count, void *data)
{
	if (registered_cb) {
		/*changed by hujia.*/
		/* registered_cb (0, cb_arg, 0);*/
		registered_cb (0, cb_arg, NULL, NULL);
	}
	return count;
}

/* for USB */
static int usb_led_blink_read (char *page, char **start, off_t off,
					int count, int *eof, void *data)
{
#ifdef AP_USB_LED_GPIO
    return sprintf (page, "%d\n", g_usbLedBlinkCountDown);
#else
	return sprintf (page, "%d\n", 0);
#endif
}

static int usb_led_blink_write (struct file *file, const char *buf,
					unsigned long count, void *data)
{
#ifdef AP_USB_LED_GPIO
	u_int32_t val;

	if (sscanf(buf, "%d", &val) != 1)
		return -EINVAL;
    
	/* modified by hejian for 3G led logic, by 120607 */
	/* added 2 cases, case 2 means turn led off, case 3 mean turn led on */
	if ((val < 0) || (val > 3))
		return -EINVAL;

	if (val == 2)
	{
		ap_usb_led_off();
        }
	else if (val == 3)
	{
		ap_usb_led_on();
    }
	else
	{
		g_usbLedBlinkCountDown = val;
	}
#endif
	return count;
}

/* For 3G switch */
/*added by ZQQ,10.06.02*/
static int usb_power_read (char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
	printk("%s %d: Current Not Supported!\n", __FUNCTION__, __LINE__);
    return 0;
}

static int usb_power_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
#	ifdef AP_USB_LED_GPIO
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	if ((val < 0) || (val > 1))
		return -EINVAL;

	if (USB_POWER_ON == val)
	{
	    printk("%s %d: write gpio(Port%d):value = %d\r\n",__FUNCTION__,__LINE__,
		   USB_POWER_SW_GPIO, USB_POWER_ON);
		ath_gpio_out_val(USB_POWER_SW_GPIO, USB_POWER_ON);
	}
	else
	{
	    printk("%s %d: write gpio(Port%d):value = %d\r\n",__FUNCTION__,__LINE__,
		   USB_POWER_SW_GPIO, USB_POWER_OFF);
		ath_gpio_out_val(USB_POWER_SW_GPIO, USB_POWER_OFF);
	}
#endif
	return count;
}


#ifdef WORKMODE_SWITCH_1ST_GPIO
static int workmode_proc_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
	u_int32_t ret = 0;
	
	u_int32_t reg_value1 = 0;
	u_int32_t reg_value2 = 0;

	reg_value1 = ath_gpio_in_val(WORKMODE_SWITCH_1ST_GPIO);
	//printk("%s %d reg_value1 = %d, WORKMODE_SWITCH_1ST_GPIO = %d \r\n", __FUNCTION__,__LINE__,reg_value1, WORKMODE_SWITCH_1ST_GPIO);
	reg_value1 = reg_value1 >> WORKMODE_SWITCH_1ST_GPIO;
	//printk("%s %d reg_value1 = %d \r\n", __FUNCTION__,__LINE__,reg_value1);

#ifdef WORKMODE_SWITCH_2ND_GPIO
	reg_value2 = ath_gpio_in_val(WORKMODE_SWITCH_2ND_GPIO);
	reg_value2 = reg_value2 >> WORKMODE_SWITCH_2ND_GPIO;
#endif
	//printk("%s %d reg_value1 = %d \r\n", __FUNCTION__,__LINE__,reg_value1);
	ret = (reg_value2 << 1) | reg_value1;
	//printk ("%s %d ret = %d, WORKMODE_SWITCH_1ST_GPIO = %d, reg_value1 = %d, reg_value2 = %d\r\n", __FUNCTION__, __LINE__, ret, WORKMODE_SWITCH_1ST_GPIO, reg_value1, reg_value2);
	//printk("%s %d ret = %d \r\n", __FUNCTION__, __LINE__, ret);
    return sprintf (page, "%d\n", ret);
}

static int workmode_proc_write(struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	if ((val < 0) || (val > 1))
		return -EINVAL;

	printk("%s %d: write value = %d\r\n",__FUNCTION__,__LINE__,val);
	
	return count;
}
#endif

#ifdef CONFIG_WPS_HOST_CLIENT_MULTIPLE
static int wps_client_mode_proc_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", wps_client_mode);
}

static int wps_client_mode_proc_write(struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	if ((val < 0) || (val > 1))
		return -EINVAL;

	printk("%s %d: write value = %d\r\n",__FUNCTION__,__LINE__,val);
	wps_client_mode = val;
	return count;
}

static int wps_client_button_proc_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", wps_client_button);
}

static int wps_client_button_proc_write(struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

	if ((val < 0) || (val > 1))
		return -EINVAL;

	printk("%s %d: write value = %d\r\n",__FUNCTION__,__LINE__,val);
	wps_client_button = val;
	return count;
}
#endif

#ifndef SYS_LED_GPIO
static int blockWps_proc_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", bBlockWps);
}

static int blockWps_proc_write(struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val = 0;

	if (sscanf(buf, "%d", &val) != 1)
	{
        return -EINVAL;
	}

	if ((val < 0) || (val > 1))
	{
		return -EINVAL;
	}

	printk("%s %d: write value = %d\r\n",__FUNCTION__,__LINE__,val);
	bBlockWps = val;

	return count;
}
#endif


typedef enum 
{
	LED_STATE_OFF		= 0,
	LED_STATE_GREEN		= 1, 
	LED_STATE_YELLOW	= 2,
	LED_STATE_ORANGE	= 3,
	LED_STATE_MAX		= 4,
} led_state_e;

static led_state_e gpio_tricolorled = LED_STATE_OFF;
static led_state_e gpio_bicolour_colorled = LED_STATE_OFF;


static int gpio_tricolor_led_read (char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", gpio_tricolorled);
}


#if defined(INET_LED_GPIO) && defined(INET_LED_COLOR_GPIO)
static int gpio_bicolour_led_write (struct file *file, const char *buf,
			unsigned long count, void *data)
{
	 u_int32_t val, green_led_onoff = 0, orange_led_onoff = 0;
	 if (sscanf(buf, "%d", &val) != 1)
		return -EINVAL;

	 if (val >= LED_STATE_MAX)
        return -EINVAL;

	 switch (val) {
        case LED_STATE_GREEN:
			/*printk("%s %d: set LED_STATE_GREEN\r\n", __FUNCTION__, __LINE__);*/
        	ath_gpio_out_val(INET_LED_GPIO, INET_LED_ON);
        	ath_gpio_out_val(INET_LED_COLOR_GPIO, INET_LED_COLOR_OFF);
			gpio_bicolour_colorled = LED_STATE_GREEN;
            break;

        case LED_STATE_ORANGE:
			/*printk("%s %d: set LED_STATE_ORANGE\r\n", __FUNCTION__, __LINE__);*/
        	ath_gpio_out_val(INET_LED_GPIO, INET_LED_OFF);
        	ath_gpio_out_val(INET_LED_COLOR_GPIO, INET_LED_COLOR_ON);
			gpio_bicolour_colorled = LED_STATE_ORANGE;
			break;

		case LED_STATE_OFF:
			/*printk("%s %d: set LED_STATE_OFF\r\n", __FUNCTION__, __LINE__);*/
        	ath_gpio_out_val(INET_LED_GPIO, INET_LED_OFF);
        	ath_gpio_out_val(INET_LED_COLOR_GPIO, INET_LED_COLOR_OFF);
			gpio_bicolour_colorled = LED_STATE_OFF;
			break;
    }
}
static int gpio_bicolour_led_read (char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", gpio_bicolour_colorled);
}
#endif

static int gpio_tricolor_led_write (struct file *file, const char *buf,
        unsigned long count, void *data)
{
    u_int32_t val, green_led_onoff = 0, yellow_led_onoff = 0;

	if (sscanf(buf, "%d", &val) != 1)
		return -EINVAL;

    if (val >= LED_STATE_MAX)
        return -EINVAL;

    if (val == gpio_tricolorled)
	return count;

    switch (val) {
        case LED_STATE_OFF :
			/* both LEDs OFF */
            green_led_onoff = JUMPSTART_LED_OFF;
            yellow_led_onoff = JUMPSTART_LED_OFF;
            break;

        case LED_STATE_GREEN:
			 /* green ON, Yellow OFF */
            green_led_onoff = JUMPSTART_LED_ON;
            yellow_led_onoff = JUMPSTART_LED_OFF;
            break;

        case LED_STATE_YELLOW:
			 /* green OFF, Yellow ON */
            green_led_onoff = JUMPSTART_LED_OFF;
            yellow_led_onoff = JUMPSTART_LED_ON;
            break;

        case LED_STATE_ORANGE:
			/* both LEDs ON */
            green_led_onoff = JUMPSTART_LED_ON;
            yellow_led_onoff = JUMPSTART_LED_ON;
            break;
    }
#ifdef CONFIG_GPIO_OPTION66
/*
 * Reset button is also used as wps button(Only for wr841nv11).
 * If a user presses the reset button, the wps led will enter flashing mode.
 * But we want to make all LEDs light all the time after the reset button is pressed for more than 5s,
 * so we should disable this flashing mode.
 * By xiejiabai@tp-link.net, 2017.07.29
 */
	if (rstPressTimeCounter == 0)
	{
		ath_gpio_out_val (TRICOLOR_LED_GREEN_PIN, green_led_onoff);
	}
#else
    ath_gpio_out_val (TRICOLOR_LED_GREEN_PIN, green_led_onoff);
#endif
    gpio_tricolorled = val;

    return count;
}

#ifdef RE_BTN_GPIO
static led_state_e gpio_re_led = LED_STATE_OFF;
static int re_led_proc_read (char *page, char **start, off_t off,
               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", gpio_re_led);
}

static int re_led_proc_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val, green_led_onoff = 0, yellow_led_onoff = 0;

    if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

    if (val >= LED_STATE_MAX)
        return -EINVAL;

    if (val == gpio_re_led)
    return count;

    switch (val) {
        case LED_STATE_OFF :
            /* both LEDs OFF */
            green_led_onoff = RE_WPS_LED_OFF;
            yellow_led_onoff = RE_WPS_LED_OFF;
            break;

        case LED_STATE_GREEN:
             /* green ON, Yellow OFF */
            green_led_onoff = RE_WPS_LED_ON;
            yellow_led_onoff = RE_WPS_LED_OFF;
            break;

        case LED_STATE_YELLOW:
             /* green OFF, Yellow ON */
            green_led_onoff = RE_WPS_LED_OFF;
            yellow_led_onoff = RE_WPS_LED_ON;
            break;

        case LED_STATE_ORANGE:
            /* both LEDs ON */
            green_led_onoff = RE_WPS_LED_OFF;
            yellow_led_onoff = RE_WPS_LED_OFF;
            break;
    }

//    ath_reg_rmw_clear(ath_get_gpio_function(TRICOLOR_LED_GREEN_PIN),
//                        0xff << ATH_GET_GPIO_SHIFT_BIT(TRICOLOR_LED_GREEN_PIN));
    ath_gpio_out_val (RE_WPS_LED_GPIO, green_led_onoff);
    gpio_re_led = val;

    return count;
}

#endif

static int create_simple_config_led_proc_entry(void)
{
	if (simple_config_entry != NULL) 
	{
		printk("Already have a proc entry for /proc/simple_config!\n");
		return -ENOENT;
	}

	simple_config_entry = proc_mkdir("simple_config", NULL);
	if (!simple_config_entry)
	{
		return -ENOENT;
	}

	simulate_push_button_entry 
		= create_proc_entry ("push_button", 0644, simple_config_entry);
	if (!simulate_push_button_entry)
	{
		return -ENOENT;
	}

	simulate_push_button_entry->write_proc = push_button_write;
	simulate_push_button_entry->read_proc = push_button_read;

#ifndef SYS_LED_GPIO/*  by huangwenzhong, 21May13 */
	blockWps_entry 
		= create_proc_entry ("block_wps", 0644, simple_config_entry);
	if (!blockWps_entry)
	{
		return -ENOENT;
	}

	blockWps_entry->write_proc = blockWps_proc_write;
	blockWps_entry->read_proc = blockWps_proc_read;
#endif

	/* added by zjg, 12Apr10 */
	multi_use_reset_button_entry = create_proc_entry ("multi_use_reset_button", 0644,
							simple_config_entry);
	if (!multi_use_reset_button_entry)
	{
		return -ENOENT;
	}

	multi_use_reset_button_entry->write_proc	= multi_use_reset_button_write;
	multi_use_reset_button_entry->read_proc 	= multi_use_reset_button_read;
	/* end added */

#ifdef WIFI_RADIO_SW_GPIO
	/* added by zjin for multi-use wifi button, 27Apr2012 */
	multi_use_wifi_button_entry = 
		create_proc_entry ("multi_use_wifi_button", 0644, simple_config_entry);
	if (!multi_use_wifi_button_entry)
	{
		return -ENOENT;
	}

	multi_use_wifi_button_entry->write_proc = multi_use_wifi_button_write;
	multi_use_wifi_button_entry->read_proc	= multi_use_wifi_button_read;
	/* end added */


	/* wifi switch entry */
	wifi_button_entry = 
			create_proc_entry("wifi_button", 0666, simple_config_entry);
	if(!wifi_button_entry)
	{
		return -ENOENT;
	}
	wifi_button_entry->write_proc = wifi_button_proc_write;
	wifi_button_entry->read_proc = wifi_button_proc_read;
#endif

	tricolor_led_entry = 
			create_proc_entry ("tricolor_led", 0644, simple_config_entry);
	if (!tricolor_led_entry)
	{
		return -ENOENT;
	}

	tricolor_led_entry->write_proc = gpio_tricolor_led_write;
	tricolor_led_entry->read_proc = gpio_tricolor_led_read;

#if (CONFIG_GPIO_INTERNET_LED_BIT >= ATH_GPIO_MIN) && (CONFIG_GPIO_INTERNET_LED_COLOR >= ATH_GPIO_MIN)
	bicolour_led_entry = create_proc_entry("bicolour_led", 0644, simple_config_entry);
	if (!bicolour_led_entry)
	{
		return -ENOENT;
	}
	bicolour_led_entry->write_proc = gpio_bicolour_led_write;
	bicolour_led_entry->read_proc = gpio_bicolour_led_read;
#endif
	
#ifdef AP_USB_LED_GPIO
	/* for usb led blink */
	usb_led_blink_entry = 
			create_proc_entry ("usb_blink", 0666, simple_config_entry);
	if (!usb_led_blink_entry)
{
		return -ENOENT;
	}

	usb_led_blink_entry->write_proc = usb_led_blink_write;
	usb_led_blink_entry->read_proc = usb_led_blink_read;
	
	/*added by ZQQ, 10.06.02 for usb power*/
	usb_power_entry = 
			create_proc_entry("usb_power", 0666, simple_config_entry);
	if(!usb_power_entry)
	{
		return -ENOENT;
	}

	usb_power_entry->write_proc = usb_power_write;
	usb_power_entry->read_proc = usb_power_read;
	/*end added*/
#endif

#ifdef WORKMODE_SWITCH_1ST_GPIO
	/* workmode switch entry */
	workmode_entry = 
			create_proc_entry("workmode", 0666, simple_config_entry);
	if(!workmode_entry)
	{
		return -ENOENT;
	}
	workmode_entry->write_proc = workmode_proc_write;
	workmode_entry->read_proc = workmode_proc_read;
#endif

#ifdef CONFIG_WPS_HOST_CLIENT_MULTIPLE
	/*  entry */
	wps_client_mode_entry = create_proc_entry("wps_client_mode", 0666, simple_config_entry);
	if(!wps_client_mode_entry)
	{
		return -ENOENT;
	}
	wps_client_mode_entry->write_proc = wps_client_mode_proc_write;
	wps_client_mode_entry->read_proc = wps_client_mode_proc_read;

	wps_client_button_entry = create_proc_entry("wps_client_button", 0666, simple_config_entry);
	if(!wps_client_button_entry)
	{
		return -ENOENT;
	}
	wps_client_button_entry->write_proc = wps_client_button_proc_write;
	wps_client_button_entry->read_proc = wps_client_button_proc_read;
#endif
	/*
	 *	General GPIO Write and Read from simple_config
	 *  echo port_num port_val > /proc/simple_config/gpio_rdwr
	 */
	gpio_write_entry = create_proc_entry("gpio_rdwr", 0666, simple_config_entry);
    gpio_write_entry->write_proc = simple_gpio_write;
    gpio_write_entry->read_proc = simple_gpio_read;

#ifdef RE_BTN_GPIO
	re_led_entry = create_proc_entry("re_led", 0666, simple_config_entry);
    if(!re_led_entry)
    {
        return -ENOENT;
    }
    re_led_entry->write_proc = re_led_proc_write;
    re_led_entry->read_proc = re_led_proc_read;
#endif
	return 0;
}


/* ioctl for reset default detection and system led switch*/
int ath_gpio_ioctl(struct inode *inode, struct file *file,  unsigned int cmd, unsigned long arg)
{
	int* argp = (int *)arg;

	if (_IOC_TYPE(cmd) != ATH_GPIO_MAGIC ||
		_IOC_NR(cmd) < ATH_GPIO_IOCTL_BASE ||
		_IOC_NR(cmd) > ATH_GPIO_IOCTL_MAX)
	{
		printk("type:%d nr:%d\n", _IOC_TYPE(cmd), _IOC_NR(cmd));
		printk("ath_gpio_ioctl:unknown command\n");
		return -1;
}

	switch (cmd)
{
	case ATH_GPIO_BTN_READ:/* 读取恢复出厂设置按钮的值*/
		*argp = counter;
		counter = 0;
		break;

	case ATH_GPIO_BTN_PUSH_CNT_READ:/*读取恢复出厂设置按钮按下次数*/
		*argp = btnPushCnt;
		btnPushCnt = 0;
        break;

#ifdef SYS_LED_GPIO
	case ATH_GPIO_LED_WRITE:/* 系统灯点灯 */
		if (unlikely(bBlockWps))
			bBlockWps = 0;

		if (SYS_LED_GPIO >= ATH_GPIO_MIN)
		{
			ath_gpio_out_val(SYS_LED_GPIO, *argp);
        }
		break;
#endif

#ifdef LAN_LED_GPIO
	case ATH_GPIO_LAN_LED_WRITE:
		if(LAN_LED_GPIO >=ATH_GPIO_MIN)
		{
			ath_gpio_out_val(LAN_LED_GPIO, *argp);
		}
		
		break;
#endif

#if (1 == CONFIG_WPS_810EU)
    case ATH_GPIO_RE_BTN_READ:
		*argp = sta_wps_flag;
		sta_wps_flag = 0;
		break;
#endif

#ifdef RE_BTN_GPIO
	case ATH_GPIO_RE_BTN_READ:
		*argp = re_button_flag;
		re_button_flag = 0;
		break;
#endif

#ifdef 	WIFI_RADIO_SW_GPIO/* for wifi switch(button) iotcl */	
	case ATH_GPIO_WIFI_SW_READ:/* 读取WIFI开关的值 */
		*argp = (ath_gpio_in_val(WIFI_RADIO_SW_GPIO)) >> WIFI_RADIO_SW_GPIO;
		
#ifdef CONFIG_GPIO_DEBUG
		print_gpio_regs("");
#endif			
		break;

	case ATH_GPIO_WIFI_BTN_READ:/* 读取WIFI按钮的值 */
		*argp = wifi_button_flag;
		wifi_button_flag = 0;
		break;
#endif

#ifdef AP_USB_LED_GPIO/* for usb led ioctl */
	case ATH_GPIO_USB_LED1_WRITE:/* USB灯点灯 */
		ath_gpio_out_val(AP_USB_LED_GPIO, *argp);
		break;
#endif

#ifdef INET_LED_GPIO/* for internet led ioctl */
	case ATH_GPIO_INET_LED_WRITE:/* 因特网灯点灯 */
         /*! if no rmw_clear and config_output, inet led do not light on 902ac chenyuqi,Jul2015 */
		ath_reg_rmw_clear(ath_get_gpio_function(INET_LED_GPIO),
					0xff << ATH_GET_GPIO_SHIFT_BIT(INET_LED_GPIO));
    	ath_gpio_config_output(INET_LED_GPIO);
		ath_gpio_out_val(INET_LED_GPIO, *argp);
		break;
#endif

#ifdef INET_LED_COLOR_GPIO
    case ATH_GPIO_WAN_LED_WRITE:
        ath_gpio_out_val(INET_LED_COLOR_GPIO, *argp);
        break;
#endif

#ifdef WIFI_LED_GPIO
    case ATH_GPIO_WIFI_LED_WRITE:
		ath_reg_rmw_clear(ath_get_gpio_function(WIFI_LED_GPIO),
					0xff << ATH_GET_GPIO_SHIFT_BIT(WIFI_LED_GPIO));
    	ath_gpio_config_output(WIFI_LED_GPIO);
        ath_gpio_out_val(WIFI_LED_GPIO, *argp);
        break;
#endif

#ifdef POWER_LED_GPIO_BIT
    case ATH_GPIO_POWER_LED_WRITE:
        ath_gpio_out_val(POWER_LED_GPIO_BIT, *argp);
        break;
#endif

#ifdef GPIP_REG_CTL_LED
	case ATH_GPIO_REG_CTL_WRITE:
		shift_register_set(*argp);
		break;
		
	case ATH_GPIO_USB_POWER_WRITE:
		ath_gpio_out_val(USB_EN_POWER_CTL_GPIO, *argp);
		break;
#endif

#if (1==CONFIG_GPIO_801)
	case ATH_GPIO_WPS_LED_WRITE:
#if (1 == CONFIG_GPIO_801_V4)
        ath_reg_rmw_clear(ath_get_gpio_function(15),
                    0xff << ATH_GET_GPIO_SHIFT_BIT(15));
        ath_gpio_config_output(15);
        ath_gpio_out_val(15, *argp);

        ath_reg_rmw_clear(ath_get_gpio_function(14),
                    0xff << ATH_GET_GPIO_SHIFT_BIT(14));
        ath_gpio_config_output(14);
        ath_gpio_out_val(14, *argp);
#else
		ath_reg_rmw_clear(ath_get_gpio_function(15),
					0xff << ATH_GET_GPIO_SHIFT_BIT(15));
    	ath_gpio_config_output(15);
        ath_gpio_out_val(15, *argp);
		
		ath_reg_rmw_clear(ath_get_gpio_function(11),
					0xff << ATH_GET_GPIO_SHIFT_BIT(11));
    	ath_gpio_config_output(11);
        ath_gpio_out_val(11, *argp);
#endif
		break;
#elif (1==CONFIG_GPIO_843)
		case ATH_GPIO_WPS_LED_WRITE:
			ath_reg_rmw_clear(ath_get_gpio_function(15),
						0xff << ATH_GET_GPIO_SHIFT_BIT(15));
			ath_gpio_config_output(15);
			ath_gpio_out_val(15, *argp);
			break;
#endif


	default:
//		printk("command not supported\n");
		return -1;
	}


	return 0;
}


int ath_gpio_open (struct inode *inode, struct file *filp)
{
	int minor = iminor(inode);
	int devnum = minor; //>> 1;
	struct mtd_info *mtd;

	if ((filp->f_mode & 2) && (minor & 1))
	{
		printk("You can't open the RO devices RW!\n");
		return -EACCES;
	}

	mtd = get_mtd_device(NULL, devnum);   
	if (!mtd)
	{
		printk("Can not open mtd!\n");
		return -ENODEV;	
	}
	filp->private_data = mtd;
	return 0;

}

/* struct for cdev */
struct file_operations gpio_device_op =
{
	.owner = THIS_MODULE,
	.ioctl = ath_gpio_ioctl,
	.open = ath_gpio_open,
};

/* struct for ioctl */
static struct cdev gpio_device_cdev =
{
	.owner  = THIS_MODULE,
	.ops	= &gpio_device_op,
};
/******* end  ioctl stuff **********/



int __init ath_simple_config_init(void)
{
	/* restore factory default and system led */
	dev_t dev;
	int rt;
	int ath_gpio_major = gpio_major;
	int ath_gpio_minor = gpio_minor;

	init_timer(&rst_timer);
	rst_timer.function = check_rst;

#ifdef WIFI_RADIO_SW_GPIO
	init_timer(&wifi_button_timer);
	wifi_button_timer.function = wifi_sw_check;
#endif

#ifdef RE_BTN_GPIO
    init_timer(&re_button_timer);
    re_button_timer.function = re_check;
#endif

	/* config gpio3, 12, 13, 17 as normal gpio function */

	/* because we use GPIO4 as WAN LED, so we need clean bit6 */
	/* we do it in ethernet driver instead */
	//ath_reg_rmw_clear(ATH_GPIO_FUNCTIONS, 1 << 6);

	/* disable JTAG before use GPIO3 */
	ath_reg_rmw_set(ATH_GPIO_FUNCTIONS, 1 << 1);

#ifdef AP_USB_LED_GPIO
#if 0
	/* gpio X usb 使能开关*/
	ath_reg_rmw_clear(ath_get_gpio_function(USB_POWER_SW_GPIO),
						0xff << ATH_GET_GPIO_SHIFT_BIT(USB_POWER_SW_GPIO));
	/* gpio Y usb 灯*/
	ath_reg_rmw_clear(ath_get_gpio_function(AP_USB_LED_GPIO),
						0xff << ATH_GET_GPIO_SHIFT_BIT(AP_USB_LED_GPIO));
#endif
	/* Configure GPIOs default status */
    /*! if no ath_reg_rmw_clear, usb led do not light on 902ac chenyuqi,Jul2015 */
	ath_reg_rmw_clear(ath_get_gpio_function(AP_USB_LED_GPIO),
			0xff << ATH_GET_GPIO_SHIFT_BIT(AP_USB_LED_GPIO));
	ath_gpio_config_output(AP_USB_LED_GPIO);
	ath_gpio_out_val(AP_USB_LED_GPIO, USB_LED_OFF);
	
	ath_gpio_config_output(USB_POWER_SW_GPIO);
	ath_gpio_out_val(USB_POWER_SW_GPIO, 1);

#endif

#ifdef SYS_LED_GPIO	
	/* gpio13 系统灯*/
	ath_reg_rmw_clear(ath_get_gpio_function(SYS_LED_GPIO),
						0xff << ATH_GET_GPIO_SHIFT_BIT(SYS_LED_GPIO));
#endif


/* gpio3 QSS灯*/
	ath_reg_rmw_clear(ath_get_gpio_function(TRICOLOR_LED_GREEN_PIN),
						0xff << ATH_GET_GPIO_SHIFT_BIT(TRICOLOR_LED_GREEN_PIN));

#ifdef WIFI_RADIO_SW_GPIO
	/* gpio17 WIFI按钮/开关*/
	ath_reg_rmw_clear(ath_get_gpio_function(WIFI_RADIO_SW_GPIO),
						0xff << ATH_GET_GPIO_SHIFT_BIT(WIFI_RADIO_SW_GPIO));
#endif

	/* gpio12 恢复出厂设置键*/
	ath_reg_rmw_clear(ath_get_gpio_function(RST_DFT_GPIO),
						0xff << ATH_GET_GPIO_SHIFT_BIT(RST_DFT_GPIO));

#ifdef INET_LED_GPIO
	/*	by huangwenzhong, 06Mar13 */
	/* gpio X 因特网灯，目前只有WR803N才有*/
	ath_reg_rmw_clear(ath_get_gpio_function(INET_LED_GPIO),
					0xff << ATH_GET_GPIO_SHIFT_BIT(INET_LED_GPIO));

#endif
#ifdef INET_LED_COLOR_GPIO
    ath_reg_rmw_clear(ath_get_gpio_function(INET_LED_COLOR_GPIO),
					0xff << ATH_GET_GPIO_SHIFT_BIT(INET_LED_COLOR_GPIO));
#endif

#ifdef RE_BTN_GPIO
    /* RE键*/
    ath_reg_rmw_clear(ath_get_gpio_function(RE_BTN_GPIO),
                        0xff << ATH_GET_GPIO_SHIFT_BIT(RE_BTN_GPIO));
#endif

	create_simple_config_led_proc_entry();

	ath_gpio_config_input(RST_DFT_GPIO);
	udelay(1000);
	
    /* configure GPIO RST_DFT_GPIO as level triggered interrupt */
    ath_gpio_config_int (RST_DFT_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
	rt = request_irq (ATH_GPIO_IRQn(RST_DFT_GPIO), rst_irq, 0,
					   "RESTORE_FACTORY_DEFAULT", NULL);
	if (rt != 0)
	{
		printk (KERN_ERR "unable to request IRQ for RESTORE_FACTORY_DEFAULT GPIO (error %d)\n", rt);
	}

#ifdef WIFI_RADIO_SW_GPIO
	/* wifi switch! */
	ath_gpio_config_input(WIFI_RADIO_SW_GPIO);

	/* configure GPIO WIFI_RADIO_SW_GPIO as level triggered interrupt */
	ath_gpio_config_int (WIFI_RADIO_SW_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);

	rt = request_irq (ATH_GPIO_IRQn(WIFI_RADIO_SW_GPIO), 
					wifi_sw_irq, 0, "WIFI_RADIO_SWITCH", NULL);
	if (rt != 0)
	{
		printk (KERN_ERR "unable to request IRQ for WIFI_RADIO_SWITCH GPIO (error %d)\n", rt);
    }
#endif

#ifdef RE_BTN_GPIO
    /*re button! */
    ath_gpio_config_input(RE_BTN_GPIO);

    /* configure GPIO WIFI_RADIO_SW_GPIO as level triggered interrupt */
    ath_gpio_config_int (RE_BTN_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);

    rt = request_irq (ATH_GPIO_IRQn(RE_BTN_GPIO), 
                    re_btn_irq, 0, "RE_BUTTON", NULL);
    if (rt != 0)
    {
        printk (KERN_ERR "unable to request IRQ for RE_BUTTON (error %d)\n", rt);
    }

    ath_gpio_config_output(RE_WPS_LED_GPIO);
    ath_gpio_out_val(RE_WPS_LED_GPIO, RE_WPS_LED_OFF);
#endif

	/* Work mode switchs!
	 * 14	16
		L	H	3G
		H	L	Router/APC Router
		H	H	AP
	 */
#ifdef WORKMODE_SWITCH_1ST_GPIO
	ath_gpio_config_input(WORKMODE_SWITCH_1ST_GPIO);
#endif

/* independent button for wps  */
#ifdef WPS_BUTTON_GPIO
	ath_gpio_config_input(WPS_BUTTON_GPIO);
    ath_gpio_config_int (WPS_BUTTON_GPIO, INT_TYPE_EDGE, INT_POL_ACTIVE_LOW);
	rt = request_irq (ATH_GPIO_IRQn(WPS_BUTTON_GPIO), wps_irq, 0,
					   "WPS_START", NULL);
	if (rt != 0)
	{
		printk (KERN_ERR "unable to request IRQ for WPS_BUTTON_GPIO (error %d)\n", rt);
	}
#endif

#ifdef WORKMODE_SWITCH_2ND_GPIO
	ath_gpio_config_input(WORKMODE_SWITCH_2ND_GPIO);
#endif

#ifdef AP_USB_LED_GPIO
	/* Configure GPIOs default status */
	ath_gpio_config_output(AP_USB_LED_GPIO);
	ath_gpio_out_val(AP_USB_LED_GPIO, USB_LED_OFF);
	
	ath_gpio_config_output(USB_POWER_SW_GPIO);
	ath_gpio_out_val(USB_POWER_SW_GPIO, 1);
#endif

#ifdef SYS_LED_GPIO	
	if (SYS_LED_GPIO >= ATH_GPIO_MIN)
	{
		ath_gpio_config_output(SYS_LED_GPIO);
		ath_gpio_out_val(SYS_LED_GPIO, SYS_LED_OFF);
	}
#endif

#ifdef INET_LED_GPIO
	ath_gpio_config_output(INET_LED_GPIO);
	ath_gpio_out_val(INET_LED_GPIO, INET_LED_OFF);
#endif
#ifdef INET_LED_COLOR_GPIO
	ath_gpio_config_output(INET_LED_COLOR_GPIO);
	ath_gpio_out_val(INET_LED_COLOR_GPIO, INET_LED_COLOR_OFF);
#endif


	/* configure wps gpio as outputs */
	ath_gpio_config_output (TRICOLOR_LED_GREEN_PIN); 
	ath_gpio_out_val (TRICOLOR_LED_GREEN_PIN, JUMPSTART_LED_OFF);

#ifdef WIFI_LED_GPIO
	ath_reg_rmw_clear(ath_get_gpio_function(WIFI_LED_GPIO),
					0xff << ATH_GET_GPIO_SHIFT_BIT(WIFI_LED_GPIO));
    ath_gpio_config_output(WIFI_LED_GPIO);
    ath_gpio_out_val(WIFI_LED_GPIO, WIFI_LED_OFF);
#endif

#if (1==CONFIG_GPIO_801)
#if (1 == CONFIG_GPIO_801_V4)
    ath_gpio_config_output(4);
    ath_gpio_out_val(4, 0);

    ath_gpio_config_output(14);
    ath_gpio_out_val(14, 0);
	
    ath_gpio_config_output(15);
    ath_gpio_out_val(15, 0);
#else
    ath_gpio_config_output(11);
    ath_gpio_out_val(11, 0);
	
    ath_gpio_config_output(15);
    ath_gpio_out_val(15, 0);
#endif
#endif

#ifdef POWER_LED_GPIO_BIT
    ath_gpio_config_output(POWER_LED_GPIO_BIT);
    ath_gpio_out_val(POWER_LED_GPIO_BIT, POWER_LED_GPIO_ON);
#endif

#ifdef GPIP_REG_CTL_LED
	ath_gpio_out_val(AP143_2_GPIO_SHIFT_OE, 0);
	shift_register_set(0xff);

	/* Power off the usb port.*/
	ath_gpio_out_val(USB_EN_POWER_CTL_GPIO, 0x0);
#endif

	/* Create char device for gpio */
	if (ath_gpio_major)
	{
		dev = MKDEV(ath_gpio_major, ath_gpio_minor);
		rt = register_chrdev_region(dev, 1, "ar7240_gpio_chrdev");
	}
	else
	{
		rt = alloc_chrdev_region(&dev, ath_gpio_minor, 1, "ar7240_gpio_chrdev");
		ath_gpio_major = MAJOR(dev);
	}

	if (rt < 0)
	{
		printk(KERN_WARNING "ar7240_gpio_chrdev : can`t get major %d\n", ath_gpio_major);
		return rt;
	}

	cdev_init (&gpio_device_cdev, &gpio_device_op);
	rt = cdev_add(&gpio_device_cdev, dev, 1);

	if (rt < 0) 
	{
		printk(KERN_NOTICE "Error %d adding ar7240_gpio_chrdev ", rt);
	}
	//print_gpio_regs(-1);
	return 0;
}

//subsys_initcall(ath_simple_config_init);

/*
 * used late_initcall so that misc_register will succeed
 * otherwise, misc driver won't be in a initializated state
 * thereby resulting in misc_register api to fail.
 */
#if !defined(CONFIG_ATH_EMULATION)
late_initcall(ath_simple_config_init);
#endif

