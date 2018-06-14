/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <aos/log.h>
#include <aos/network.h>
//#include "coap_transport.h"
#include "iot_import_coap.h"

#define TRANSPORT_ADDR_LEN 16

#ifndef IP_PKTINFO
#define IP_PKTINFO   IP_MULTICAST_IF
#endif

#ifndef IPV6_PKTINFO
#define IPV6_PKTINFO IPV6_V6ONL
#endif

#define NETWORK_ADDR_LEN      (16)

