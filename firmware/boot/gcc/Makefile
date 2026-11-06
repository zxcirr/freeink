############################################################################################
# FreeInk bootloader v0 — runs at 0x404000, 64 KB budget.
#   export PATH=/path/to/xpack-arm-none-eabi-.../bin:$PATH
#   make -j4
# Relies on the SDK symlink created by the app Makefile
# (OM6626-sdk/projects/freeink -> <repo>/firmware).  Build the app once first,
# or create it yourself:  ln -sfn <repo>/firmware <sdk>/projects/freeink
############################################################################################
PROJECT = freeink_boot

BUILD_DIR = ../build
$(shell mkdir -p $(BUILD_DIR))
SDK_ROOT ?= ../../../../OM6626-sdk
ROOT_DIR = $(SDK_ROOT)/
CUR_ROMLIB_DIR = $(ROOT_DIR)hal/device/om6626/rom_lib/current/
TARGET = $(BUILD_DIR)/freeink_boot

# boot occupies 0x404000..0x413FFF only
LINKER_SCRIPT_GEN = $(BUILD_DIR)/linker_boot.ld
$(shell sed -e 's/ORIGIN = 0x00404000, LENGTH = 1012K/ORIGIN = 0x00404000, LENGTH = 64K/' $(ROOT_DIR)hal/device/om6626/rom_lib/current/GCC/linker_flash.ld > $(LINKER_SCRIPT_GEN))

DEFINES = CONFIG_OM6626A=1 CONFIG_LIB_PRESET_BLE_1PERIPHERAL CONFIG_AUTOCONF_PRESET _start=main

CONFIG_OPTIMIZATION_LEVEL = 4
CONFIG_LIB_GENERATE = n
POSTBUILDPROG =

# same non-BLE source set as the app, plus boot.c via the projects/freeink link
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
    $(ROOT_DIR)components/evt/evt.c \
    $(ROOT_DIR)components/evt/evt_timer.c \
    $(ROOT_DIR)components/crypto/sha256.c \
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
    $(ROOT_DIR)hal/driver/common/drv_rtc.c \
    $(ROOT_DIR)hal/driver/common/drv_tim.c \
    $(ROOT_DIR)hal/driver/common/drv_lp_tim.c \
    $(ROOT_DIR)hal/driver/common/drv_rng.c \
    $(ROOT_DIR)hal/driver/om6626/drv_adc.c \
    $(ROOT_DIR)hal/driver/common/drv_i2c.c \
    $(ROOT_DIR)hal/driver/common/drv_spi.c \
    $(ROOT_DIR)hal/driver/om6626/drv_pinmux.c \
    $(ROOT_DIR)hal/driver/om6626/drv_pmu.c \
    $(ROOT_DIR)hal/driver/om6626/drv_rcc.c \
    $(ROOT_DIR)hal/driver/om6626/drv_calib.c \
    $(ROOT_DIR)hal/driver/om6626/drv_calib_repair.c \
    $(ROOT_DIR)hal/device/om6626/GCC/system_asm.S \
    $(ROOT_DIR)hal/device/om6626/GCC/startup.S \
    $(ROOT_DIR)hal/device/om6626/system.c \
    $(ROOT_DIR)projects/freeink/boot/src/boot.c

INCLUDES = \
    $(ROOT_DIR)bsp/include \
    $(ROOT_DIR)common/ \
    $(ROOT_DIR)components/shell/ \
    $(ROOT_DIR)components/eeprom/ \
    $(ROOT_DIR)components/nvds/ \
    $(ROOT_DIR)components/evt/ \
    $(ROOT_DIR)components/crypto/ \
    $(ROOT_DIR)components/pm \
    $(ROOT_DIR)components/mbr \
    $(ROOT_DIR)components/trace \
    $(ROOT_DIR)hal/CMSIS/Core/Include \
    $(ROOT_DIR)hal/driver/include \
    $(ROOT_DIR)hal/device/include \
    $(ROOT_DIR)include \
    $(ROOT_DIR)projects/freeink/boot/src \
    $(ROOT_DIR)projects/freeink/src

LDSCRIPT = $(LINKER_SCRIPT_GEN)
LDEXTFILE = $(CUR_ROMLIB_DIR)GCC/linker_ble_1peripheral.mk
LDDIR  = $(CUR_ROMLIB_DIR)GCC

include $(ROOT_DIR)tools/build/rules_prj.mk
