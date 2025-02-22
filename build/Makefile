export DEV_NAME=wr841nv11_eu
all:

ifndef DEV_NAME
$(error "You have to define Dev name to use this Makefile")
endif

export TOPDIR=$(PWD)/..
export INSTALL_ROOT=$(TOPDIR)/rootfs.build.$(KERNELVER)
export INSTALL_ROOT_FOR_BOARD=$(TOPDIR)/rootfs.$(DEV_NAME)
export IMAGE_ROOT=$(TOPDIR)/rootfs-$(BOARD_TYPE).optbuild
temp_BOARD_TYPE = $(strip $(subst fus, , $(BOARD_TYPE)))
export GPL_DIR=$(TOPDIR)/$(GPL_PACKAGE_NAME)
export GPL_INSTALL_ROOT=$(GPL_DIR)/rootfs.build.$(KERNELVER)
export TMP_INSTALL_ROOT=$(TOPDIR)/rootfs.$(DEV_NAME).tmp
export GPL_TMP_INSTALL_ROOT=$(GPL_DIR)/rootfs.$(DEV_NAME).tmp
export GPL_INSTALL_ROOT_FOR_BOARD=$(GPL_DIR)/rootfs.$(DEV_NAME)
export GPL_PRODUCT_CONFIG_DIR=$(GPL_DIR)/build/products_config/$(DEV_NAME)/

#config file dirs
export PRODUCT_CONFIG_DIR=$(TOPDIR)/build/products_config/$(DEV_NAME)/
export KERNEL_CONFIG_FILE=$(PRODUCT_CONFIG_DIR)/kernel.config
export BUSYBOX_CONFIG_FILE=$(PRODUCT_CONFIG_DIR)/busybox.config
export MODEL_CONFIG_FILE=$(PRODUCT_CONFIG_DIR)/model.conf
export EMULATOR_DIR=$(TOPDIR)/EMULATOR_$(DEV_NAME)

ifeq ($(CONFIG_GUESTNETWORK),y)
BOARD_KERNEL_MODULES += br_guest_filter br_guest_mark
endif



# modules used by all boards
export BOARD_KERNEL_MODULES = stats netfilter wlan_warn ethernet br_filter ts_kmp pppol2tp pptp_module tp_domain ap_vlan art
# applications used by all board types 
export BOARD_APPLICATIONS = busybox pppoe l2tp bpa iptables tc_build net_ioctl arp


# include all necessary config files 
# include basic config file, it will give basic info
# such as toolchain,kernel version,flash size and so on 
#include scripts/$(DEV_NAME)/basic.config
include $(PRODUCT_CONFIG_DIR)/basic.config

ifneq ($(FEATURE_FS_SIZE_CUT), y)
BOARD_KERNEL_MODULES += netsched
BOARD_APPLICATIONS += lltd
endif

ifndef BOARD_TYPE
$(error "You have to define Board type to use this Makefile")
endif

ifndef BUILD_TYPE
export BUILD_TYPE=squashfs
endif

ifndef TOOLCHAIN
$(error "Must specify TOOLCHAIN value")
endif

ifndef TOOLARCH
$(error "Must specify TOOLARCH value")
endif

ifndef KERNEL
$(error "Must specify KERNEL value")
endif

ifndef KERNELVER
$(error "Must specify KERNELVER value")
endif

ifndef KERNELTARGET
$(error "Must specify KERNELTARGET value")
endif

ifndef KERNELARCH
$(error "Must specify KERNELARCH value")
endif

ifndef TFTPPATH
export TFTPPATH=$(TOPDIR)/tftpboot/$(DEV_NAME)/
ifeq ($(GPL_BUILD), y)
export GPL_TFTPPATH=$(GPL_DIR)/tftpboot/$(DEV_NAME)/
endif
endif

# To enable library optimization (removing unused object code from
# shared libraries) set BUILD_LIBRARYOPT to 'y' else 'n'
BUILD_LIBRARYOPT := n

ifndef  BUILD_OPTIMIZED
export BUILD_OPTIMIZED=n
export STRIP=$(TOOLPREFIX)strip
endif

ifeq ($(BUILD_OPTIMIZED), y)
export OLDSTRIP=$(TOOLPREFIX)strip
export STRIP=$(TOOLPREFIX)sstrip
endif

ifndef COMPRESSED_UBOOT
export COMPRESSED_UBOOT=0
endif

ifndef FLASH_CUT_KERNEL_BLK
export FLASH_CUT_KERNEL_BLK=0
endif

ifndef ATH_CONFIG_NVRAM
export ATH_CONFIG_NVRAM=0
endif

ifndef ATH_SINGLE_CFG
export ATH_SINGLE_CFG=0
endif

ifndef INSTALL_BLACKLIST
export INSTALL_BLACKLIST="None"
endif

ifeq ($(BOOT_FROM_NAND),1)
export NAND=-nand
else
export NAND=
endif

ifeq ($(SPECIAL_ISP_DNS), y)
export SPECIAL_ISP_DNS=y
else
export SPECIAL_ISP_DNS=n
endif

ifeq ($(MODULE_IPV6_6IN4), y)
export MODULE_IPV6_6IN4=y
endif

#export IMAGEPATH=$(TOPDIR)/images/$(BOARD_TYPE)$(BUILD_CONFIG)$(BUILD_EXT)$(NAND)
export IMAGEPATH=$(TOPDIR)/images/$(DEV_NAME)
export GPL_IMAGEPATH=$(GPL_DIR)/images/$(DEV_NAME)

ifeq ($(strip $(BUILD_SQUASHFS)),1)
KERNEL_BUILD_CONFIG=_squashfs$(BUILD_CONFIG)
else
KERNEL_BUILD_CONFIG=$(BUILD_CONFIG)
endif
export FS_NAME=$(DEV_NAME)-squashfs

ifeq ($(strip $(BOARD_TYPE)),ap124)
export MAKE_SQUASHFS_TOOL=$(TOPDIR)/util/mksquashfs4-ap124
else ifneq ($(findstring $(BOARD_TYPE), ap143 ap143-2.0 db12x), )
export MAKE_SQUASHFS_TOOL=$(TOPDIR)/util/mksquashfs4-ap143
else
export MAKE_SQUASHFS_TOOL=$(TOPDIR)/util/mksquashfs4
endif

ifneq ($(findstring $(BOARD_TYPE), ap151 ap143-2.0), )
#external header include
export ACFGAPPDIR=$(TOPDIR)/$(BOARD_TYPE)/apps/acfg/common-include
export ACFGDIR=$(TOPDIR)/$(BOARD_TYPE)/apps/acfg
endif

ifeq ($(strip $(BOARD_TYPE)),ap151)
export CFG_BOARD_TYPE := $(BOARD_TYPE)
endif

ifndef MAKE_FLASH
ifeq ($(FLASH_SIZE),16)
export MAKE_FLASH=make_flash-16M
else
ifeq ($(FLASH_CUT_KERNEL_BLK),1)
export MAKE_FLASH=make_flash_ck
else
export MAKE_FLASH=make_flash
endif
endif
ifeq ($(FEATURE_USE_MAKEFLASHV3),y)
export MAKE_FLASH=make_flash_v3
endif
ifeq ($(FEATURE_UN_VERSION_CHECK),y)
export MAKE_FLASH=make_flash_v4
ifeq ($(FEATURE_UN_VERSION_CHECK_FOR_940V5),y)
export MAKE_FLASH=make_flash_940v5
endif
ifeq ($(FEATURE_FS_EXTEND_SIZE),y)
export MAKE_FLASH=make_flash_FS_extend_size
$(warning "the MAKE_FLASH:$(MAKE_FLASH)")
endif
endif
else
endif

export MAKE_FLASH_TOOL=$(TOPDIR)/util/$(MAKE_FLASH)

export INFOFILE=vmlinux.info
export KTFILE=$(KERNELTARGET)

ifneq ($(COMPRESSED_UBOOT),1)
export UBOOTFILE=u-boot.bin
export UBOOT_BINARY=u-boot.bin
else
export UBOOTFILE=tuboot.bin
export UBOOT_BINARY=tuboot.bin
endif
export UBOOT_FILE=u-boot.bin

#
# Other environmental variables that are configured as per the configuration file
# specified above.  These contain all platform specific configuration items.
#

ifeq ($(strip $(ATH_CARR_DIR)),)
export KERNELPATH=$(TOPDIR)/$(BOARD_TYPE)/linux/kernels/$(KERNEL)
export MAKEARCH=$(MAKE) ARCH=$(KERNELARCH) CROSS_COMPILE=$(TOOLPREFIX)

export TOOLPATH=$(TOPDIR)/build/$(TOOLCHAIN)/$(TOOLARCH)/
export BOOTLOADERDIR=$(TOPDIR)/$(BOARD_TYPE)/boot/u-boot

export UBOOTDIR=$(TOPDIR)/$(BOARD_TYPE)/boot/u-boot
endif

# Save PATH for later use for compiling host-only tools etc.
#~export ORIGINAL_PATH:=$(PATH)
# Set PATH so we find target compiler when say "gcc", etc.
# as well as other tools we expect to find.
export PATH:=$(TOOLPATH)/bin:${PATH}

# madwifi
export HAL=$(TOPDIR)/$(BOARD_TYPE)/wlan/hal
export ATH_PHYERR=$(TOPDIR)/$(BOARD_TYPE)/wlan/madwifi/dfs
export ATH_RATE=$(TOPDIR)/$(BOARD_TYPE)/wlan/madwifi/ratectrl11n/
# this path is use for hostapd and wpa_supplicant, so must define this val
export ATHEROSPATH=$(TOPDIR)/$(BOARD_TYPE)/wlan
export MODULEPATH=$(INSTALL_ROOT)/lib/modules/$(KERNELVER)/net

KERNEL_MODULE_PATH=$(TOPDIR)/images/$(DEV_NAME)/kernel_modules/
GPL_KERNEL_MODULE_PATH=$(GPL_DIR)/images/$(DEV_NAME)/kernel_modules/
KERNEL_MODULES_IMAGE=$(IMAGEPATH)/kernel_modules


uboot_clean:
	@if test -f $(UBOOTDIR)/Makefile;then \
	echo Making uboot clean; \
	cd $(UBOOTDIR) && $(MAKEARCH) distclean; \
	else echo "Do nothing for clean uboot"; \
	fi


#if you have uboot code,then you will execute uboot_build_copy
#this rule will compile your uboot code and copy uboot to your image direcory
uboot_build_copy:
	@echo Making uboot at `date`
	cd $(UBOOTDIR) && $(MAKEARCH) mrproper
	cd $(UBOOTDIR) && $(MAKEARCH) $(BOARD_TYPE)_config
	@echo ========= build dir: $(TOPDIR)/build ============
	cd $(UBOOTDIR) && $(MAKEARCH) all BUILD_DIR=$(TOPDIR)/build

	mkdir -p $(TOPDIR)/binary
	cp -f $(UBOOTDIR)/${UBOOT_BINARY} $(TOPDIR)/binary/$(UBOOT_FILE)
	@echo End Making uboot at `date`

#new uboot rule for somebody may have no uboot code
uboot:
	@if test -f $(UBOOTDIR)/Makefile;then \
	$(MAKE) uboot_build_copy;\
	else @echo This is bad; \
	fi

all: uboot