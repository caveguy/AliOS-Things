NAME := mbedtls

DEBUG := no

$(NAME)_TYPE := share

GLOBAL_INCLUDES     += include

$(NAME)_CFLAGS      += -Wall -Werror -Os

ifeq ($(DEBUG), yes)
$(NAME)_DEFINES     += CONFIG_SSL_DEBUG
endif

$(NAME)_COMPONENTS  := alicrypto

ifeq ($(HOST_ARCH), linux)
ifeq ($(LWIP), 1)
$(NAME)_DEFINES     += LWIP_ENABLED
endif
else
$(NAME)_DEFINES     += LWIP_ENABLED
endif

PLATFORM := linuxhost
ifeq ($(HOST_ARCH), linux)

PLATFORM := linuxhost
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedtls.a

ifeq (1,$(with_lwip))
$(info using lwip version mbedtls)
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedtls.a.lwip
endif

else ifeq ($(HOST_ARCH), armhflinux)

PLATFORM := armhflinux
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedtls.a

else ifeq ($(HOST_ARCH), ARM968E-S)

PLATFORM := mk108
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedtls.a

else ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)

$(NAME)_DEFINES          += MBEDTLS_NET_ALT_UART
$(NAME)_PREBUILT_LIBRARY := lib/b_l475e/libmbedtls.a

else

$(error "not find correct platform!")

endif

$(NAME)_SOURCES     := mbedtls_net.c
$(NAME)_SOURCES     += mbedtls_ssl.c

