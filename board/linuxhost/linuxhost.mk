
NAME := board_linuxhost

MODULE              := 1062
HOST_ARCH           := linux
HOST_MCU_FAMILY     := linux

$(NAME)_COMPONENTS  := yloop vfs wrapper hal log vcall ysh

$(NAME)_INCLUDES    += .
GLOBAL_INCLUDES     += include
GLOBAL_LDFLAGS      += -lpthread -lm -lcrypto
GLOBAL_DEFINES      += CONFIG_YOS_RHINO_MMREGION

$(NAME)_SOURCES     := main/arg_options.c
$(NAME)_SOURCES     += main/main.c
$(NAME)_SOURCES     += main/hw.c
$(NAME)_SOURCES     += main/sensor.c
$(NAME)_SOURCES     += main/crypto_impl.c
$(NAME)_SOURCES     += csp/csp_rhino.c
$(NAME)_SOURCES     += soc/soc_impl.c
$(NAME)_SOURCES     += soc/hook_impl.c
$(NAME)_SOURCES     += soc/ysh_impl.c
