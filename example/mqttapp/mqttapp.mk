ifeq (1,${BINS})
	GLOBAL_CFLAGS += -DSYSINFO_OS_BINS
endif
CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
	GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

NAME := mqttapp

GLOBAL_DEFINES      += ALIOT_DEBUG IOTX_DEBUG
CONFIG_OTA_CH = mqtt
ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)
$(NAME)_SOURCES     := mqtt-example-b_l475e.c
else
$(NAME)_SOURCES     := mqtt-example.c
endif

$(NAME)_INCLUDES := ../../../../utility/cjson/include

$(NAME)_COMPONENTS := cli connectivity.mqtt cjson

LWIP := 0
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
