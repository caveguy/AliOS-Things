NAME := board_pca10056


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := pca10056
SUPPORT_BINS         := no
HOST_MCU_NAME        := pca10056

GLOBAL_INCLUDES += .

BLUETOOTH_INC_CONTROL := 1

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP BOARD_PCA10056 CONFIG_GPIO_AS_PINRESET FLOAT_ABI_HARD NRF52840_XXAA CONFIG_SOC_SERIES_NRF52X CONFIG_CLOCK_CONTROL_NRF5_K32SRC_RC=y CONFIG_CLOCK_CONTROL_NRF5_K32SRC_250PPM=y  CONFIG_BT_CTLR_WORKER_PRIO=0 CONFIG_BT_CTLR_JOB_PRIO=0 CONFIG_DEVICE_POWER_MANAGEMENT=y CONFIG_BT_CTLR_LE_ENC=y CONFIG_BOARD_NRF52840_PCA10056=1  

GLOBAL_DEFINES_TMP += CONFIG_BT_CTLR_PHY_2M CONFIG_BT_CTLR_XTAL_ADVANCED=1 CONFIG_BT_CTLR_XTAL_THRESHOLD=5168 CONFIG_TINYCRYPT_AES=1 CONFIG_BT_CTLR_MIN_USED_CHAN=1 CONFIG_BT_CTLR_PHY=1 CONFIG_BT_CTLR_LE_ENC=1 CONFIG_BT_CTLR_SCAN_REQ_NOTIFY=1 CONFIG_CLOCK_CONTROL_NRF5_K32SRC_XTAL=1 CONFIG_BT_CTLR_CONN_PARAM_REQ=1 CONFIG_BT_CTLR_RX_BUFFERS=1 CONFIG_BT_CTLR_CHAN_SEL_2=1

GLOBAL_DEFINES += GLOBAL_DEFINES_TMP

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_pca10056
CONFIG_SYSINFO_DEVICE_NAME := pca10056

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"


GLOBAL_CFLAGS  +=   -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52  -DNRF52_PAN_74 -DCONFIG_POLL -DCONFIG_BT_BROADCASTER


# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
#EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

# Define default component testcase set
ifeq (, $(findstring yts, $(BUILD_STRING)))
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=1
TEST_COMPONENTS += basic api wifi_hal rhino vcall kv yloop alicrypto cjson digest_algorithm hashtable
else
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0
endif
