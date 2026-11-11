############################################################################################
# ZKC21V / ZKC42V custom EPD firmware
#
# Build examples:
#   make BOARD=ZKC42V -j4
#   make BOARD=ZKC21V -j4
#
# The generated binary is linked at 0x404000, the same first-stage slot used
# by the stock/InkTag recovery. Keep a full 1 MiB J-Link backup before flashing.
############################################################################################

BOARD ?= ZKC42V

# SLOT: empty = legacy single-image at 0x404000;
#       A = 0x414000 (192K), B = 0x444000 (192K) for the FreeInk A/B layout
SLOT ?=
ifeq ($(SLOT),A)
SLOT_ORIGIN = 0x00414000
SLOT_LENGTH = 192K
else ifeq ($(SLOT),B)
SLOT_ORIGIN = 0x00444000
SLOT_LENGTH = 192K
endif

PROJECT = freeink_$(BOARD)$(if $(SLOT),_slot$(SLOT))

# Keep the linked ELF/bin/hex/map in a dedicated directory.
BUILD_DIR = ../build

# Path to the OnMicro OM6626 SDK. Override if it lives elsewhere:
#   make BOARD=ZKC21V SDK_ROOT=/path/to/OM6626-sdk
SDK_ROOT ?= ../../sdk

# The SDK build rules only compile sources expressed relative to the SDK
# root.  Link this repo's firmware/ into the SDK's projects/ directory so
# $(ROOT_DIR)$(PRJ_REL)/src/*.c resolves, and reference sources through it.
PRJ_LINK  = $(SDK_ROOT)/projects/freeink
PRJ_REL   = projects/freeink/src
$(shell mkdir -p $(SDK_ROOT)/projects && ln -sfn $(abspath ../) $(PRJ_LINK))
$(shell mkdir -p $(BUILD_DIR))
TARGET = $(BUILD_DIR)/freeink_$(BOARD)$(if $(SLOT),_slot$(SLOT))

ROOT_DIR = $(SDK_ROOT)/
CUR_ROMLIB_DIR = $(ROOT_DIR)hal/device/om6626/rom_lib/current/

ifeq ($(BOARD),ZKC21V)
BOARD_DEFINE = BOARD_ZKC21V
EPD_SRC = $(ROOT_DIR)$(PRJ_REL)/epd_ssd1680.c
else
BOARD_DEFINE = BOARD_ZKC42V
EPD_SRC = $(ROOT_DIR)$(PRJ_REL)/epd_jd79668.c
endif

DEFINES = CONFIG_OM6626A=1 CONFIG_LIB_PRESET_BLE_1PERIPHERAL CONFIG_AUTOCONF_PRESET _start=main $(BOARD_DEFINE)=1

ifeq ($(PM),1)
DEFINES += PM_SLEEP_ENABLE
endif
ifeq ($(EPDDBG),1)
DEFINES += EPD_DEBUG
endif

CONFIG_OPTIMIZATION_LEVEL = 4
CONFIG_LIB_GENERATE = n
POSTBUILDPROG =

SOURCES = \
    $(ROOT_DIR)common/om_libc_retarget.c \
    $(ROOT_DIR)common/om_printf.c \
    $(ROOT_DIR)common/om_fifo.c \
    $(ROOT_DIR)common/om_list.c \
    $(ROOT_DIR)common/om_utils.c \
    $(ROOT_DIR)common/om_log.c \
    $(ROOT_DIR)components/shell/shell.c \
    $(ROOT_DIR)components/shell/shell_cmd.c \
    $(ROOT_DIR)components/shell/shell_port.c \
    $(ROOT_DIR)components/ble/common/ob_mem_heap.c \
    $(ROOT_DIR)components/evt/evt.c \
    $(ROOT_DIR)components/evt/evt_timer.c \
    $(ROOT_DIR)components/crypto/sha256.c \
    $(ROOT_DIR)components/crypto/micro_ecc/uECC.c \
    $(ROOT_DIR)components/pm/pm.c \
    $(ROOT_DIR)components/mbr/mbr.c \
    $(ROOT_DIR)components/trace/trc_io.c \
    $(ROOT_DIR)hal/driver/common/drv_isr.c \
    $(ROOT_DIR)hal/driver/common/drv_usart.c \
    $(ROOT_DIR)hal/driver/common/drv_usart_ex.c \
    $(ROOT_DIR)hal/driver/common/drv_wdt.c \
    $(ROOT_DIR)hal/driver/common/drv_pmu_timer.c \
    $(ROOT_DIR)hal/driver/common/drv_cortex.c \
    $(ROOT_DIR)hal/driver/common/drv_24g.c \
    $(ROOT_DIR)hal/driver/common/drv_aes_hw.c \
    $(ROOT_DIR)hal/driver/common/drv_gpio.c \
    $(ROOT_DIR)hal/driver/common/drv_efuse.c \
    $(ROOT_DIR)hal/driver/common/drv_sf_base.c \
    $(ROOT_DIR)hal/driver/common/drv_sf_sys.c \
    $(ROOT_DIR)hal/driver/common/drv_sf.c \
    $(ROOT_DIR)hal/driver/common/drv_dma.c \
    $(ROOT_DIR)hal/driver/common/drv_spi.c \
    $(ROOT_DIR)hal/driver/common/drv_i2c.c \
    $(ROOT_DIR)hal/driver/common/drv_rtc.c \
    $(ROOT_DIR)hal/driver/common/drv_radio.c \
    $(ROOT_DIR)hal/driver/common/drv_tim.c \
    $(ROOT_DIR)hal/driver/common/drv_lp_tim.c \
    $(ROOT_DIR)hal/driver/common/drv_rng.c \
    $(ROOT_DIR)hal/driver/om6626/drv_pinmux.c \
    $(ROOT_DIR)hal/driver/om6626/drv_pmu.c \
    $(ROOT_DIR)hal/driver/om6626/drv_rcc.c \
    $(ROOT_DIR)hal/driver/om6626/drv_calib.c \
    $(ROOT_DIR)hal/driver/om6626/drv_calib_repair.c \
    $(ROOT_DIR)hal/driver/om6626/drv_adc.c \
    $(ROOT_DIR)hal/device/om6626/GCC/system_asm.S \
    $(ROOT_DIR)hal/device/om6626/GCC/startup.S \
    $(ROOT_DIR)hal/device/om6626/system.c \
    $(ROOT_DIR)$(PRJ_REL)/main.c \
    $(ROOT_DIR)$(PRJ_REL)/board.c \
    $(ROOT_DIR)$(PRJ_REL)/led.c \
    $(ROOT_DIR)$(PRJ_REL)/ble_epd.c \
    $(EPD_SRC)

INCLUDES = \
    $(ROOT_DIR)bsp/include \
    $(ROOT_DIR)common/ \
    $(ROOT_DIR)components/shell/ \
    $(ROOT_DIR)components/eeprom/ \
    $(ROOT_DIR)components/ble/include/ \
    $(ROOT_DIR)components/ble/controller/include/ \
    $(ROOT_DIR)components/ble/host/include/ \
    $(ROOT_DIR)components/ble/mesh/include/ \
    $(ROOT_DIR)components/nvds/ \
    $(ROOT_DIR)components/evt/ \
    $(ROOT_DIR)components/crypto/ \
    $(ROOT_DIR)components/crypto/micro_ecc \
    $(ROOT_DIR)components/pm \
    $(ROOT_DIR)components/mbr \
    $(ROOT_DIR)components/trace \
    $(ROOT_DIR)hal/CMSIS/Core/Include \
    $(ROOT_DIR)hal/driver/include \
    $(ROOT_DIR)hal/device/include \
    $(ROOT_DIR)include \
    $(ROOT_DIR)$(PRJ_REL)

LDSCRIPT_BASE = $(ROOT_DIR)hal/device/om6626/rom_lib/current/GCC/linker_flash.ld
ifneq ($(SLOT),)
LDSCRIPT = $(BUILD_DIR)/linker_slot$(SLOT).ld
$(shell sed -e 's/ORIGIN = 0x00404000, LENGTH = 1012K/ORIGIN = $(SLOT_ORIGIN), LENGTH = $(SLOT_LENGTH)/' $(LDSCRIPT_BASE) > $(LDSCRIPT))
LDDIR  = $(CUR_ROMLIB_DIR)GCC
else
LDSCRIPT = $(LDSCRIPT_BASE)
endif
LDEXTFILE = $(ROOT_DIR)hal/device/om6626/rom_lib/current/GCC/linker_ble_1peripheral.mk

include $(ROOT_DIR)tools/build/rules_prj.mk
