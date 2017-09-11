/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_WIFI_HAL_H
#define UR_WIFI_HAL_H

enum {
    WIFI_DISCOVERY_TIMEOUT           = 75,     /* ms */
    WIFI_ATTACH_REQUEST_TIMEOUT      = 1000,   /* ms */
    WIFI_SID_REQUEST_TIMEOUT         = 3000,   /* ms */
    WIFI_LINK_REQUEST_MOBILE_TIMEOUT = 1000,     /* ms */
    WIFI_LINK_REQUEST_TIMEOUT        = 30000,   /* ms */
#ifndef CONFIG_AOS_DDA
    WIFI_ADVERTISEMENT_TIMEOUT       = 20000,  /* ms, 20 seconds */
    WIFI_NEIGHBOR_ALIVE_TIMEOUT      = 240000, /* ms, 4 mins */
    WIFI_NET_SCAN_TIMEOUT            = 600000, /* ms, 10 mins */
#else
    WIFI_ADVERTISEMENT_TIMEOUT       = 4000,  /* ms, 4 seconds */
    WIFI_NEIGHBOR_ALIVE_TIMEOUT      = 24000, /* ms, 24 seconds */
    WIFI_NET_SCAN_TIMEOUT            = 60000, /* ms, 1 min */
#endif
    WIFI_MIGRATE_WAIT_TIMEOUT        = 5 * WIFI_ADVERTISEMENT_TIMEOUT,
    WIFI_NOTIFICATION_TIMEOUT        = 60000,  /* ms, 1 mins */
    WIFI_ADDR_CACHE_ALIVE_TIMEOUT    = 3,
};

#endif  /* UR_WIFI_HAL_H */
