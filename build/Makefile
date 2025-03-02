export DEV_NAME=wr841nv11_eu

ifndef DEV_NAME
$(error "You have to define Dev name to use this Makefile")
endif

export TOPDIR=$(PWD)/..

#config file dirs
export PRODUCT_CONFIG_DIR=$(TOPDIR)/build/products_config/$(DEV_NAME)/


# include all necessary config files 
# include basic config file, it will give basic info
# such as toolchain,kernel version,flash size and so on 
#include scripts/$(DEV_NAME)/basic.config
include basic.config


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

ifneq ($(COMPRESSED_UBOOT),1)
export UBOOTFILE=u-boot.bin
export UBOOT_BINARY=u-boot.bin
else
export UBOOTFILE=tuboot.bin
export UBOOT_BINARY=tuboot.bin
endif
export UBOOT_FILE=u-boot.bin

# Set a toolpath. A toolchain based on GCC 4.3.3 can be found here: https://archive.openwrt.org/backfire/10.03.1/ar71xx/
export TOOLPATH=$(TOPDIR)/OpenWrt-Toolchain-ar71xx-for-mips_r2-gcc-4.3.3+cs_uClibc-0.9.30.1/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr
export TOOLPREFIX=mips-openwrt-linux-uclibc-
ifndef TOOLPATH
$(error TOOLPATH not set. Hence I can't locate toolchain)
endif

# Changing PATH
export PATH:=$(TOOLPATH)/bin:${PATH}

export MAKEARCH=$(MAKE) ARCH=$(KERNELARCH) CROSS_COMPILE=$(TOOLPREFIX)
export UBOOTDIR=$(TOPDIR)/$(BOARD_TYPE)/boot/u-boot

all: uboot

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

