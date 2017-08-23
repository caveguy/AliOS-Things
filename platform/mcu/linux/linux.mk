HOST_OPENOCD := linux

NAME := linuximpl
ARCH_LINUX := ../../arch/linux/
no_with_lwip ?= 1

GLOBAL_INCLUDES += . $(ARCH_LINUX)

$(NAME)_COMPONENTS  := hal vflash netmgr framework.common  modules.fs.kv

ifeq ($(openssl),1)
GLOBAL_LDFLAGS += -lssl -lcrypto
else
$(NAME)_COMPONENTS += mbedtls
endif

ifeq ($(gcov),1)
GLOBAL_DEFINES += GCOV_ENABLE
GLOBAL_CFLAGS  += -fprofile-arcs -ftest-coverage
GLOBAL_LDFLAGS += --coverage
endif

ifeq ($(gprof),1)
GLOBAL_CFLAGS += -pg
GLOBAL_LDFLAGS += -pg
endif

$(NAME)_INCLUDES    += .
GLOBAL_INCLUDES     += include include/yos csp/lwip/include
GLOBAL_LDFLAGS      += -lpthread -lm -lrt
GLOBAL_DEFINES      += CONFIG_YOS_RHINO_MMREGION
GLOBAL_DEFINES      += CONFIG_YSH_CMD_DUMPSYS
GLOBAL_CFLAGS       += -Wall -Wno-missing-field-initializers -Wno-strict-aliasing -Wno-address -Wno-unused-result
GLOBAL_DEFINES      += CSP_LINUXHOST
GLOBAL_DEFINES      += CONFIG_LOGMACRO_DETAILS

# arch linux
$(NAME)_SOURCES := $(ARCH_LINUX)/cpu_impl.c
$(NAME)_SOURCES += $(ARCH_LINUX)/swap.S
# mcu
$(NAME)_SOURCES     += main/arg_options.c
$(NAME)_SOURCES     += main/main.c
$(NAME)_SOURCES     += main/hw.c
$(NAME)_SOURCES     += main/wifi_port.c
$(NAME)_SOURCES     += main/ota_port.c
$(NAME)_SOURCES     += main/trace_task.c
$(NAME)_SOURCES     += soc/soc_impl.c
$(NAME)_SOURCES     += soc/hook_impl.c
$(NAME)_SOURCES     += soc/trace_impl.c
$(NAME)_SOURCES     += soc/trace_hal.c
$(NAME)_SOURCES     += soc/fifo.c
$(NAME)_SOURCES     += soc/uart.c

ifeq ($(linux80211),1)
$(NAME)_SOURCES     += csp/wifi/common.c
$(NAME)_SOURCES     += csp/wifi/linux.c
$(NAME)_SOURCES     += csp/wifi/osdep.c
$(NAME)_SOURCES     += csp/wifi/mesh.c
$(NAME)_SOURCES     += csp/wifi/radiotap/radiotap.c
$(NAME)_DEFINES     += LINUX_MESH_80211
$(NAME)_CFLAGS      += -Wno-unused-but-set-variable
endif

ifneq (,$(filter protocols.net,$(COMPONENTS)))
$(NAME)_SOURCES     += \
    csp/lwip/netif/delif.c \
    csp/lwip/netif/fifo.c \
    csp/lwip/netif/list.c \
    csp/lwip/netif/tapif.c \
    csp/lwip/netif/tcpdump.c \
    csp/lwip/netif/tunif.c

$(NAME)_SOURCES     += csp/lwip/lwip_linuxhost.c
endif
